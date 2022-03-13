#include <Arduino.h>
#include <Servo.h>
#include <ezButton.h>
#include <limits.h>

// CONFIG PARAMS
#define HAS_BRAKE // Comment out to disable braking.
#define DEBUG     // Uncomment to enable debug logging over serial.
#define DEBUG_BAUD 115200

#ifdef HAS_BRAKE
#define BRAKE_PIN 12 // Brake sensor pin.
#endif
#define PPM_PIN 3    // PPM Output pin.
#define BUTTON_PIN 2 // Throttle button pin.

#define SHORT_PRESS 500          // ms
#define LONG_PRESS 1000          // ms
#define OVERDRIVE_OFF_DELAY 5000 // ms

#define OVERDRIVE_EXPIRY_TO_LOW 2 // Number of presses after overdrive to go to low
#define OVERDRIVE_EXPIRY_TO_MED 3 // Number of presses after overdrive to go to med, medium should always be higher

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
  low = 30,
  med = 60,
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
bool handleBrake();
void handleOverdriveExpiry();
void handleStandardOperation();
void startOverdriveTimer();

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
unsigned long overdriveExpiredTimestamp = ULONG_MAX;
bool overdriveExpiryActive = false;

void setup()
{
  pinMode(PPM_PIN, OUTPUT);
#ifdef DEBUG
  Serial.begin(DEBUG_BAUD);
#endif
#ifdef HAS_BRAKE
  brakeSensor.setDebounceTime(50);
#endif
  button.setDebounceTime(50);
  button.setCountMode(COUNT_RISING);
  ppm.attach(PPM_PIN, PPM_MIN, PPM_MAX);
  ppm.writeMicroseconds(map(0, MIN_INPUT, 100, PPM_MIN, PPM_MAX));
}

void loop()
{
  button.loop(); // Needed to update button state.

  if (handleBrake())
  {
    return;
  }

  if (overdriveExpiryActive)
  {

    handleOverdriveExpiry();
    return;
  }
  handleStandardOperation();
  updateOutput();
}

/**
 * Handles the brake trigger. Brake is expected to be pressed in when NOT active
 * Releasing the brake sensor activates the braking mode.
 * @returns true if the brake is still active, false otherwise.
 */
bool handleBrake()
{
#ifdef HAS_BRAKE
  brakeSensor.loop();
  if (brakeSensor.isReleased())
  {
    shortPressTimestamp = ULONG_MAX;
    longPressTimestamp = ULONG_MAX;
    if (currentMode == overdrive)
    {
      // Safety: If brake is triggered while in overdrive disable overdrive on restart
      startOverdriveTimer();
    }
    lastMode = currentMode;
    currentMode = brake;
    updateOutput();
  }
  if (brakeSensor.isPressed())
  {
    currentMode = lastMode;
    lastMode = null;
    updateOutput();
  }
  if (currentMode == brake)
  {
    // If braking don't check throttle contol
    updateOutput();
    return true;
  }
#endif
  return false;
}

/**
 * Handles if the override expiry is running. Disables the overdrive if either the timers has expired
 * or the user has pressed the button three times, indicating they want a medium throttle.
 */
void handleOverdriveExpiry()
{
  if (millis() > overdriveExpiredTimestamp || button.getCount() >= OVERDRIVE_EXPIRY_TO_MED)
  {
    overdriveExpiredTimestamp = ULONG_MAX;
    int count = button.getCount();
    switch (count)
    {
    case 0:
      currentMode = lastMode;
      break;
    case OVERDRIVE_EXPIRY_TO_LOW:
      currentMode = low;
      break;
    case OVERDRIVE_EXPIRY_TO_MED:
      currentMode = med;
      break;
    default:
      currentMode = off;
    }
    lastMode = button.isReleased() ? currentMode : null; // Standard operation will clean current to null on the same release
    overdriveExpiredTimestamp = ULONG_MAX;
    overdriveExpiryActive = false;
#ifdef DEBUG
    Serial.print("Overdrive expiry finished. Count: ");
    Serial.print(count);
    Serial.print(" ");
#endif
    button.resetCount();
  }
}

/**
 * Handles the standard operation being able to select a throttle mode or overdrive active/deactive
 */
void handleStandardOperation()
{
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
      startOverdriveTimer();
    }
    else
    {
      // reset for next press.
      lastMode = null;
    }
    
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

void startOverdriveTimer()
{
  // was overdriven and released, Start overdrive delay
  overdriveExpiredTimestamp = millis() + OVERDRIVE_OFF_DELAY;
  overdriveExpiryActive = true;
  button.resetCount();
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