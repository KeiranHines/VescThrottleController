#ifndef ITHROTTLE_CONTROLLER_H
#define ITHROTTLE_CONTROLLER_H

#include "VescUart.h"

class IThrottleController
{
public:
    virtual void loop() = 0;
};

#endif // ITHROTTLE_CONTROLLER_H