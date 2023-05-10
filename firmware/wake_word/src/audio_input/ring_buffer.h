#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <array>
#include <memory>

#include "config.h"

using audio_buffer = std::array<uint16_t, AUDIO_BUFFER_LENGTH>;
using audio_buffer_ptr = std::unique_ptr<audio_buffer>;
using audio_buffer_arr = std::array<audio_buffer_ptr, NO_I2S_BUFFERS>;
using audio_buffers_ptr = std::shared_ptr<audio_buffer_arr>;

// todo: add documentation
// todo: add tests

struct ring_buffer
{
  ring_buffer(audio_buffers_ptr audio_buffers)
  : m_audio_buffers{audio_buffers},
    m_current_buffer{(*m_audio_buffers)[0].get()},
    m_buffer_pos{0},
    m_buffer_idx{0}
  {
  }

  size_t get_index()
  {
    return m_buffer_idx * AUDIO_BUFFER_LENGTH + m_buffer_pos;
  }

  void set_index(size_t idx)
  {
    idx = (idx + RING_BUFFER_TOTAL_SIZE) % RING_BUFFER_TOTAL_SIZE;
    m_buffer_idx = (idx / AUDIO_BUFFER_LENGTH) % NO_I2S_BUFFERS;
    m_buffer_pos = idx % AUDIO_BUFFER_LENGTH;
    m_current_buffer = (*m_audio_buffers)[m_buffer_idx].get();
  }

  int16_t get_current_sample()
  {
    return (*m_current_buffer)[m_buffer_pos];
  }

  void set_current_sample(uint16_t sample)
  {
    (*m_current_buffer)[m_buffer_pos] = sample;
  }

  void rewind(size_t no_samples)
  {
    set_index(get_index() - no_samples);
  }

  bool move_to_next_sample()
  {
    m_buffer_pos++;
    if (m_buffer_pos == AUDIO_BUFFER_LENGTH)
    {
      m_buffer_pos = 0;
      m_buffer_idx++;

      if (m_buffer_idx == NO_I2S_BUFFERS)
        m_buffer_idx = 0;

      m_current_buffer = (*m_audio_buffers)[m_buffer_idx].get();
      return true;
    }
    return false;
  }

  audio_buffers_ptr m_audio_buffers;
  audio_buffer* m_current_buffer;
  size_t m_buffer_pos;
  size_t m_buffer_idx;
};

using ring_buffer_ptr = std::unique_ptr<ring_buffer>;

#endif
