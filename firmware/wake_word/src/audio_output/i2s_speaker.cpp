#include "i2s_speaker.h"

i2s_speaker::i2s_speaker()
{

}

void i2s_speaker::start()
{
  // i2s_config_t i2s_config = {
  //   .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
  //   .sample_rate = 44100,
  //   .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
  //   .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
  //   .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S_MSB),
  //   .dma_buf_count = 8,
  //   .dma_buf_len = 64,
  //   .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1
  // };
  // i2s_pin_config_t pin_config = {
  //   .bck_io_num = BCK_PIN,
  //   .ws_io_num = WS_PIN,
  //   .data_out_num = DATA_PIN,
  //   .data_in_num = I2S_PIN_NO_CHANGE
  // };
  // i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  // i2s_set_pin(I2S_PORT, &pin_config);
  // pinMode(AMP_PIN, OUTPUT);

}