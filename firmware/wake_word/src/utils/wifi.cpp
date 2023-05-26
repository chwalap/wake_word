#include "wifi.h"
#include <WiFi.h>
#include "led/led_driver.h"
#include "wifi_credentials.h"

extern led_driver_ptr led;

void wifi_connect()
{
  WiFi.disconnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
    vTaskDelay(pdMS_TO_TICKS(30));

  led->next_init_step();
}
