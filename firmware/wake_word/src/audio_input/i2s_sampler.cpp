#include "i2s_sampler.h"
#include <driver/i2s.h>

extern TaskHandle_t detector_task_handle;

i2s_sampler::i2s_sampler()
{
  m_audio_buffers = std::make_shared<audio_buffer_arr>();
  for (auto &buffer : *m_audio_buffers)
    buffer = std::make_unique<audio_buffer>();

  m_ring_buffer = std::make_unique<ring_buffer>(m_audio_buffers);
}

void i2s_sampler::start()
{
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_pin_config_t i2s_pins = {
    .bck_io_num = AUDIO_IN_SCK_PIN,
    .ws_io_num = AUDIO_IN_WS_PIN,
    .data_out_num = -1,
    .data_in_num = AUDIO_IN_DATA_PIN
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 4, &m_queue);
  i2s_set_pin(I2S_NUM_0, &i2s_pins);

  xTaskCreatePinnedToCore(
    [](void *p)
    {
      i2s_sampler* sampler = (i2s_sampler*)p;

      while (true)
      {
        i2s_event_t e;
        if (xQueueReceive(sampler->m_queue, &e, portMAX_DELAY) == pdPASS)
        {
          if (e.type == I2S_EVENT_RX_DONE)
          {
            size_t bytes_read = 0;
            do
            {
              uint8_t data[1024];
              i2s_read(I2S_NUM_0, data, 1024, &bytes_read, 10);
              sampler->process_data(data, bytes_read);
            } while (bytes_read > 0);
          }
        }

        taskYIELD();
      }
    }, "Audio in task", 8192, this, 1, &m_reader_handle, 0
  );
}

ring_buffer_ptr i2s_sampler::get_ring_buffer_reader()
{
  ring_buffer_ptr copy = std::make_unique<ring_buffer>(m_audio_buffers);
  copy->set_index(m_ring_buffer->get_index());
  return copy;
}

void i2s_sampler::add_sample(int16_t sample)
{
  m_ring_buffer->set_current_sample(sample);

  if (m_ring_buffer->move_to_next_sample())
    xTaskNotify(detector_task_handle, 1, eSetBits);
}

void i2s_sampler::process_data(uint8_t* data, size_t bytes_read)
{
  const int16_t* samples = (int16_t*)data;
  const size_t size = bytes_read / 2;

  for (auto i = 0; i < size; ++i)
    add_sample(samples[i]);
}
