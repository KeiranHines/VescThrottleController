# ESP Vesc

## About
This library is enables an ESP32 to act as a BLE <-> UART bridge for a VESC based ESC.
Operating modes include bridge only, or being able to inject a throttle signal. Currently only a relative current signal over UART is support, PRs welcome for more throttle controls.

The throttle controller is designed to use a simple pushbutton to toggle between an off, low, and medium setting with a short press with the option to go to an overdrive power with a long press and hold. Additionally a brake is supported, typically this would be via a hall effect sensor.

## Getting started
This project was developed in PlatformIO rather than the Arduino IDE. 

For details on setup of platform IO see [here](https://docs.platformio.org/en/stable/tutorials/index.html)

## Configuration
Configuration is handled by the configuration headerfile at src/config.h

## Supported throttle controllers
* Button controller
    * A simple toggle switch to switch between low, medium and overdrive throttle positions, with or without brake support

## Support Control types
* COMM_SET_CURRENT_REL
    * Relative current control between [-1,1].

### ESP32s tested:
* Wemos D1 Mini ESP32 clone. 