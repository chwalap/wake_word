#include "speech_synthesizer.h"
#include <Arduino.h>

#include "config.h"
#include "common.h"
#include "led/led_driver.h"

TaskHandle_t m_milena_task_handle;
extern led_driver_ptr led;
extern speech_synthesizer_ptr synthesizer;

speech_synthesizer::speech_synthesizer()
{
  m_speech_buffer.reserve(2048);

  m_milena = std::make_unique<AudioGeneratorMilena>();
  m_milena->setInternalDAC(false);
  m_milena->setShowPhonemes(false);
  m_milena->setPitch(SPEECH_SYNTHESIS_PITCH);
  m_milena->setSpeed(SPEECH_SYNTHESIS_SPEED);
  m_milena->setVolume(SPEECH_SYNTHESIS_VOLUME);
  m_milena->setContrast(SPEECH_SYNTHESIS_CONTRAST);

  m_audio_output = std::make_unique<AudioOutputI2S>(1);
  m_audio_output->SetBitsPerSample(16);
  m_audio_output->SetChannels(1);
  m_audio_output->SetRate(SAMPLE_RATE);
  m_audio_output->SetOutputModeMono(true);
  m_audio_output->SetPinout(AUDIO_OUT_SCK_PIN, AUDIO_OUT_WS_PIN, AUDIO_OUT_DATA_PIN);
  m_audio_output->SetGain(SPEECH_SYNTHESIS_GAIN);

  led->next_init_step();
}

void speech_synthesizer::start()
{
  xTaskCreatePinnedToCore(
    [](void*)
    {
      synthesizer->loop();
    }, "Audio out task", 8192, nullptr, 0, &m_milena_task_handle, 0
  );
}

void speech_synthesizer::loop()
{
  while (true)
  {
    uint32_t ulNotValue = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));
    if (ulNotValue > 0)
    {
      // Serial.println("Got signal to start speaking!");
      m_speaking_idx = 0;

      while (m_is_feeding || m_speaking_idx < m_speech_buffer.length())
      {
        // get next word
        const auto end_of_sentence = find_sentence_end(m_speech_buffer, m_speaking_idx);
        // const auto end_of_sentence = speech->m_speech_buffer.find(".", speech->m_speaking_idx);
        if (end_of_sentence == std::string::npos)
        {
          // Serial.println("Waiting for buffer to be filled.");
          vTaskDelay(pdMS_TO_TICKS(50));
          continue;
        }

        const auto sentence = m_speech_buffer.substr(m_speaking_idx, end_of_sentence - m_speaking_idx);
        // Serial.printf("Next sentence to say: %s\n", sentence.c_str());

        // todo: do something with this error: I2S: register I2S object to platform failed
        if (!m_milena->begin(sentence.c_str(), m_audio_output.get()))
        {
          Serial.println("Unable to start speech synthesis!");
          break;
        }

        while (m_milena->isRunning())
        {
          if (!m_milena->loop())
          {
            m_milena->stop();
            break;
          }

          taskYIELD();
        }
        // Serial.println("The sentence has been spoken!");

        m_speaking_idx = end_of_sentence + 1; // +1 to consume dot
        taskYIELD();
      }

      // Serial.println("Synthesizer finished talking!");

      m_is_talking = false;
      m_speech_buffer.clear();
    }
  }
}

void speech_synthesizer::say(const std::string& text)
{
  m_speech_buffer += text;
  m_is_feeding = true;

  if (!m_is_talking)
  {
    if (find_sentence_end(text) != std::string::npos)
    {
      xTaskNotify(m_milena_task_handle, 1, eSetBits);
      m_is_talking = true;
    }
  }
}

size_t speech_synthesizer::find_sentence_end(const std::string& str, size_t pos)
{
  return str.find_first_of(".!?:;\n", pos);
}
