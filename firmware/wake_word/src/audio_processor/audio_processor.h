#ifndef AUDIO_PROCESSOR_H
#define AUDIO_PROCESSOR_H

#include "tools/kiss_fftr.h"

#include "config.h"
#include "audio_input/ring_buffer.h"
#include "hamming_window.h"

#include <memory>

// todo: move to common place?
// using fft_in_arr = std::array<float, FFT_SIZE>; // already defined in hamming_window
using fft_in_ptr = std::unique_ptr<fft_in_arr>;

using fft_out_arr = std::array<kiss_fft_cpx, ENERGY_SIZE>;
using fft_out_ptr = std::unique_ptr<fft_out_arr>;

using energy_arr = std::array<float, ENERGY_SIZE>;
using energy_ptr = std::unique_ptr<energy_arr>;

struct audio_processor
{
  audio_processor();
  bool get_spectrogram(ring_buffer_ptr& reader, float* spec_out);
  void get_spectrogram_segment(float* spec_row);

  energy_ptr m_energy;
  fft_in_ptr m_fft_input;
  fft_out_ptr m_fft_output;
  kiss_fftr_cfg m_kiss_cfg;
  float m_smoothed_noise_floor;
  hamming_window_ptr m_hamming_window;
};

using audio_processor_ptr = std::unique_ptr<audio_processor>;

#endif
