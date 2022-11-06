#ifndef BUTTON_CONTROLLER
#define BUTTON_CONTROLLER
#include "IThrottleController.h"
#include "ezButton.h"
#include "config.h"

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

class ButtonController : public IThrottleController
{
public:
    ButtonController(VescUart * vesc);
    void loop();

private:
    float getOutputPercentage();
    float getOutputRel();
    void updateOutput();
    VescUart *vesc;
    Mode lastMode = null;    // Last mode seen in this cylce, used to reset overdrive back to last position.
    Mode currentMode = off;  // Current mode.
    Mode lastWritten = null; // Last written mode to prevent expensive writes.
    ezButton *button = new ezButton(BUTTON_PIN);

    unsigned long shortPressTimestamp = ULONG_MAX;
    unsigned long longPressTimestamp = ULONG_MAX;
#ifdef HAS_BRAKE
    ezButton *brakeSensor = new ezButton(BRAKE_PIN);
#endif
};
#endif // BUTTON_CONTROLLER