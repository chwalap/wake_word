#pragma once
#include <auth/GAuthManager.h>
#include "command_detector/command_detector.h"

#define PROJECT_ID "komputer-385115"
#define CLIENT_EMAIL "speech@komputer-385115.iam.gserviceaccount.com"
#define CLIENT_ID "114617957847718509525"
#define PRIVATE_KEY_ID "3c7e4e52a0297f23c24f9f47425d79dba445e5dc"
const char PRIVATE_KEY[] PROGMEM = "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQChuvAXJE1cHuJQ\nWwu9e9YgYM/jaePu+L/BOqOMols/sPCsyQUIsU033dZNDZ4XzbKqwsE4wN1LUfYF\nELl1jwcwyqlnm+6jJb3FLdrD8rmsD4Ml8qcZnPHndhY3ba7mlWRN0CsNeYhMsOtJ\nBcUJ+byQnIVPuYlE+YviqEYHzzaHdN7qRxJjZfEDaH4/QhHEcyiV40l4f0TPI5Xu\nK7kjdqtul3B3Q4WUP5JE+DvhS35u06bkx/9etKd4L3mOKQVpJU6Z6UlLtkhr4x+R\nN4G3yJ8HhnC3w1Mvb8U4TfoOUz434b7T+p951EEno16X130qdqXaulGuyRLTYBCh\nRBRuv0y7AgMBAAECggEABDgZ8r8BMtghak9w8N2hE+r119M+ELcCwm05Sz47219n\n0yVS95hi6p3joDks0lnzTi4ffhxviXswmMHVCY9hWvEzVl8eH7mooWy7tzHtWXyo\n6YQF8cHQGiXQy/Uu3P5+HxbBQ/kGlVpmNCgtrQylUfzeidpnjabLv4fi4wN39jRx\n8eIbvWj/2oLZJ0wl0nX1Qt/JZLUSogt8qvvUw/9+vrex+cGQSv6diQy2Pry+n2d+\nY632UvhNv7weRpGx9X2/004npSXUxJlcww49hsCjMlBgPaiRtlRrrzcHCf7auWup\nnHOPOLhpX9/Fai/rDsxXta5bUJPNtH/+KQdBsh5RdQKBgQDhqQl/GukpyJSV+iAh\n6lGApUYdjBoIl/4hQ8cbjERHLF7Xbv3SnT9cpz9U4MF3NxeoWDejT6aqWZ8j+8Dv\nFqldGaNJg7v7a9Ls5N5uBzXmHN8a5O0csvL8wpSuUZodLD6qgzM83Xw2SQgO3vR8\nFHbGuEZDzqw7vGd6T0c2UjoOpwKBgQC3eYCnE0BKi3liLTCfdWxmXdpEkg0UsrBR\nJzJEhkhlZQ7iCOzayQvW2tJT9MG0itWOCwV7G9pMO6Wjy3WmHRpQe+tMPCE+hDa+\nyp+xsKNwtQ2UjpJT8GgOyNbcNkNcRNoxD/alL3eqwe0CBD4ro0nPHAjnv4NKbeB8\nacNSP60HzQKBgQDCMUN43GG2kWQ8xGkOhqTovlHeb4ifyJzrZ3Za4JSJY0eiwWMw\n8h1q0IqRsnEK9KApQPFwWsUkZ8epoabgmXHaKov4YaNNs/Zp4nL/4LijdEkSzdq3\nDdBJb/GfKZ4kaMTs28cmI7wWwzZqBbgDZ2DnlgDxrVNOfM3X1Y2E07IelQKBgEFJ\nItZQq1fbOV4N5ndXOTNTwg8USUNMMiEuaVyoy6oIw7r6og1X7LaPNrhZJ3Qu0r6w\nfUrJJmxp7Q7TnxF02aA5+AgwfUPZmAFb6Gc6C25XfnWWJctoiB56qLnDNUTn2xwK\nKiGtOk+wob1N9TFh8SmZODH+m6EoZ/DdUXs63T8hAoGBAKNDu+SEjYhHjBXtHk1V\nD9veWhiwYLuJP0z/VHFS52p5bOSTbfyikTCBVyc++/l2aQF/emrnt4blShAmFx9+\naL6xHlv1lSj462qTXerIbHvuJltS1VZ9TsfPOue0vEvbawdY0PrF8wehbACFs4/H\nHTk5pf5UTCxcYbMm3RH6VjaP\n-----END PRIVATE KEY-----\n";
const char rootCACert[] PROGMEM = "-----BEGIN CERTIFICATE-----\n"
                                  "MIIFVzCCAz+gAwIBAgINAgPlk28xsBNJiGuiFzANBgkqhkiG9w0BAQwFADBHMQsw\n"
                                  "CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n"
                                  "MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAw\n"
                                  "MDAwWjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n"
                                  "Y2VzIExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjEwggIiMA0GCSqGSIb3DQEBAQUA\n"
                                  "A4ICDwAwggIKAoICAQC2EQKLHuOhd5s73L+UPreVp0A8of2C+X0yBoJx9vaMf/vo\n"
                                  "27xqLpeXo4xL+Sv2sfnOhB2x+cWX3u+58qPpvBKJXqeqUqv4IyfLpLGcY9vXmX7w\n"
                                  "Cl7raKb0xlpHDU0QM+NOsROjyBhsS+z8CZDfnWQpJSMHobTSPS5g4M/SCYe7zUjw\n"
                                  "TcLCeoiKu7rPWRnWr4+wB7CeMfGCwcDfLqZtbBkOtdh+JhpFAz2weaSUKK0Pfybl\n"
                                  "qAj+lug8aJRT7oM6iCsVlgmy4HqMLnXWnOunVmSPlk9orj2XwoSPwLxAwAtcvfaH\n"
                                  "szVsrBhQf4TgTM2S0yDpM7xSma8ytSmzJSq0SPly4cpk9+aCEI3oncKKiPo4Zor8\n"
                                  "Y/kB+Xj9e1x3+naH+uzfsQ55lVe0vSbv1gHR6xYKu44LtcXFilWr06zqkUspzBmk\n"
                                  "MiVOKvFlRNACzqrOSbTqn3yDsEB750Orp2yjj32JgfpMpf/VjsPOS+C12LOORc92\n"
                                  "wO1AK/1TD7Cn1TsNsYqiA94xrcx36m97PtbfkSIS5r762DL8EGMUUXLeXdYWk70p\n"
                                  "aDPvOmbsB4om3xPXV2V4J95eSRQAogB/mqghtqmxlbCluQ0WEdrHbEg8QOB+DVrN\n"
                                  "VjzRlwW5y0vtOUucxD/SVRNuJLDWcfr0wbrM7Rv1/oFB2ACYPTrIrnqYNxgFlQID\n"
                                  "AQABo0IwQDAOBgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4E\n"
                                  "FgQU5K8rJnEaK0gnhS9SZizv8IkTcT4wDQYJKoZIhvcNAQEMBQADggIBAJ+qQibb\n"
                                  "C5u+/x6Wki4+omVKapi6Ist9wTrYggoGxval3sBOh2Z5ofmmWJyq+bXmYOfg6LEe\n"
                                  "QkEzCzc9zolwFcq1JKjPa7XSQCGYzyI0zzvFIoTgxQ6KfF2I5DUkzps+GlQebtuy\n"
                                  "h6f88/qBVRRiClmpIgUxPoLW7ttXNLwzldMXG+gnoot7TiYaelpkttGsN/H9oPM4\n"
                                  "7HLwEXWdyzRSjeZ2axfG34arJ45JK3VmgRAhpuo+9K4l/3wV3s6MJT/KYnAK9y8J\n"
                                  "ZgfIPxz88NtFMN9iiMG1D53Dn0reWVlHxYciNuaCp+0KueIHoI17eko8cdLiA6Ef\n"
                                  "MgfdG+RCzgwARWGAtQsgWSl4vflVy2PFPEz0tv/bal8xa5meLMFrUKTX5hgUvYU/\n"
                                  "Z6tGn6D/Qqc6f1zLXbBwHSs09dR2CQzreExZBfMzQsNhFRAbd03OIozUhfJFfbdT\n"
                                  "6u9AWpQKXCBfTkBdYiJ23//OYb2MI3jSNwLgjt7RETeJ9r/tSQdirpLsQBqvFAnZ\n"
                                  "0E6yove+7u7Y/9waLd64NnHi/Hm3lCXRSHNboTXns5lndcEZOitHTtNCjv0xyBZm\n"
                                  "2tIMPNuzjsmhDYAPexZ3FL//2wmUspO8IFgV6dtxQ/PeEMMA3KgqlbbC1j+Qa3bb\n"
                                  "bP6MvPJwNQzcmRk13NfIRmPVNnGuV/u3gm3c\n"
                                  "-----END CERTIFICATE-----\n";

