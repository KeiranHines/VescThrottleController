#include <Arduino.h>
#include <Servo.h>
#include <ezButton.h>
#include <limits.h>

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

#ifdef HAS_BRAKE
#define MIN_INPUT -100
#else
#define MIN_INPUT 0
#endif

Mode nextMode(Mode m);
void updateOutput();

Mode lastMode = null;    // Last mode seen in this cylce, used to reset overdrive back to last position.
Mode currentMode = off;  // Current mode.
Mode lastWritten = null; // Last written mode to prevent expensive writes.

Servo ppm;
ezButton button(BUTTON_PIN);
#ifdef HAS_BRAKE
ezButton brakeSensor(BRAKE_PIN);
#endif

unsigned long shortPressTimestamp = ULONG_MAX;
unsigned long longPressTimestamp = ULONG_MAX;

void setup()
{
  pinMode(PPM_PIN, OUTPUT);
#ifdef DEBUG
  Serial.begin(115200);
#endif
  button.setDebounceTime(50);
  ppm.attach(PPM_PIN, 1000, 2000);
  ppm.writeMicroseconds(map(0, MIN_INPUT, 100, PPM_MIN, PPM_MAX));
}

void loop()
{
  button.loop(); // Needed to update button state.

#ifdef HAS_BRAKE
  brakeSensor.loop();
  if (brakeSensor.isPressed())
  {
    shortPressTimestamp = ULONG_MAX;
    longPressTimestamp = ULONG_MAX;
    if (currentMode == overdrive)
    {
      // Safety: If brake is triggered while in overdrive disable overdrive on restart
      currentMode = lastMode;
    }
    lastMode = currentMode;
    currentMode = brake;
    updateOutput();
  }
  if (brakeSensor.isReleased())
  {
    currentMode = lastMode;
    lastMode = null;
    updateOutput();
  }
  if (currentMode == brake)
  {
    // If braking don't check throttle contol
    return;
  }
#endif

  if (button.isPressed())
  {
    // Start held timer.
    long ms = millis();
    shortPressTimestamp = ms + SHORT_PRESS;
    longPressTimestamp = ms + LONG_PRESS;
  }
  if (button.isReleased())
  {
    if (lastMode == null)
    {
      // Shorter than 1s, turn off.
      currentMode = off;
    }
    if (currentMode == overdrive)
    {
      // was overdriven and released, reset to last state.
      currentMode = lastMode;
    }

    // reset for next press.
    lastMode = null;
    shortPressTimestamp = ULONG_MAX;
    longPressTimestamp = ULONG_MAX;
  }

  if (lastMode == null && millis() > shortPressTimestamp)
  {
    // Short press active first time, cache lastMode and set current mode.
    lastMode = currentMode;
    currentMode = nextMode(currentMode);
    if (currentMode == overdrive)
    {
      // Prevent medium to overdrive transition on short press.
      currentMode = med;
    }
  }
  if (currentMode != overdrive && millis() > longPressTimestamp)
  {
    // Long press activate overdrive until release.
    currentMode = overdrive;
  }
  updateOutput();
}

/**
 * Updates the output to the ppm pin if it has changed.
 */
void updateOutput()
{
  if (lastWritten != currentMode)
  {
    lastWritten = currentMode;
    long output = map(currentMode, MIN_INPUT, 100, PPM_MIN, PPM_MAX);
    ppm.writeMicroseconds(output);
#ifdef DEBUG
    Serial.print("Output is: ");
    Serial.print(currentMode);
    Serial.print("% / ");
    Serial.print(output);
    Serial.println("us");
#endif
  }
}

/**
 * Helper method for transitions from one mode to the next.
 */
Mode nextMode(Mode m)
{
  switch (m)
  {
  case null:
    return off;
  case off:
    return low;
  case low:
    return med;
  case med:
    return overdrive;
  case overdrive:
    return null;
  default:
    return null;
  }
}