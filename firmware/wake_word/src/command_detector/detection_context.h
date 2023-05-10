#ifndef DETECTION_CONTEXT_H
#define DETECTION_CONTEXT_H

#include "config.h"

#include <memory>

struct detection_context
{
  bool m_is_detector_running = true;
  size_t m_detection_idx = 0;

  // todo: implement this and use in Google STT API to detect silence
  float m_detected_mean = 0.f;
  float m_detected_max = 0.f;
};

using detection_context_ptr = std::unique_ptr<detection_context>;

#endif
