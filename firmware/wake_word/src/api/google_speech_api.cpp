#include "google_speech_api.h"

#include "command_detector/detection_context.h"
#include "utils/base64.h"
#include "utils/response_parser.h"

#include <miniz.h>
#include <ArduinoJson.h>
#include <sstream>

extern i2s_sampler_ptr sampler;
extern detection_context_ptr detection_ctx;

const char GOOGLE_SPEECH_API[] PROGMEM = "speech.googleapis.com";

google_speech_api::google_speech_api()
{
  m_recording = std::make_unique<recording_arr>();
  m_client.setInsecure();
  m_is_recording = false;
}

void google_speech_api::speech_to_text()
{
  bool recording_completed = false;

  // todo: experiment with taking task notification here and recording everything in here
  // uint32_t ulNotValue = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(100));
  // if (ulNotValue > 0)

  {
    auto reader = sampler->get_ring_buffer_reader();
    auto start_idx = reader->get_index();

    if (!m_is_recording)
    {
      Serial.println("Start recording...");
      m_is_recording = true;
      m_recording_start_idx = detection_ctx->m_detection_idx;
      m_recording_last_idx = m_recording_start_idx;
      m_read_idx = 0;
      m_silence_lenght = 0;
    }

    reader->set_index(m_recording_last_idx);
    while (reader->get_index() != start_idx)
    {
      int16_t sample = reader->get_current_sample();
      // Serial.println(sample);
      (*m_recording)[m_read_idx] = sample;
      reader->move_to_next_sample();
      ++m_read_idx;

      // stop recording when reached end of buffer
      if (m_read_idx == MAX_RECORDING_LENGTH)
      {
        Serial.println("End of recording due to end of buffer");
        recording_completed = true;
        break;
      }

      // stop recording when 1s of silence
      // if (abs(sample) < SILENCE_THRESHOLD)
      // {
      //   ++m_silence_lenght;
      //   if (m_silence_lenght % 1600 == 0)
      //     Serial.println("100ms silence");

      //   if (m_silence_lenght >= SILENCE_CUT_RECORDING_LENGTH)
      //   {
      //     Serial.println("End of recording due to 1s silence");
      //     recording_completed = true;
      //     break;
      //   }
      // }
      // else
      // {
      //   Serial.printf("Silence for %zu samples\n", m_silence_lenght);
      //   m_silence_lenght = 0;
      // }
    }

    m_recording_last_idx = reader->get_index();
  }

  if (!recording_completed)
    return;

  m_client.stop();
  if (m_client.connect(GOOGLE_SPEECH_API, 443, 5000) == 0)
  {
    detection_ctx->m_is_detector_running = true;
    return;
  }
  m_client.setTimeout(5);

  const auto request = prepare_request();
  // const auto request = prepare_compressed_request();
  // const auto payload = prepare_compressed_payload();
  // todo: optimize this; only one call should be present here
  // const auto gzip = createGzipFile(payload);

  // Serial.printf("%s\n", prepare_payload().c_str());
  // for (const auto &b : payload)
  //   Serial.printf("%#02x ", b);
  // Serial.printf("\nRequest:\n%s", request.c_str());
  // Serial.printf("Payload:\n%s\n", (const char*)payload.data());

  // m_client.println(request.c_str());
  // Serial.printf("Timeout: %u\n", m_client.getTimeout());

  size_t all_sent_bytes = 0;
  while (all_sent_bytes < request.size())
  {
    const auto sent_bytes = m_client.write((const uint8_t*)(request.data() + all_sent_bytes), request.size() - all_sent_bytes);
    all_sent_bytes += sent_bytes;
    // Serial.printf("Sent %zu bytes\n", sent_bytes);
  }
  // Serial.printf("Sent bytes: %zu\nRequest: %zu\n", all_sent_bytes, request.length());

  String resp;
  auto reading_start = millis();
  while (resp.isEmpty() && millis() - reading_start < 10000)
  {
    resp += m_client.readString();
  }
  // m_client.readString();
  const std::string response = std::string{resp.c_str()};
  // Serial.printf("Response: %s\n", response.c_str());
  const auto json_response = extract_json(response);
  // Serial.printf("JSON: %s\n", json_response.c_str());

  auto json = std::make_unique<StaticJsonDocument<4096>>();
  deserializeJson(*json, json_response);
  String transcript = (*json)["results"][0]["alternatives"][0]["transcript"];
  Serial.printf(
    "========================================================================================================================\n"
    "Transcript: %s\n"
    "========================================================================================================================\n", transcript.c_str());

  m_is_recording = false;
  m_client.stop();
  detection_ctx->m_is_detector_running = true;
  return;
}

