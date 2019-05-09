# Arduino Midi Drum Kit

I have a Rock Band 4 drum kit with cymbals for the Playstation 4. While the pads work fairly well, the cymbals are total trash. They're _somewhat functional_ for some songs, occasinally dropping or double-hitting notes on slow songs, but anything that requires fast hit of the cymbals or high hats, the cymbals drop a majority of notes to be useful.

This Arduino C++ code takes over the original Rock Band "brain", handles all of the analog data from the piezos, and outputs to MIDI. Instead of relying on the thresholds set in the original hardware, we can control, monitor and tweak thresholds or outputs in our Arduino brain! The result is a kit that's far more responsive and can be used on your computer to provide MIDI input for your music.

## Resources

The majority of this code is based on Evan Kale's Arduino Midi Drums: https://github.com/evankale/ArduinoMidiDrums/blob/master/ArduinoMidiDrums.ino

Our code base changes the MIDI notes, supports multiple Rock Band pedals which are simple On (1) or Off (0), and also adds better velocity scaling.

## Requirements

* Rock Band Drum kit - Any kit works (PS2, PS3, Xbox, Wii, etc.) provided the pads work. If you have the cymbals, even better. 

- OR -

* Guitar Hero World Tour Drum Kit

- OR -

* A can-do attitude if you want to make your own using wood or other materials. But you'll need Piezo sensors.

#### Rock Band Pro Drums

If you plan on *playing* Rock Band 3 or 4 with this modified or custom kit, you'll also need:

* Rock Band 3 Pro MIDI Adapter

If you're on the Xbox One, you'll need the adapter above AND possibly the PDP Wired Legacy Adapter, which is unfortunately very difficult to find, and if you can find one, is also very expensive. 

If you're on the Playstation 4, you don't need any other special pieces aside from the Rock Band 3 Pro MIDI Adapter.

#### Electronics

If you're using the Rock Band drum kit with 3 cymbals OR want a custom drum kit with more than 6 pads and cymbals, you'll want the:
* Arduino Mega

If you're using 6 or less piezos (pads/cymbals), I _still_ recommend the Mega for easier debugging, more memory, multiple Tx/Rx interfaces, future enhancements, etc. But the Arduino Uno will also work. Just know that the Uno only supports 1 serial monitor and maximum 6 analog inputs

* MIDI female adapters - You'll use 1 as a MIDI Out but you can use more for debugging
* MIDI cables
* 3.5mm (1/8") audio jacks for breadboards
* Breadboards
* Wires, bridges, solder, soldering gun, etc.
* 4+ 220 ohm resistors (but get a good amount of them to keep on hand) - For our MIDI connections
* 7+ 1M ohm resistors (one for each piezo input you're receiving, but like above, have some extra)

## How It Works

Each pad or cymbal in every Rock Band or Guitar Hero drum kit uses a single piezo sensor, often in the center of the pad or the zone where you're going to strike more often (in a cymbal, that may be on one edge.) A piezo sensor sends a voltage of varying strength depending on how soft or hard you hit it. 

In the Arduino, reading analog input returns a value from 0-1023. The higher the value, the harder the sensor was hit. While the sensors are great at detecting hits, they're also really good at detecting vibrations from nearby sources, such as other pads or cymbals. Therefore, we apply a threshold to ignore any values below a certain number. For instance, we may want to set a threshold of 100 for our pads after discovering that hitting the adjacent pads don't register values above 100.

MIDI in a very simple explanation sends data to another device in a standard form. A piano may send data depending on what key number was pressed (e.g. 60), key note (e.g. C4), how hard the note was hit (values between 1 and 127), and whether the note is still being held down or if it was released. The device receiving this data will be able to play notes on this standardized information.

Using what we've learned, we can scale the value we have from our Piezo sensor (100-1023) to the MIDI note velocity (1-127). In the `scaleVelocity()` function, we return a value between 1-127 that's proportionate to the original sensor value we have. The result is a velocity sensitive drum pad! The softer you hit the pad, the quieter the sound is (for instance, a soft snare hit). The harder you hit it, the louder the sound.

### Caveats

We can stop there and that's generally good enough for most people. But it's worth noting that unlike real drums where you can get an equal volume of sound if you strike the head of the drum or cymbal in any place with the same force, you _can't really get_ the same effect with the piezo sensor. If you strike softly directly in the center of the pad, where the piezo sensor is located, that piezo sensor is going to detect a *very high* value. But if you strike with the same force a couple of inches away, that sensor suddenly returns a very low reading. That can result in a drum kit that sounds wildly inconsistent depending where on the kit you hit it, even if it's all with even force.

What we can try to do to fix the issue is use _multiple_ piezo sensors. For instance, instead of using 1 sensor in the middle of the pad, we can place 3 in a triangle shape around the outside of the center. When we detect a hit on a pad, we can take the values of all 3 sensors and get the average amount of force detected. This accomplishes a couple of things: We take the sensor away from the middle where a majority of the hits may be made. And because we're no longer using 1 sensor for the reading, we get way better velocity control over how we strike the drum pad.

### Issues with Rock Band game

