#include "serial.h"
#include "config.h"
#include "led/led_driver.h"

#include <Arduino.h>

extern led_driver_ptr led;

void init_serial()
{
  // start serial port
  Serial.begin(115200);
  auto tSerialConnectionStart = millis();
  while (!Serial && millis() - tSerialConnectionStart < SERIAL_CONNECTION_TIMEOUT)
    vTaskDelay(pdMS_TO_TICKS(10));

  led->next_init_step();
}
