#include "watchdog.h"
#include "config.h"
#include "led/led_driver.h"

#include <esp_task_wdt.h>

extern led_driver_ptr led;

void init_watchdog()
{
  // just don't panic!
  esp_task_wdt_init(WATCHDOG_TIMEOUT, true);
  led->next_init_step();
}
