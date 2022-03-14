#ifndef CONFIG_H
#define CONFIG_H

#define NO_GLOBAL_SERIAL

#define VESC_NAME "ESP_VESC"
#define VESC_BAUD_RATE 115200 // BAUD rate of vesc
#define VESC_SERIAL_NUM 2

#define DEBUG // Uncomment to enable debug logging over serial.
#ifdef DEBUG
#define DEBUG_BAUD 115200
#define DEBUG_SERIAL_NUM 0
#endif // DEBUG

#define THROTTLE_CONTROLLER // If a throttle controller is present

#ifdef THROTTLE_CONTROLLER
/* General throttle controls */
#define HAS_BRAKE // Comment out to disable braking.
/* Throttle Type */
#define BUTTON_THROTTLE

/* Button Throttle Config */
#ifdef BUTTON_THROTTLE
#ifdef HAS_BRAKE
#define BRAKE_PIN 12 // Brake sensor pin.
#define BRAKE_PERCENT -100
#endif               // HAS_BRAKE
#define BUTTON_PIN 2 // Throttle button pin.

#define SHORT_PRESS 500 // ms
#define LONG_PRESS 1000 // ms
#define OFF_PERCENT 0
#define LOW_PERCENT 30
#define MED_PERCENT 60
#define OVERDRIVE_PERCENT 100

#endif // BUTTON_THROTTLE
#endif // THROTTLE_CONTROLLER

#endif //CONFIG_H