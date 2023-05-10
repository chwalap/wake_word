#ifndef COMMAND_DETECTOR_H
#define COMMAND_DETECTOR_H

#include "audio_input/i2s_sampler.h"
#include "audio_processor/audio_processor.h"
#include "neural_network/neural_network.h"

#include <memory>

// todo: add documentation
// todo: add tests

struct command_detector
{
  command_detector();
  void detect_wake_word();
  size_t get_best_score_idx() const;

  unsigned long m_last_detection;
  neural_network_ptr m_neural_network;
  audio_processor_ptr m_audio_proc;
};

using command_detector_ptr = std::shared_ptr<command_detector>;

#endif
