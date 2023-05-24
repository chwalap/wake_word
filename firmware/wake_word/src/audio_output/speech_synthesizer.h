#ifndef SPEECH_SYNTHESIER_H
#define SPEECH_SYNTHESIER_H

#include <AudioGeneratorMilena.h>
#include <AudioOutputI2S.h>

#include <memory>

using milena_ptr = std::unique_ptr<AudioGeneratorMilena>;
using audio_output_ptr = std::unique_ptr<AudioOutputI2S>;

struct speech_synthesizer
{
  speech_synthesizer();
  void say(const std::string& text);
  size_t find_sentence_end(const std::string& str, size_t pos = 0);

  milena_ptr m_milena;
  audio_output_ptr m_audio_output;

  std::string m_speech_buffer;
  size_t m_speaking_idx;
  bool m_is_talking = false;
  bool m_is_feeding = false;
};

using speech_synthesizer_ptr = std::unique_ptr<speech_synthesizer>;

#endif
