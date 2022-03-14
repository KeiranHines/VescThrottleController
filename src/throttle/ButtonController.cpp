#include "ButtonController.h"
#include "utils.h"

#ifdef BUTTON_THROTTLE
#ifdef HAS_BRAKE
constexpr int MIN_INPUT = -100;
constexpr int MIN_OUTPUT = -1;
#else
constexpr int MIN_INPUT = 0;
constexpr int MIN_OUTPUT = 0;
#endif

constexpr int DEBOUCNCE = 50;

Mode nextMode(Mode m);
ButtonController::ButtonController(VescUart *vesc)
{
    this->vesc = vesc;
    button->setDebounceTime(DEBOUCNCE);
#ifdef HAS_BRAKE
    brakeSensor->setDebounceTime(DEBOUCNCE);
#endif // HAS_BRAKE
}

void ButtonController::loop()
{
    button->loop(); // Needed to update button state.

#ifdef HAS_BRAKE
    brakeSensor->loop();
    if (brakeSensor->isPressed())
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
    if (brakeSensor->isReleased())
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
    unsigned long ms = millis();

    if (button->isPressed())
    {
        // Start held timer.
        shortPressTimestamp = ms + SHORT_PRESS;
        longPressTimestamp = ms + LONG_PRESS;
    }
    if (button->isReleased())
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

    if (lastMode == null && ms > shortPressTimestamp)
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
    if (currentMode != overdrive && ms > longPressTimestamp)
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
#ifdef DEBUG
        debugSerial->print("Setting throttle to: ");
        debugSerial->print(currentMode);
        debugSerial->println("%");
#endif
        // Todo: Support more operating modes than just current_rel. E.g. Duty Cycle.
        vesc->writeCurrentRel(getOutputRel());
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
    default:
        return null;
    }
}
#endif // BUTTON_THROTTLE