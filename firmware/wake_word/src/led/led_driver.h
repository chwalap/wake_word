#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include "config.h"

#include <memory>
#include <FastLED.h>

struct led_driver
{
  led_driver();

  void set_color(const CRGB& color);
  void next_init_step();
  void init_colors();
  void init_done();

  CRGBArray<NO_LEDS> m_leds;
  CRGB m_init_colors[NO_INIT_STEPS];
  size_t m_init_step;
};

using led_driver_ptr = std::unique_ptr<led_driver>;

#endif
