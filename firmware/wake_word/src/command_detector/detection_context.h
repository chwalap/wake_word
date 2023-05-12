#ifndef DETECTION_CONTEXT_H
#define DETECTION_CONTEXT_H

#include "config.h"

#include <memory>

struct detection_context
{
  bool m_is_detector_running = true;
  size_t m_detection_idx = 0;
};

using detection_context_ptr = std::unique_ptr<detection_context>;

#endif
