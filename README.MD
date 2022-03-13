# ESP Vesc

## About
This library is enables an ESP32 to act as a BLE <-> UART bridge for a VESC based ESC.
Operating modes include bridge only, or being able to inject a throttle signal as well. 

## Supported throttle controllers
* Button controller
    * A simple toggle switch to switch between low, medium and overdrive throttle positions, with or without brake support

## Support Control types
* COMM_SET_CURRENT_REL
    * Relative current control between -1 - 1. 