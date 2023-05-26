#include "audio_processor.h"

audio_processor::audio_processor()
{
  m_fft_input = std::make_unique<fft_in_arr>();
  m_fft_output = std::make_unique<fft_out_arr>();
  m_energy = std::make_unique<energy_arr>();
  m_hamming_window = std::make_unique<hamming_window>();
  m_kiss_cfg = kiss_fftr_alloc(FFT_SIZE, false, 0, 0);
  m_smoothed_noise_floor = 0.f;
}

bool audio_processor::get_spectrogram(ring_buffer_ptr& reader, float* spec_out)
{
  auto start_idx = reader->get_index();
  float mean = 0.f;
  for (size_t i = 0; i < AUDIO_BUFFER_LENGTH; ++i)
  {
    mean += reader->get_current_sample();
    reader->move_to_next_sample();
  }
  mean /= (float) AUDIO_BUFFER_LENGTH;
  reader->set_index(start_idx);

  float max = 0.f;
  float noise_floor = 0.f;
  size_t samples_over_noise_floor = 0;
  for (size_t i = 0; i < AUDIO_BUFFER_LENGTH; ++i)
  {
    const float v = fabsf(((float) reader->get_current_sample()) - mean);
    max = std::max(max, v);
    noise_floor += v;

    if (v > 5 * m_smoothed_noise_floor)
      ++samples_over_noise_floor;

    reader->move_to_next_sample();
  }
  noise_floor /= (float) AUDIO_BUFFER_LENGTH;

  if (noise_floor < m_smoothed_noise_floor)

    m_smoothed_noise_floor = 0.7f * m_smoothed_noise_floor + noise_floor * 0.3f;
  else
    m_smoothed_noise_floor = 0.99f * m_smoothed_noise_floor + noise_floor * 0.01;

  const auto win_end = start_idx + AUDIO_BUFFER_LENGTH - WINDOW_SIZE;
  for (size_t win_start = start_idx; win_start < win_end; win_start += STEP_SIZE)
  {
    reader->set_index(win_start);

    for (size_t i = 0; i < WINDOW_SIZE; ++i)
    {
      (*m_fft_input)[i] = ((float) reader->get_current_sample() - mean) / max;
      reader->move_to_next_sample();
    }

    for (size_t i = WINDOW_SIZE; i < FFT_SIZE; ++i)
      (*m_fft_input)[i] = 0.f;

    get_spectrogram_segment(spec_out);
    spec_out += ENERGY_POOLED_SIZE;
  }

  return samples_over_noise_floor > AUDIO_BUFFER_LENGTH / 20;
}

void audio_processor::get_spectrogram_segment(float* spec_row)
{
  m_hamming_window->apply(*m_fft_input);

  kiss_fftr(m_kiss_cfg, m_fft_input->data(), m_fft_output->data());

  for (size_t i = 0; i < ENERGY_SIZE; ++i)
  {
    const float real = (*m_fft_output)[i].r;
    const float imag = (*m_fft_output)[i].i;
    const float m = (real * real) + (imag * imag);
    (*m_energy)[i] = m;
  }

  float* out_src = m_energy->data();
  float* out_dst = spec_row;

  for (size_t i = 0; i < ENERGY_SIZE; i += POOLING_SIZE)
  {
    float avg = 0.f;
    for (size_t j = 0; j < POOLING_SIZE; ++j)
    {
      if (i + j < ENERGY_SIZE)
      {
        avg += *out_src;
        ++out_src;
      }
    }
    *out_dst = avg / (float) POOLING_SIZE;
    ++out_dst;
  }

  for (size_t i = 0; i < ENERGY_POOLED_SIZE; ++i)
    spec_row[i] = log10f(spec_row[i] + EPSILON_F);
}
