
#define NUM_PIEZOS 7 // Number of Pads/Cymbals
#define NUM_FOOT_SWITCHES 1
#define START_SLOT 0     //first analog slot of piezos
#define DIGITAL_START_SLOT 2 // first digital slot for pedals

#define SNARE_THRESHOLD 100 
#define HTOM_THRESHOLD 100
#define MTOM_THRESHOLD 100
#define LTOM_THRESHOLD 100
// ---------------------------
#define HIHAT_THRESHOLD 200
#define RIDECYM_THRESHOLD 200
#define CRASHCYM_THRESHOLD 200

// ===========================
#define PEDAL_THRESHOLD 50 // not really needed, the kick pedal is either 1 or 0.


//MIDI note defines for each trigger
// #define SNARE_NOTE 38
// #define HTOM_NOTE 48
// #define MTOM_NOTE 47
// #define LTOM_NOTE 45
// #define HIHAT_NOTE 42
// #define RIDECYM_NOTE 51
// #define CRASHCYM_NOTE 49
// #define KICK_NOTE 36
#define ALTKICK_NOTE 36
#define HIHAT_CHOKE_NOTE 78

// Rock Band Midi Adapter notes
#define SNARE_NOTE 38;  // Midi value for Red Pad
#define HIHAT_NOTE 22;  // Midi value for Yellow Cymbal
#define HTOM_NOTE 48;  // Midi value for Yellow Pad
#define RIDECYM_NOTE 51;  // Midi value for Blue Cymbal
#define MTOM_NOTE 45;  // Midi value for Blue Pad
#define CRASHCYM_NOTE 49;  // Midi value for Green Cymbal
#define LTOM_NOTE 41;  // Midi value for Green pad
#define KICK_NOTE 33;  // Midi value for Kick pedal

//MIDI defines
#define NOTE_ON_CMD 0x90
#define NOTE_OFF_CMD 0x80
#define MAX_MIDI_VELOCITY 127

//MIDI baud rate
#define SERIAL_RATE 31250

//Program defines
//ALL TIME MEASURED IN MILLISECONDS
#define SIGNAL_BUFFER_SIZE 80
#define PEAK_BUFFER_SIZE 30
#define MAX_TIME_BETWEEN_PEAKS 20
#define MIN_TIME_BETWEEN_NOTES 50

//map that holds the mux slots of the piezos
unsigned short slotMap[NUM_PIEZOS];

//map that holds the respective note to each piezo
unsigned short noteMap[NUM_PIEZOS];

//map that holds the respective threshold to each piezo
unsigned short thresholdMap[NUM_PIEZOS];

//Ring buffers to store analog signal and peaks
short currentSignalIndex[NUM_PIEZOS];
short currentPeakIndex[NUM_PIEZOS];
unsigned short signalBuffer[NUM_PIEZOS][SIGNAL_BUFFER_SIZE];
unsigned short peakBuffer[NUM_PIEZOS][PEAK_BUFFER_SIZE];

boolean noteReady[NUM_PIEZOS];
unsigned short noteReadyVelocity[NUM_PIEZOS];
boolean isLastPeakZeroed[NUM_PIEZOS];

unsigned long lastPeakTime[NUM_PIEZOS];
unsigned long lastNoteTime[NUM_PIEZOS];

// Buffers for foot pedals
short currentFootSwitchIndex[NUM_FOOT_SWITCHES];
unsigned short footSwitchSlotMap[NUM_FOOT_SWITCHES];
unsigned short footSwitchNoteMap[NUM_FOOT_SWITCHES];
unsigned short footSwitchThresholdMap[NUM_FOOT_SWITCHES];

boolean footNoteReady[NUM_FOOT_SWITCHES];
int footSwitchLastNote[NUM_FOOT_SWITCHES];


