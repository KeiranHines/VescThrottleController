#ifndef __BUTTON_CONTROLLER__
#define __BUTTON_CONTROLLER__
#include "IThrottleController.h"

class ButtonController : public IThrottleController
{
public:
    ButtonController();
    void init(VescUart *vesc);
    void loop();

private:
    float getOutputPercentage();
    float getOutputRel();
    void updateOutput();
    VescUart *vescUart;
};
#endif // __BUTTON_CONTROLLER__