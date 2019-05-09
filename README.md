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
* 4+ 220k resistors (but get a good amount of them to keep on hand) - For our MIDI connections
* 7+ 1M resistors (one for each piezo input you're receiving, but like above, have some extra)