void setup() {

  // Debug Serial Monitoring
  Serial.begin(19200);
  
  //initialize globals
  for (short i=0; i<NUM_PIEZOS; ++i) {
    currentSignalIndex[i] = 0;
    currentPeakIndex[i] = 0;
    memset(signalBuffer[i],0,sizeof(signalBuffer[i]));
    memset(peakBuffer[i],0,sizeof(peakBuffer[i]));
    noteReady[i] = false;
    noteReadyVelocity[i] = 0;
    isLastPeakZeroed[i] = true;
    lastPeakTime[i] = 0;
    lastNoteTime[i] = 0;    
    slotMap[i] = START_SLOT + i;
  }

  for (short i=0; i<NUM_FOOT_SWITCHES; ++i) {
    Serial.println("Enumerating foot switches");
    footNoteReady[i] = false;
    footSwitchSlotMap[i] = DIGITAL_START_SLOT + i;
    footSwitchLastNote[i] = 0;
    Serial.println(footSwitchSlotMap[i]);
  }

  thresholdMap[0] = SNARE_THRESHOLD;
  thresholdMap[1] = HTOM_THRESHOLD;
  thresholdMap[2] = MTOM_THRESHOLD;
  thresholdMap[3] = LTOM_THRESHOLD;
  thresholdMap[4] = HIHAT_THRESHOLD;
  thresholdMap[5] = RIDECYM_THRESHOLD;
  thresholdMap[6] = CRASHCYM_THRESHOLD;
  //thresholdMap[7] = PEDAL_THRESHOLD;
  //thresholdMap[8] = PEDAL_THRESHOLD;

  footSwitchThresholdMap[0] = PEDAL_THRESHOLD;
  footSwitchThresholdMap[1] = PEDAL_THRESHOLD;
  
  noteMap[0] = SNARE_NOTE;
  noteMap[1] = HTOM_NOTE;
  noteMap[2] = MTOM_NOTE;
  noteMap[3] = LTOM_NOTE;
  noteMap[4] = HIHAT_NOTE;
  noteMap[5] = RIDECYM_NOTE;  
  noteMap[6] = CRASHCYM_NOTE;

  footSwitchNoteMap[0] = KICK_NOTE;
  footSwitchNoteMap[1] = ALTKICK_NOTE;
  //noteMap[7] = KICK_NOTE;
  //noteMap[8] = KICK_NOTE;

  Serial1.begin(SERIAL_RATE);
  Serial2.begin(SERIAL_RATE);


}

void loop() {
  unsigned long currentTime = millis();
  
  for (short i=0; i<NUM_PIEZOS; ++i) {
    //get a new signal from analog read
    unsigned short newSignal = analogRead(slotMap[i]);
    signalBuffer[i][currentSignalIndex[i]] = newSignal;
    
    //if new signal is 0
    if (newSignal < thresholdMap[i]) {
      if (!isLastPeakZeroed[i] && (currentTime - lastPeakTime[i]) > MAX_TIME_BETWEEN_PEAKS) {
        recordNewPeak(i,0);
      } else {
        //get previous signal
        short prevSignalIndex = currentSignalIndex[i]-1;
        if(prevSignalIndex < 0) prevSignalIndex = SIGNAL_BUFFER_SIZE-1;        
        unsigned short prevSignal = signalBuffer[i][prevSignalIndex];
        
        unsigned short newPeak = 0;
        
        //find the wave peak if previous signal was not 0 by going
        //through previous signal values until another 0 is reached
        while (prevSignal >= thresholdMap[i]) {
          if(signalBuffer[i][prevSignalIndex] > newPeak) {
            newPeak = signalBuffer[i][prevSignalIndex];        
          }
          
          //decrement previous signal index, and get previous signal
          prevSignalIndex--;
          if (prevSignalIndex < 0) prevSignalIndex = SIGNAL_BUFFER_SIZE-1;
          prevSignal = signalBuffer[i][prevSignalIndex];
        }
        
        if(newPeak > 0) {
          recordNewPeak(i, newPeak);
        }
      }
  
    }
        
    currentSignalIndex[i]++;
    if (currentSignalIndex[i] == SIGNAL_BUFFER_SIZE) currentSignalIndex[i] = 0;
  }


  for (short i=0; i<NUM_FOOT_SWITCHES; ++i) {
    //get current signal from the digital foot switches
    unsigned short dSignal = digitalRead(footSwitchSlotMap[i]);
      
    if ( dSignal == HIGH  ) {
        footSwitchLastNote[i] = dSignal;
        midiNoteOn(footSwitchNoteMap[i], MAX_MIDI_VELOCITY);
        Serial.println("on!");
    } else if ( ( dSignal == LOW ) && ( footSwitchLastNote[i] != LOW ) ) {
        footSwitchLastNote[i] = dSignal;
        midiNoteOff(footSwitchNoteMap[i], 0);
        Serial.println("off!");
    }

  }

  
}

