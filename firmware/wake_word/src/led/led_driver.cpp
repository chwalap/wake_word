#include "led_driver.h"

led_driver::led_driver()
{
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(m_leds, NO_LEDS).setCorrection(TypicalLEDStrip);
  m_init_step = 0;
  init_colors();
}

void led_driver::set_color(const CRGB& color)
{
  m_leds[0] = color;
  FastLED.show();
}

void led_driver::next_init_step()
{
  set_color(m_init_colors[m_init_step]);
  m_init_step++;
}

void led_driver::init_colors()
{
  for(size_t i = 0; i < NO_INIT_STEPS; ++i)
  {
    int hue = map(i, 0, NO_INIT_STEPS - 1, 0, 96);
    m_init_colors[i] = CHSV(hue, 255, 255);
  }
}

void led_driver::init_done()
{
  for (size_t i = 0; i < 3; ++i)
  {
    set_color(CRGB::Green);
    delay(100);
    set_color(CRGB::Black);
    delay(100);
  }

  set_color(CRGB::Red);
}