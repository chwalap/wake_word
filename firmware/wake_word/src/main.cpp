#include <Arduino.h>

#include "config.h"
#include "utils/wifi.h"
#include "utils/watchdog.h"
#include "utils/serial.h"
#include "audio_input/i2s_sampler.h"
#include "command_detector/command_detector.h"
#include "command_detector/detection_context.h"
#include "api/google_speech_api.h"
#include "api/chat_gpt.h"
#include "audio_output/speech_synthesizer.h"
#include "led/led_driver.h"

// todo: recompile idf libraries and force mbedtls to use psram
// todo: better error handling
// todo: proper HTTP response parsing
// todo: better speech synthesis

i2s_sampler_ptr sampler;
command_detector_ptr detector;
google_speech_api_ptr speech_api;
detection_context_ptr detection_ctx;
speech_synthesizer_ptr synthesizer;
chat_gpt_ptr chat;
led_driver_ptr led;

void init_components()
{
  detection_ctx = std::make_unique<detection_context>();
  sampler = std::make_shared<i2s_sampler>();
  detector = std::make_shared<command_detector>();
  speech_api = std::make_unique<google_speech_api>();
  chat = std::make_unique<chat_gpt>();
  synthesizer = std::make_unique<speech_synthesizer>();
}

void setup()
{
  led = std::make_unique<led_driver>();

  init_serial();
  init_watchdog();
  wifi_connect();
  init_components();

  synthesizer->start();
  synthesizer->say("Powiedz komputer.");
  detector->start();
  sampler->start();

  led->init_done();
}

void loop()
{
  vTaskDelay(pdMS_TO_TICKS(1000));
}
