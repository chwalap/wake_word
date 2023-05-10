#ifndef HAMMING_WINDOW_H
#define HAMMING_WINDOW_H

#include "config.h"

#include <array>
#include <memory>

using fft_in_arr = std::array<float, FFT_SIZE>;
using coefficients_arr = std::array<float, WINDOW_SIZE>;
using coefficients_ptr = std::unique_ptr<coefficients_arr>;

struct hamming_window
{
  hamming_window();
  void apply(fft_in_arr& input);

  coefficients_ptr m_coefficients;
};

using hamming_window_ptr = std::unique_ptr<hamming_window>;

#endif
