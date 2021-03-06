# ToySon
Simple UltraSonic MIDI controller (Arduino based toy)

This project is just an example of using Arduino Nano and HS-SR04 ultrasonic sensor to build a very simple MIDI controller.
Only two electronic components, some wires and PC (Windows prefered) are required to implement this project.
You can use this sketch as a demo sample or simply to entertain yourself :)

## Features

* Compatible with all types of  Arduino boards and clones
* Uses Serial (over USB) to transmit MIDI messages to software synthesizer on PC
* Uses HS-SR04 ultrasonic sensor to control pitch of notes (by hand or using direct TX-RX link)

## Getting Started

1. Use Arduino's Library Manager to install [Arduino MIDI Library](https://github.com/FortySevenEffects/arduino_midi_library)
2. Connect HS-SR04 properly (see [Guide](https://github.com/rnd64/ToySon/blob/master/ToySon%20Guide.pdf) and [sketch diagram](https://github.com/rnd64/ToySon/blob/master/Sketch/USMC_Sketch.png) )
3. Download [ToySon Sketch] (https://github.com/rnd64/ToySon/blob/master/Sketch/USMC.ino) to Arduino
4. Play!

## Documentation
* [ToySon Project Guide](https://github.com/rnd64/ToySon/blob/master/ToySon%20Guide.pdf)

## Contact

* To report any bugs or simply request support, please create an issue [here](https://github.com/rnd64/ToySon/issues/new).

## License

* Free
