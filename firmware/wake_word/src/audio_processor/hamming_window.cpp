#include "hamming_window.h"
#include <math.h>

hamming_window::hamming_window()
{
  m_coefficients = std::make_unique<coefficients_arr>();
  const float arg = M_PI * 2.f / (float) WINDOW_SIZE;

  for (size_t i = 0; i < WINDOW_SIZE; ++i)
    (*m_coefficients)[i] = 0.5f - (0.5f * cos(arg * ((float) i + 0.5f)));
}

void hamming_window::apply(fft_in_arr& input)
{
  for (size_t i = 0; i < WINDOW_SIZE; ++i)
    input[i] = input[i] * (*m_coefficients)[i];
}
