#ifndef GOOGLE_SPEECH_API_H
#define GOOGLE_SPEECH_API_H

#include "token_manager.h"
#include "audio_input/i2s_sampler.h"

#include <WiFiClientSecure.h>

#include <memory>
#include <vector>

using recording_arr = std::array<uint16_t, MAX_RECORDING_LENGTH>;
using recording_ptr = std::unique_ptr<recording_arr>;
using compressed_arr = std::vector<uint8_t>;

struct google_speech_api
{
  google_speech_api();
  void speech_to_text();
  std::string prepare_request();
  std::string prepare_payload();
  std::string prepare_compressed_request();
  compressed_arr prepare_compressed_payload();
  std::vector<uint8_t> createGzipFile(const std::vector<uint8_t>& compressed_data, int compression_level = 9, uint8_t os_id = 3);

  recording_ptr m_recording;
  token_manager m_token_manager;
  size_t m_recording_start_idx;
  size_t m_recording_last_idx;
  size_t m_silence_lenght;
  size_t m_read_idx;
  bool m_is_recording;
  bool m_recording_first_segment;
  int16_t m_max_value_first_segment;
  WiFiClientSecure m_client;
};

using google_speech_api_ptr = std::unique_ptr<google_speech_api>;

#endif
