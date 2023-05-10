#include "token_manager.h"
#include "utils/base64.h"

#include <ArduinoJson.h>

const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQChuvAXJE1cHuJQ\nWwu9e9YgYM/jaePu+L/BOqOMols/sPCsyQUIsU033dZNDZ4XzbKqwsE4wN1LUfYF\nELl1jwcwyqlnm+6jJb3FLdrD8rmsD4Ml8qcZnPHndhY3ba7mlWRN0CsNeYhMsOtJ\nBcUJ+byQnIVPuYlE+YviqEYHzzaHdN7qRxJjZfEDaH4/QhHEcyiV40l4f0TPI5Xu\nK7kjdqtul3B3Q4WUP5JE+DvhS35u06bkx/9etKd4L3mOKQVpJU6Z6UlLtkhr4x+R\nN4G3yJ8HhnC3w1Mvb8U4TfoOUz434b7T+p951EEno16X130qdqXaulGuyRLTYBCh\nRBRuv0y7AgMBAAECggEABDgZ8r8BMtghak9w8N2hE+r119M+ELcCwm05Sz47219n\n0yVS95hi6p3joDks0lnzTi4ffhxviXswmMHVCY9hWvEzVl8eH7mooWy7tzHtWXyo\n6YQF8cHQGiXQy/Uu3P5+HxbBQ/kGlVpmNCgtrQylUfzeidpnjabLv4fi4wN39jRx\n8eIbvWj/2oLZJ0wl0nX1Qt/JZLUSogt8qvvUw/9+vrex+cGQSv6diQy2Pry+n2d+\nY632UvhNv7weRpGx9X2/004npSXUxJlcww49hsCjMlBgPaiRtlRrrzcHCf7auWup\nnHOPOLhpX9/Fai/rDsxXta5bUJPNtH/+KQdBsh5RdQKBgQDhqQl/GukpyJSV+iAh\n6lGApUYdjBoIl/4hQ8cbjERHLF7Xbv3SnT9cpz9U4MF3NxeoWDejT6aqWZ8j+8Dv\nFqldGaNJg7v7a9Ls5N5uBzXmHN8a5O0csvL8wpSuUZodLD6qgzM83Xw2SQgO3vR8\nFHbGuEZDzqw7vGd6T0c2UjoOpwKBgQC3eYCnE0BKi3liLTCfdWxmXdpEkg0UsrBR\nJzJEhkhlZQ7iCOzayQvW2tJT9MG0itWOCwV7G9pMO6Wjy3WmHRpQe+tMPCE+hDa+\nyp+xsKNwtQ2UjpJT8GgOyNbcNkNcRNoxD/alL3eqwe0CBD4ro0nPHAjnv4NKbeB8\nacNSP60HzQKBgQDCMUN43GG2kWQ8xGkOhqTovlHeb4ifyJzrZ3Za4JSJY0eiwWMw\n8h1q0IqRsnEK9KApQPFwWsUkZ8epoabgmXHaKov4YaNNs/Zp4nL/4LijdEkSzdq3\nDdBJb/GfKZ4kaMTs28cmI7wWwzZqBbgDZ2DnlgDxrVNOfM3X1Y2E07IelQKBgEFJ\nItZQq1fbOV4N5ndXOTNTwg8USUNMMiEuaVyoy6oIw7r6og1X7LaPNrhZJ3Qu0r6w\nfUrJJmxp7Q7TnxF02aA5+AgwfUPZmAFb6Gc6C25XfnWWJctoiB56qLnDNUTn2xwK\nKiGtOk+wob1N9TFh8SmZODH+m6EoZ/DdUXs63T8hAoGBAKNDu+SEjYhHjBXtHk1V\nD9veWhiwYLuJP0z/VHFS52p5bOSTbfyikTCBVyc++/l2aQF/emrnt4blShAmFx9+\naL6xHlv1lSj462qTXerIbHvuJltS1VZ9TsfPOue0vEvbawdY0PrF8wehbACFs4/H\nHTk5pf5UTCxcYbMm3RH6VjaP\n-----END PRIVATE KEY-----\n";
const char GOOGLE_TOKEN_HOST[] PROGMEM = "www.googleapis.com";

token_manager::token_manager()
{
  m_ntp_client.begin();
  m_client.setInsecure();

  // todo: create task for this
  fetch_new_token();
}

const std::string& token_manager::get_token()
{
  if (token_expired())
    fetch_new_token();

  return m_token;
}

