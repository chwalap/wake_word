#include <Arduino.h>
#include <esp_task_wdt.h>
// #include <Audio.h>

#include "config.h"
#include "utils/wifi.h"
#include "audio_input/i2s_sampler.h"
#include "command_detector/command_detector.h"
#include "command_detector/detection_context.h"
#include "api/google_speech_api.h"
#include "api/chat_gpt.h"
// #include "audio_output/speech_synthesizer.h"
// #include "audio_output/i2s_speaker.h"
#include "led/led_driver.h"

// todo: recompile idf libraries and force mbedtls to use psram

i2s_sampler_ptr sampler;
command_detector_ptr detector;
google_speech_api_ptr speech_api;
detection_context_ptr detection_ctx;
// speech_synthesizer_ptr synthesizer;
chat_gpt_ptr chat;
led_driver_ptr led;
// i2s_speaker_ptr speaker;
// Audio audio(false, 3, I2S_NUM_1);

TaskHandle_t detector_task_handle;

void setup()
{
  led = std::make_unique<led_driver>();

  // just don't panic!
  esp_task_wdt_init(10, true);
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

  // synthesizer = std::make_unique<speech_synthesizer>();
  // led->next_init_step();

  // speaker = std::make_unique<i2s_speaker>();
  // audio.setPinout(AUDIO_OUT_SCK_PIN, AUDIO_OUT_WS_PIN, AUDIO_OUT_DATA_PIN);
  // audio.setVolume(100);
  // audio.connecttohost("http://vis.media-ice.musicradio.com/CapitalMP3");

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
  // speaker->start();
  led->next_init_step();
  Serial.println(R"(Say "komputer")");

  led->init_done();
}

void loop()
{
  // int16_t i2s_write_buff[256];
  // for(int i = 0; i < 256; i += 2) {
  //   float sample_val = sinf(2.0 * M_PI * i * WAVE_FREQ_HZ / SAMPLE_RATE);
  //   int16_t sample_val_i16 = static_cast<int16_t>(sample_val * 32767);

  //   // Write the same value to left and right channels
  //   i2s_write_buff[i] = sample_val_i16;     // Left channel
  //   i2s_write_buff[i + 1] = sample_val_i16; // Right channel
  // }
  // size_t bytes_written;
  // i2s_write(I2S_NUM_1, i2s_write_buff, sizeof(i2s_write_buff), &bytes_written, portMAX_DELAY);

  // const auto free_ram = esp_get_free_heap_size();
  // Serial.printf("Free HEAP: %u\tSPRAM: %u\tRAM: %d\n", ESP.getFreeHeap(), ESP.getFreePsram(), free_ram);
  // synthesizer->say("dupa");
  // i2s_write
  // audio.loop();
  vTaskDelay(pdMS_TO_TICKS(1000));
}
