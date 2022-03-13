# Vesc Throttle Controller

## About
This project is designed to enable simple throttle control to a VESC based ESC.
The Arduino based implementation is desiged to communicate to the VESC via the PPM signal wire.

The throttle controller is designed to use a simple pushbutton to toggle between an off, low, and medium setting with a short press with the option to go to an overdrive power with a long press and hold. Additionally a brake is supported, typically this would be via a hall effect sensor. 

## Getting started
This project was developed in PlatformIO rather than the Arduino IDE. 

For details on setup of platform IO see [here](https://docs.platformio.org/en/stable/tutorials/index.html)

## Configuration
Configuration of the power outputs and press delays can be done at the top of the main.cpp file. 

```
// CONFIG PARAMS
#define HAS_BRAKE // Comment out to disable braking.
// #define DEBUG // Uncomment to enable debug logging over serial.

#ifdef HAS_BRAKE
#define BRAKE_PIN 12 // Brake sensor pin.
#endif
#define PPM_PIN 3    // PPM Output pin.
#define BUTTON_PIN 2 // Throttle button pin.

#define SHORT_PRESS 1000 // ms
#define LONG_PRESS 3000  // ms

#define PPM_MIN 1000 // us
#define PPM_MAX 2000 // us

/**
 * Modes for throttle steps.
 * Edit the numbers to affect the percentage output of each mode.
 */
enum Mode
{
#ifdef HAS_BRAKE
  brake = -100, // Braking percentage
#endif
  null = -1,
  off = 0,
  low = 50,
  med = 75,
  overdrive = 100
};

// END CONFIG
```

### Arduinos tested:
* Nano clone
* Arduino pro mini 5V 16Mhz

If you have tested other boards, please feel free to let me know and I'll add them to the list. 