std::string google_speech_api::prepare_request()
{
  const auto token = m_token_manager.get_token();
  const auto payload = prepare_payload();
  const auto request =
    "POST /v1/speech:recognize HTTP/1.1\r\n"
    "Authorization: Bearer " + token + "\r\n"
    "Host: speech.googleapis.com\r\n"
    "Accept: application/json\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: " + std::to_string(payload.length()) + "\r\n"
    "\r\n" + payload + "\r\n";

  return request;
}

std::string google_speech_api::prepare_payload()
{
  const auto base64 = base64_encode((uint8_t*) m_recording->data(), m_read_idx * 2);
  const auto payload =
  "{"
    "\"config\":{"
      "\"languageCode\":\"pl-PL\","
      "\"encoding\":\"LINEAR16\","
      "\"sampleRateHertz\":16000,"
      "\"enableWordTimeOffsets\":false"
    "},"
    "\"audio\":{"
        "\"content\":\"" + base64 + "\""
    "}"
  "}";

  return payload;
}

compressed_arr google_speech_api::prepare_compressed_payload()
{
  const auto payload = prepare_payload();
  const mz_ulong original_length = payload.length();
  mz_ulong compressed_length = mz_compressBound(original_length);
  compressed_arr compressed_payload;
  compressed_payload.resize(compressed_length);

  if (mz_compress2(compressed_payload.data(), &compressed_length, (const uint8_t*)payload.data(), original_length, MZ_BEST_COMPRESSION) != MZ_OK)
    Serial.println("Compression failed!");

  // compressed_arr result = {0x1F, 0x8B, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  // result.insert(result.end(), compressed_payload.begin(), compressed_payload.begin() + compressed_length);
  // return result;
  return compressed_arr(compressed_payload.begin(), compressed_payload.begin() + compressed_length);
}

std::string google_speech_api::prepare_compressed_request()
{
  const auto token = m_token_manager.get_token();
  const auto payload = prepare_compressed_payload();
  const auto gzip = createGzipFile(payload);

  const auto request =
    "POST /v1/speech:recognize HTTP/1.1\r\n"
    "Authorization: Bearer " + token + "\r\n"
    "Host: speech.googleapis.com\r\n"
    "Accept: application/json\r\n"
    "Content-Type: application/json\r\n"
    "Content-Encoding: gzip\r\n"
    "Content-Length: " + std::to_string(gzip.size()) + "\r\n";

  return request;
}

// code generated by ChatGPT
std::vector<uint8_t> google_speech_api::createGzipFile(const std::vector<uint8_t>& compressed_data, int compression_level, uint8_t os_id)
{
  std::vector<uint8_t> gzip_file;
  gzip_file.reserve(10 + compressed_data.size() + 8);

  auto tm = m_token_manager.m_ntp_client.getEpochTime();

  // Gzip header
  gzip_file.push_back(0x1f); // magic number
  gzip_file.push_back(0x8b); // magic number
  gzip_file.push_back(0x08); // DEFLATE
  gzip_file.push_back(0x01); // header flags
  gzip_file.push_back(static_cast<uint8_t>(tm & 0xff)); tm >>= 8;
  gzip_file.push_back(static_cast<uint8_t>(tm & 0xff)); tm >>= 8;
  gzip_file.push_back(static_cast<uint8_t>(tm & 0xff)); tm >>= 8;
  gzip_file.push_back(static_cast<uint8_t>(tm & 0xff));
  gzip_file.push_back(0x02); // compression flags
  gzip_file.push_back(0xff); // OS unknown

  // Compressed data
  gzip_file.insert(gzip_file.end(), compressed_data.begin(), compressed_data.end());

  // Gzip trailer (CRC32 and uncompressed data length)
  uint32_t crc = crc32(0L, Z_NULL, 0);
  crc = crc32(crc, &compressed_data[0], compressed_data.size());
  gzip_file.push_back(static_cast<uint8_t>(crc & 0xff)); crc >>= 8;
  gzip_file.push_back(static_cast<uint8_t>(crc & 0xff)); crc >>= 8;
  gzip_file.push_back(static_cast<uint8_t>(crc & 0xff)); crc >>= 8;
  gzip_file.push_back(static_cast<uint8_t>(crc & 0xff));

  uint32_t length = static_cast<uint32_t>(compressed_data.size());
  gzip_file.push_back(static_cast<uint8_t>(length & 0xff)); length >>= 8;
  gzip_file.push_back(static_cast<uint8_t>(length & 0xff)); length >>= 8;
  gzip_file.push_back(static_cast<uint8_t>(length & 0xff)); length >>= 8;
  gzip_file.push_back(static_cast<uint8_t>(length & 0xff));

  return gzip_file;
}
