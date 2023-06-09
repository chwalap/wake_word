#include "command_detector.h"
#include "detection_context.h"
#include "config.h"
#include "led/led_driver.h"
#include "api/google_speech_api.h"

#include <esp32-hal.h>
#include <HardwareSerial.h>

TaskHandle_t detector_task_handle;

extern i2s_sampler_ptr sampler;
extern detection_context_ptr detection_ctx;
extern led_driver_ptr led;
extern google_speech_api_ptr speech_api;
extern command_detector_ptr detector;

command_detector::command_detector()
{
  m_neural_network = std::make_unique<neural_network>();
  m_audio_proc = std::make_unique<audio_processor>();
  m_last_detection = 0;

  led->next_init_step();
}

void command_detector::start()
{
  xTaskCreatePinnedToCore(
    [](void*)
    {
      detector->loop();
    }, "AI task", 8192, nullptr, 1, &detector_task_handle, 1
  );
}

void command_detector::loop()
{
  while (true)
  {
    uint32_t ulNotValue = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));
    if (ulNotValue > 0)
    {
      if (detection_ctx->m_is_detector_running)
      {
        detect_wake_word();
      }
      else
      {
        speech_api->speech_to_text();
      }
    }

    taskYIELD();
  }
}

void command_detector::detect_wake_word()
{
  auto start = millis();

  {
    ring_buffer_ptr reader = sampler->get_ring_buffer_reader();
    reader->rewind(AUDIO_BUFFER_LENGTH);
    detection_ctx->m_detection_idx = reader->get_index();
    auto* input_buffer = m_neural_network->get_input_buffer();
    m_audio_proc->get_spectrogram(reader, input_buffer);
  }

  m_neural_network->predict();
  auto best_idx = get_best_score_idx();

  Serial.printf("\nDetection took %ums\n",  millis() - start);

  if (start - m_last_detection > MIN_TIME_BETWEEN_DETECTIONS)
  {
    if (best_idx == TARGET_WORD_IDX)
    {
      m_last_detection = start;
      // Serial.println("Detected komputer!");
      detection_ctx->m_is_detector_running = false;
      led->set_color(CRGB::Orange);
    }
  }
}

size_t command_detector::get_best_score_idx() const
{
  float best_score = -1.f;
  size_t best_index = 0;

  for (size_t i = 0; i < NO_COMMANDS; i++)
  {
    float pred = std::max(m_neural_network->get_output_buffer()[i], EPSILON_F);
    // Serial.printf("%.2f\t", pred);
    if (pred > best_score)
    {
      best_score = pred;
      best_index = i;
    }
  }

  return best_index;
}