void recordNewPeak(short slot, short newPeak) {
  isLastPeakZeroed[slot] = (newPeak == 0);
  
  unsigned long currentTime = millis();
  lastPeakTime[slot] = currentTime;
  
  //new peak recorded (newPeak)
  peakBuffer[slot][currentPeakIndex[slot]] = newPeak;
  
  //1 of 3 cases can happen:
  // 1) note ready - if new peak >= previous peak
  // 2) note fire - if new peak < previous peak and previous peak was a note ready
  // 3) no note - if new peak < previous peak and previous peak was NOT note ready
  
  //get previous peak
  short prevPeakIndex = currentPeakIndex[slot]-1;
  if(prevPeakIndex < 0) prevPeakIndex = PEAK_BUFFER_SIZE-1;        
  unsigned short prevPeak = peakBuffer[slot][prevPeakIndex];
   
  if (newPeak > prevPeak && (currentTime - lastNoteTime[slot])>MIN_TIME_BETWEEN_NOTES) {
    noteReady[slot] = true;
    if (newPeak > noteReadyVelocity[slot])
      noteReadyVelocity[slot] = newPeak;
  } else if (newPeak < prevPeak && noteReady[slot]) {
    noteFire(noteMap[slot], scaleVelocity(noteReadyVelocity[slot], thresholdMap[slot]));
    noteReady[slot] = false;
    noteReadyVelocity[slot] = 0;
    lastNoteTime[slot] = currentTime;
  }
  
  currentPeakIndex[slot]++;
  if(currentPeakIndex[slot] == PEAK_BUFFER_SIZE) currentPeakIndex[slot] = 0;  
}

unsigned short scaleVelocity(unsigned short originalVelocity, unsigned short threshold) {
  unsigned short r_min = threshold;
  unsigned short r_max = 1023;
  unsigned short t_min = 1;
  unsigned short t_max = MAX_MIDI_VELOCITY;

  float value = ((float) t_max - (float) t_min) * ((float) originalVelocity - (float) r_min) / ((float) r_max - (float) r_min) + (float) t_min;
  return static_cast<unsigned short>(value);
}

void noteFire(unsigned short note, unsigned short velocity) {
  if(velocity > MAX_MIDI_VELOCITY)
    velocity = MAX_MIDI_VELOCITY;
  
  midiNoteOn(note, velocity);
  midiNoteOff(note, velocity);
}

void midiNoteOn(byte note, byte midiVelocity) {
  Serial1.write(NOTE_ON_CMD);
  Serial1.write(note);
  Serial1.write(midiVelocity);
  Serial2.write(NOTE_ON_CMD);
  Serial2.write(note);
  Serial2.write(midiVelocity);
}

void midiNoteOff(byte note, byte midiVelocity) {
  Serial1.write(NOTE_OFF_CMD);
  Serial1.write(note);
  Serial1.write(midiVelocity);
  Serial2.write(NOTE_OFF_CMD);
  Serial2.write(note);
  Serial2.write(midiVelocity);
}
