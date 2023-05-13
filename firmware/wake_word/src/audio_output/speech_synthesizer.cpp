// #include "speech_synthesizer.h"
// #include <Arduino.h>

// #include "config.h"

// TaskHandle_t m_milena_task_handle;

// speech_synthesizer::speech_synthesizer()
// {
//   // m_milena = std::make_unique<AudioGeneratorMilena>();
//   // m_milena->setInternalDAC(false);
//   // m_milena->setShowPhonemes(false);
//   // m_milena->setPitch(SPEECH_SYNTHESIS_PITCH);
//   // m_milena->setSpeed(SPEECH_SYNTHESIS_SPEED);
//   // m_milena->setVolume(SPEECH_SYNTHESIS_VOLUME);
//   // m_milena->setContrast(SPEECH_SYNTHESIS_CONTRAST);

//   // m_audio_output = std::make_unique<AudioOutputI2S>(1, 0);
//   // m_audio_output->SetBitsPerSample(16);
//   // m_audio_output->SetChannels(1);
//   // m_audio_output->SetRate(SAMPLE_RATE);
//   // m_audio_output->SetOutputModeMono(true);
//   // m_audio_output->SetPinout(AUDIO_OUT_SCK_PIN, AUDIO_OUT_WS_PIN, AUDIO_OUT_DATA_PIN);
//   // m_audio_output->SetGain(SPEECH_SYNTHESIS_GAIN);

//   // xTaskCreatePinnedToCore(
//   //   [](void* param) mutable
//   //   {
//   //     auto* speech = (speech_synthesizer*) param;

//   //     while (true)
//   //     {
//   //       while (speech->m_milena->isRunning())
//   //       {
//   //         if (!speech->m_milena->loop())
//   //           speech->m_milena->stop();
//   //       }

//   //       vTaskDelay(pdMS_TO_TICKS(30));
//   //       // taskYIELD();
//   //     }
//   //   }, "Audio out task", 8192, this, 1, &m_milena_task_handle, 0 // todo: consider priority 0
//   // );
// }

// void speech_synthesizer::say(const char* text)
// {
//   // if (!m_milena->begin(text, m_audio_output.get()))
//   // {
//   //   Serial.println("Error during speech synthesis!");
//   // }
// }