void token_manager::fetch_new_token()
{
  m_token.clear();

  const auto request = prepare_token_request();

  m_client.stop();
  if (auto ret = m_client.connect(GOOGLE_TOKEN_HOST, 443); ret == 0)
  {
    Serial.printf("Unable to connect: %d\n", ret);
    return;
  }

  const auto sent_bytes = m_client.println(request.c_str());
  const auto json_response = extract_json(std::string{m_client.readString().c_str()});

  StaticJsonDocument<2048> doc;
  deserializeJson(doc, json_response);
  const String token = doc["access_token"];
  m_token = std::string{token.c_str()};
  m_token_creation_time = m_ntp_client.getEpochTime();
  m_client.stop();
}

bool token_manager::token_expired()
{
  return m_ntp_client.getEpochTime() - TOKEN_LIFETIME >= m_token_creation_time;
}

std::string token_manager::prepare_token_request()
{
  const std::string payload = "{\"grant_type\":\"urn:ietf:params:oauth:grant-type:jwt-bearer\",\"assertion\":\"" + get_jwt() + "\"}";
  const std::string request =
    "POST /token HTTP/1.1\r\n"
    "Host: oauth2.googleapis.com\r\n"
    "User-Agent: ESP\r\n"
    "Content-Length: " + std::to_string(payload.length()) + "\r\n"
    "Content-Type: application/json\r\n\r\n";

  return request + payload;
}

std::string token_manager::get_jwt()
{
  const std::string header_base64 PROGMEM = "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9";

  m_ntp_client.update();
  const auto cur_tm = m_ntp_client.getEpochTime();
  const auto exp_tm = cur_tm + TOKEN_LIFETIME;
  const auto cur_tm_str = std::to_string(cur_tm);
  const auto exp_tm_str = std::to_string(exp_tm);

  const std::string payload{
    "{"
      "\"iss\":\"speech@komputer-385115.iam.gserviceaccount.com\","
      "\"sub\":\"speech@komputer-385115.iam.gserviceaccount.com\","
      "\"aud\":\"https://oauth2.googleapis.com/token\","
      "\"iat\":" + cur_tm_str + ","
      "\"exp\":" + exp_tm_str + ","
      "\"scope\":\"https://www.googleapis.com/auth/cloud-platform\""
    "}"
  };
  auto jwt = header_base64 + "." + base64_encode((uint8_t*)payload.data(), payload.length());

  std::array<uint8_t, SHA256_HASH_LEN> hash;
  mbedtls_md(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), (const unsigned char *) jwt.c_str(), jwt.length(), hash.data());

  auto pk_ctx = std::make_unique<mbedtls_pk_context>();
  mbedtls_pk_init(pk_ctx.get());
  mbedtls_pk_parse_key(pk_ctx.get(), (const unsigned char *) PRIVATE_KEY, strlen(PRIVATE_KEY) + 1, nullptr, 0);

  auto entropy_ctx = std::make_unique<mbedtls_entropy_context>();
  auto ctr_drbg_ctx = std::make_unique<mbedtls_ctr_drbg_context>();

  mbedtls_entropy_init(entropy_ctx.get());
  mbedtls_ctr_drbg_init(ctr_drbg_ctx.get());
  mbedtls_ctr_drbg_seed(ctr_drbg_ctx.get(), mbedtls_entropy_func, entropy_ctx.get(), NULL, 0);

  std::array<uint8_t, SIGNATURE_LEN> signature;
  size_t signature_len = 0;
  mbedtls_pk_sign(pk_ctx.get(), MBEDTLS_MD_SHA256, (const unsigned char *)hash.data(), SHA256_HASH_LEN, signature.data(), &signature_len, mbedtls_ctr_drbg_random, ctr_drbg_ctx.get());

  const auto signature_base64 = base64_encode(signature.data(), signature.size());

  jwt += "." + signature_base64;

  mbedtls_pk_free(pk_ctx.get());
  mbedtls_entropy_free(entropy_ctx.get());
  mbedtls_ctr_drbg_free(ctr_drbg_ctx.get());

  return jwt;
}

// code generated by ChatGPT
std::string token_manager::extract_json(const std::string& input)
{
  std::string result;
  const size_t start_pos = input.find('{');
  const size_t end_pos = input.find('}');

  if (start_pos != std::string::npos && end_pos != std::string::npos && end_pos > start_pos)
    result = input.substr(start_pos, end_pos - start_pos + 1);

  return result;
}
