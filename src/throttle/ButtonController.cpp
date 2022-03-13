#include "ButtonController.h"
#include "ezButton.h"
#include "utils.h"
#include "config.h"

#ifdef BUTTON_THROTTLE
#ifdef HAS_BRAKE
#define MIN_INPUT -100
#define MIN_OUTPUT -1
#else
#define MIN_INPUT 0
#define MIN_OUTPUT 0
#endif

/**
 * Modes for throttle steps.
 */
enum Mode
{
#ifdef HAS_BRAKE
    brake = BRAKE_PERCENT, // Braking percentage
#endif
    null = -1,
    off = OFF_PERCENT,
    low = LOW_PERCENT,
    med = MED_PERCENT,
    overdrive = OVERDRIVE_PERCENT
};

Mode lastMode = null;    // Last mode seen in this cylce, used to reset overdrive back to last position.
Mode currentMode = off;  // Current mode.
Mode lastWritten = null; // Last written mode to prevent expensive writes.

Mode nextMode(Mode m);

ezButton button(BUTTON_PIN);

#ifdef HAS_BRAKE
ezButton brakeSensor(BRAKE_PIN);
#endif

unsigned long shortPressTimestamp = ULONG_MAX;
unsigned long longPressTimestamp = ULONG_MAX;

ButtonController::ButtonController() {}

void ButtonController::init(VescUart *vesc)
{
    vescUart = vesc;
    button.setDebounceTime(50);
}

void ButtonController::loop()
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

float ButtonController::getOutputPercentage()
{
    return currentMode;
}

float ButtonController::getOutputRel()
{
    return mapFloat(currentMode, MIN_INPUT, 100, MIN_OUTPUT, 1);
}

/**
 * Updates the output to the ppm pin if it has changed.
 */
void ButtonController::updateOutput()
{
    if (lastWritten != currentMode)
    {
        lastWritten = currentMode;
        // Todo: Support more operating modes than just current_rel. E.g. Duty Cycle.
        vescUart->writeCurrentRel(getOutputRel());
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
#endif // BUTTON_THROTTLE