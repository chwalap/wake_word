#ifndef I2S_SAMPLER_H
#define I2S_SAMPLER_H

#include <Arduino.h>

#include "ring_buffer.h"
#include "config.h"

#include <memory>

// todo: add documentation
// todo: add tests

struct i2s_sampler
{
  i2s_sampler();
  void start();
  void add_sample(uint16_t sample);
  void process_data(uint8_t *data, size_t bytes_read);
  ring_buffer_ptr get_ring_buffer_reader();

  audio_buffers_ptr m_audio_buffers;
  ring_buffer_ptr m_ring_buffer;
  size_t m_current_buffer;

  TaskHandle_t m_reader_handle;
  QueueHandle_t m_queue;
};

using i2s_sampler_ptr = std::shared_ptr<i2s_sampler>;

#endif
