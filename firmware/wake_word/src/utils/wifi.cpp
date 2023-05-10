#include "wifi.h"
#include <WiFi.h>

const char* SSID = "Smart";
const char* PASSWORD = "dupa123kotki666";

void wifi_connect()
{
  WiFi.disconnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
    vTaskDelay(pdMS_TO_TICKS(30));
}
