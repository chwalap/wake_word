#include <Arduino.h>
#include <esp_task_wdt.h>

#include "config.h"
#include "utils/wifi.h"
#include "audio_input/i2s_sampler.h"
#include "command_detector/command_detector.h"
#include "command_detector/detection_context.h"
#include "api/google_speech_api.h"

// todo: recompile idf libraries and force mbedtls to use psram

i2s_sampler_ptr sampler;
command_detector_ptr detector;
google_speech_api_ptr speech_api;
detection_context_ptr detection_ctx;
TaskHandle_t detector_task_handle;

void setup()
{
  // just don't panic!
  esp_task_wdt_init(10, false);

  // start serial port
  Serial.begin(115200);
  while (!Serial)
    vTaskDelay(pdMS_TO_TICKS(10));

  // connect to wifi
  wifi_connect();

  // create all components
  sampler = std::make_shared<i2s_sampler>();
  detector = std::make_shared<command_detector>();
  speech_api = std::make_unique<google_speech_api>();
  detection_ctx = std::make_unique<detection_context>();

  xTaskCreatePinnedToCore(
    [](void*)
    {
      while (true)
      {
        uint32_t ulNotValue = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));
        if (ulNotValue > 0)
        {
          if (detection_ctx->m_is_detector_running)
          {
            detector->detect_wake_word();
          }
          else
          {
            speech_api->speech_to_text();
          }
        }

        taskYIELD();
      }
    }, "Detector Task", 8192, nullptr, 1, &detector_task_handle, 1
  );

  sampler->start();
  Serial.println("Powiedz \"komputer\" i co tam jeszcze chcesz!");
}

void loop()
{
  const auto free_ram = esp_get_free_heap_size();
  // Serial.printf("Free HEAP: %u\tSPRAM: %u\tRAM: %d\n", ESP.getFreeHeap(), ESP.getFreePsram(), free_ram);
  vTaskDelay(pdMS_TO_TICKS(1000));
}
