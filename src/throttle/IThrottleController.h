#ifndef __ITHROTTLE_CONTROLLER_H__
#define __ITHROTTLE_CONTROLLER_H__

#include "VescUart.h"

class IThrottleController
{
public:
    virtual void init(VescUart *vesc) = 0;
    virtual void loop() = 0;
};

#endif // __ITHROTTLE_CONTROLLER_H__