class GoogleSpeechAPI
{
public:
  GoogleSpeechAPI() { recording = new uint16_t[recLen]; }
  ~GoogleSpeechAPI() { delete[] recording; }

  bool begin();
  bool setSecure();
  bool run(command_detector_ptr &cd);
  std::string prepareRequest(int len);
  std::string preparePayload(int &len);

// private:
  const size_t recLen = 16000 * 2;
  uint16_t *recording;
  uint32_t mb_ts = 0;
  uint32_t mb_ts_offset = 0;
  gauth_cfg_t m_Config;
  GAuthManager m_AuthMan;
  WiFiClientSecure m_Client;
  bool cert_updated = false;
  int cert_addr = 0;
  int response_code = 0;
  MB_FS mbfs;

  // const char* TOKEN_API_URL = "https://accounts.google.com/o/oauth2/v2/auth";
  // const int TOKEN_LIFETIME = 30 * 24 * 60 * 60; // 30 days
  // const char* SPEECH_API_URL = "https://speech.googleapis.com/v1/speech:recognize";

  // const char* CONFIG = R"(
  //   {
  //     "encoding": LINEAR16,
  //     "sampleRateHertz": 16000,
  //     "audioChannelCount": 1,
  //     "enableSeparateRecognitionPerChannel": false,
  //     "languageCode": "pl-PL",
  //     "model": "latest_short",
  //     "useEnhanced": false
  //   }
  // )";

