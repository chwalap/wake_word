#include <Arduino.h>
#include <esp_task_wdt.h>

#include "config.h"
#include "utils/wifi.h"
#include "audio_input/i2s_sampler.h"
#include "command_detector/command_detector.h"
#include "command_detector/detection_context.h"
#include "api/google_speech_api.h"
#include "api/chat_gpt.h"
#include "audio_output/speech_synthesizer.h"
#include "led/led_driver.h"

// todo: recompile idf libraries and force mbedtls to use psram

i2s_sampler_ptr sampler;
command_detector_ptr detector;
google_speech_api_ptr speech_api;
detection_context_ptr detection_ctx;
speech_synthesizer_ptr synthesizer;
chat_gpt_ptr chat;
led_driver_ptr led;

TaskHandle_t detector_task_handle;

void setup()
{
  led = std::make_unique<led_driver>();

  // just don't panic!
  esp_task_wdt_init(10, false);
  led->next_init_step();

  // start serial port
  Serial.begin(115200);
  while (!Serial)
    vTaskDelay(pdMS_TO_TICKS(10));
  led->next_init_step();

  // connect to wifi
  wifi_connect();
  led->next_init_step();

  // create all components
  detection_ctx = std::make_unique<detection_context>();
  led->next_init_step();

  sampler = std::make_shared<i2s_sampler>();
  led->next_init_step();

  detector = std::make_shared<command_detector>();
  led->next_init_step();

  speech_api = std::make_unique<google_speech_api>();
  led->next_init_step();

  chat = std::make_unique<chat_gpt>();
  led->next_init_step();

  synthesizer = std::make_unique<speech_synthesizer>();
  led->next_init_step();

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
    }, "AI task", 8192, nullptr, 1, &detector_task_handle, 1
  );
  led->next_init_step();

  sampler->start();
  led->next_init_step();
  Serial.println(R"(Say "komputer")");

  led->init_done();
}

void loop()
{
  const auto free_ram = esp_get_free_heap_size();
  // Serial.printf("Free HEAP: %u\tSPRAM: %u\tRAM: %d\n", ESP.getFreeHeap(), ESP.getFreePsram(), free_ram);
  synthesizer->say("dupa");
  vTaskDelay(pdMS_TO_TICKS(1000));
}
