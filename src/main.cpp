#include "VescUart.h"
#include "config.h"
#include "BleServer.h"

VescUart vesc(2);

BleServer *bleServer = new BleServer();

#ifdef THROTTLE_CONTROLLER
#ifdef BUTTON_THROTTLE
#include "throttle/ButtonController.h"
IThrottleController *throttle = new ButtonController();
#endif // BUTTON_THROTTLE
#endif // THROTTLE_CONTROLLER

void setup()
{
  vesc.begin(VESC_BAUD_RATE, SERIAL_8N1, VESC_RX_PIN, VESC_TX_PIN, false);
  delay(50);
  Serial.begin(115200);
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