  // const char* CLIENT_KEY = R"(
  //   {
  //     "type": "service_account",
  //     "project_id": "komputer-385115",
  //     "private_key_id": "3c7e4e52a0297f23c24f9f47425d79dba445e5dc",
  //     "private_key": "-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQChuvAXJE1cHuJQ\nWwu9e9YgYM/jaePu+L/BOqOMols/sPCsyQUIsU033dZNDZ4XzbKqwsE4wN1LUfYF\nELl1jwcwyqlnm+6jJb3FLdrD8rmsD4Ml8qcZnPHndhY3ba7mlWRN0CsNeYhMsOtJ\nBcUJ+byQnIVPuYlE+YviqEYHzzaHdN7qRxJjZfEDaH4/QhHEcyiV40l4f0TPI5Xu\nK7kjdqtul3B3Q4WUP5JE+DvhS35u06bkx/9etKd4L3mOKQVpJU6Z6UlLtkhr4x+R\nN4G3yJ8HhnC3w1Mvb8U4TfoOUz434b7T+p951EEno16X130qdqXaulGuyRLTYBCh\nRBRuv0y7AgMBAAECggEABDgZ8r8BMtghak9w8N2hE+r119M+ELcCwm05Sz47219n\n0yVS95hi6p3joDks0lnzTi4ffhxviXswmMHVCY9hWvEzVl8eH7mooWy7tzHtWXyo\n6YQF8cHQGiXQy/Uu3P5+HxbBQ/kGlVpmNCgtrQylUfzeidpnjabLv4fi4wN39jRx\n8eIbvWj/2oLZJ0wl0nX1Qt/JZLUSogt8qvvUw/9+vrex+cGQSv6diQy2Pry+n2d+\nY632UvhNv7weRpGx9X2/004npSXUxJlcww49hsCjMlBgPaiRtlRrrzcHCf7auWup\nnHOPOLhpX9/Fai/rDsxXta5bUJPNtH/+KQdBsh5RdQKBgQDhqQl/GukpyJSV+iAh\n6lGApUYdjBoIl/4hQ8cbjERHLF7Xbv3SnT9cpz9U4MF3NxeoWDejT6aqWZ8j+8Dv\nFqldGaNJg7v7a9Ls5N5uBzXmHN8a5O0csvL8wpSuUZodLD6qgzM83Xw2SQgO3vR8\nFHbGuEZDzqw7vGd6T0c2UjoOpwKBgQC3eYCnE0BKi3liLTCfdWxmXdpEkg0UsrBR\nJzJEhkhlZQ7iCOzayQvW2tJT9MG0itWOCwV7G9pMO6Wjy3WmHRpQe+tMPCE+hDa+\nyp+xsKNwtQ2UjpJT8GgOyNbcNkNcRNoxD/alL3eqwe0CBD4ro0nPHAjnv4NKbeB8\nacNSP60HzQKBgQDCMUN43GG2kWQ8xGkOhqTovlHeb4ifyJzrZ3Za4JSJY0eiwWMw\n8h1q0IqRsnEK9KApQPFwWsUkZ8epoabgmXHaKov4YaNNs/Zp4nL/4LijdEkSzdq3\nDdBJb/GfKZ4kaMTs28cmI7wWwzZqBbgDZ2DnlgDxrVNOfM3X1Y2E07IelQKBgEFJ\nItZQq1fbOV4N5ndXOTNTwg8USUNMMiEuaVyoy6oIw7r6og1X7LaPNrhZJ3Qu0r6w\nfUrJJmxp7Q7TnxF02aA5+AgwfUPZmAFb6Gc6C25XfnWWJctoiB56qLnDNUTn2xwK\nKiGtOk+wob1N9TFh8SmZODH+m6EoZ/DdUXs63T8hAoGBAKNDu+SEjYhHjBXtHk1V\nD9veWhiwYLuJP0z/VHFS52p5bOSTbfyikTCBVyc++/l2aQF/emrnt4blShAmFx9+\naL6xHlv1lSj462qTXerIbHvuJltS1VZ9TsfPOue0vEvbawdY0PrF8wehbACFs4/H\nHTk5pf5UTCxcYbMm3RH6VjaP\n-----END PRIVATE KEY-----\n",
  //     "client_email": "speech@komputer-385115.iam.gserviceaccount.com",
  //     "client_id": "114617957847718509525",
  //     "auth_uri": "https://accounts.google.com/o/oauth2/auth",
  //     "token_uri": "https://oauth2.googleapis.com/token",
  //     "auth_provider_x509_cert_url": "https://www.googleapis.com/oauth2/v1/certs",
  //     "client_x509_cert_url": "https://www.googleapis.com/robot/v1/metadata/x509/speech%40komputer-385115.iam.gserviceaccount.com"
  //   }
  // )";
};
