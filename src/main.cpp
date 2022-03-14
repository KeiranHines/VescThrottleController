#include "VescUart.h"
#include "config.h"
#include "BleServer.h"
#include "utils.h"

VescUart vesc(VESC_SERIAL_NUM);

#ifdef DEBUG
HardwareSerial *debugSerial = new HardwareSerial(DEBUG_SERIAL_NUM);
#endif // DEBUG

BleServer *bleServer = new BleServer();

#ifdef THROTTLE_CONTROLLER
#ifdef BUTTON_THROTTLE
#include "throttle/ButtonController.h"
IThrottleController *throttle = new ButtonController();
#endif // BUTTON_THROTTLE
#endif // THROTTLE_CONTROLLER

void setup()
{
  vesc.begin(VESC_BAUD_RATE, SERIAL_8N1);
  delay(50);
#ifdef DEBUG
  debugSerial->begin(DEBUG_BAUD);
#endif
  bleServer->init(&vesc);
#ifdef THROTTLE_CONTROLLER
  throttle->init(&vesc);
#endif // THROTTLE_CONTROLLER
}

void loop()
{
  bleServer->loop();
  vesc.alive();
#ifdef THROTTLE_CONTROLLER
  throttle->loop();
#endif // THROTTLE_CONTROLLER
}