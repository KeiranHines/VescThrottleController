#include "VescUart.h"
#include "config.h"
#include "BleServer.h"
#include "utils.h"

static VescUart *vesc;
static BleServer *bleServer;

#ifdef DEBUG
HardwareSerial *debugSerial;
#endif // DEBUG

#ifdef THROTTLE_CONTROLLER
#ifdef BUTTON_THROTTLE
#include "throttle/ButtonController.h"
IThrottleController *throttle;
#endif // BUTTON_THROTTLE
#endif // THROTTLE_CONTROLLER

void setup()
{
  vesc = new VescUart(VESC_SERIAL_NUM);
  vesc->begin(VESC_BAUD_RATE, SERIAL_8N1);
  delay(50);
#ifdef DEBUG
  debugSerial = new HardwareSerial(DEBUG_SERIAL_NUM);
  debugSerial->begin(DEBUG_BAUD);
#endif
  bleServer = new BleServer(vesc);
#ifdef THROTTLE_CONTROLLER
#ifdef BUTTON_THROTTLE
  throttle = new ButtonController(vesc);
#endif // BUTTON_THROTTLE
#endif // THROTTLE_CONTROLLER
}

void loop()
{
  bleServer->loop();
  vesc->alive();
#ifdef THROTTLE_CONTROLLER
  throttle->loop();
#endif // THROTTLE_CONTROLLER
}