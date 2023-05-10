#include "google_api.h"
#include "audio_input/ring_buffer.h"
#include "utils/base64.h"

using namespace std;

bool GoogleSpeechAPI::begin()
{
  int addr = reinterpret_cast<int>(rootCACert);
  if (addr != cert_addr)
  {
      cert_updated = true;
      cert_addr = addr;
  }

  m_Config.service_account.data.client_email = CLIENT_EMAIL;
  m_Config.service_account.data.project_id = PROJECT_ID;
  m_Config.service_account.data.private_key = PRIVATE_KEY;
  m_Config.service_account.data.private_key_id = PRIVATE_KEY_ID;
  m_Config.service_account.data.client_id = CLIENT_ID;
  m_Config.signer.tokens.token_type = token_type_oauth2_access_token;
  m_Config.signer.tokens.expires = 0;
  m_Config.signer.preRefreshSeconds = 30 * 60;
  m_Config.signer.expiredSeconds = 3600;
  m_Config.internal.reconnect_wifi = WiFi.getAutoReconnect();

  m_AuthMan.begin(&m_Config, &mbfs, &mb_ts, &mb_ts_offset);
  m_AuthMan.newClient(&m_AuthMan.tcpClient);
  m_AuthMan.begin(&m_Config, &mbfs, &mb_ts, &mb_ts_offset);
  GS_TCP_Client *client = m_AuthMan.tcpClient;

  if (!setSecure())
      return false;

//   if (client && !client->connected())
//     if (!client->begin((const char *)FPSTR("www.googleapis.com"), 443, &response_code))
//       return false;

  while (!m_AuthMan.tokenReady())
    delay(2000);

  Serial.println(m_Config.internal.auth_token.substr(0, 128).c_str());

  return true;
}

bool GoogleSpeechAPI::setSecure()
{
  GS_TCP_Client *client = m_AuthMan.tcpClient;

  if (!client)
      return false;

  client->setConfig(&m_Config, &mbfs);

  if (!m_AuthMan.reconnect(client))
      return false;

  if (TimeHelper::getTime(&mb_ts, &mb_ts_offset) > GS_DEFAULT_TS)
  {
      m_Config.internal.clock_rdy = true;
      client->setClockStatus(true);
  }

  if (client->getCertType() == gs_cert_type_undefined || cert_updated)
  {

      if (!m_Config.internal.clock_rdy && (m_Config.cert.file.length() > 0 || m_Config.cert.data != NULL || cert_addr > 0))
          TimeHelper::syncClock(&m_AuthMan.ntpClient, &mb_ts, &mb_ts_offset, m_Config.internal.gmt_offset, &m_Config);

      if (m_Config.cert.file.length() == 0)
      {
          if (cert_addr > 0)
              client->setCACert(reinterpret_cast<const char *>(cert_addr));
          else if (m_Config.cert.data != NULL)
              client->setCACert(m_Config.cert.data);
          else
              client->setCACert(NULL);
      }
      else
      {
          if (!client->setCertFile(m_Config.cert.file.c_str(), m_Config.cert.file_storage))
              client->setCACert(NULL);
      }
      cert_updated = false;
  }
  return true;
}

bool GoogleSpeechAPI::run(command_detector_ptr &cd)
{
    bool ret = true;
    static size_t recordingIdx = 0;
    static size_t lastIdx = 0;
    static bool isRecording = false;
    static long start = 0;

    ring_buffer_ptr reader = cd->m_sampler->get_ring_buffer_reader();

    if (!isRecording)
    {
        start = millis();
        isRecording = true;
        recordingIdx = 0;

        reader->rewind(16000);
        auto startIdx = reader->get_index();
        for (size_t i = 0; i < 16000; i++)
        {
            recording[recordingIdx] = reader->get_current_sample();
            reader->move_to_next_sample();
            recordingIdx++;
        }
        lastIdx = reader->get_index();
        reader->set_index(startIdx);
        Serial.printf("Current recordingIdx: %zu; time: %u\n", recordingIdx, millis() - start);
        return ret;
    }

    auto currentIdx = reader->get_index();
    reader->set_index(lastIdx);
    while (reader->get_index() != currentIdx)
    {
        recording[recordingIdx] = reader->get_current_sample();
        reader->move_to_next_sample();
        recordingIdx++;
        if (recordingIdx == recLen)
        {
            Serial.printf("All recordings captured %zu == %zu!\n", recordingIdx, recLen);
            Serial.printf("Recorded command... %ums\n", millis() - start);
            // cd->komputerDetected = false;
            ret = false;
            isRecording = false;
            break;
        }
    }
    lastIdx = reader->get_index();
    reader->set_index(currentIdx);
    Serial.printf("Current recordingIdx: %zu; time: %u\n", recordingIdx, millis() - start);

    if (recordingIdx == recLen)
    {
        Serial.println("Sending request to GoogleAPI!");
        int len = 0;
        auto payload = preparePayload(len);
        auto request = prepareRequest(len);

        auto REQ = request + payload;
        // Serial.println(REQ.c_str());
        Serial.println(m_Config.internal.auth_token.c_str());

        // GS_TCP_Client *client = m_AuthMan.tcpClient;

        // if (client && !client->connected())
        //     if (!client->begin((const char *)FPSTR("speech.googleapis.com"), 443, &response_code))
        //         return;

        // Serial.printf("response code %d\n", response_code);

        // if (!m_AuthMan.tokenReady())
        //     return;

        // if (!client)
        // {
        //     Serial.println("Invalid client!");
        //     return;
        // }

        // int ret = client->send(REQ.c_str());

        WiFiClientSecure C;
        C.setCACert(rootCACert);
        C.setInsecure();
        C.stop();
        if (C.connect("googleapis.com", 443)) {
        // if (C.connect("speech.googleapis.com", 443)) {
            Serial.println("CONNECTED!");
            C.print(REQ.c_str());
            while (C.connected()) {
                String line = C.readStringUntil('\n'); //HTTP headers
                if (line == "\r") {
                    break;
                }
            }
            String content = C.readString();
            Serial.println(content);
            // String line = client.readStringUntil('\n'); //payload first row
        } else {
            Serial.println(F("Connection wasnt established"));
        }
    }

        // GS_TCP_Client *client = m_AuthMan.tcpClient;

        // if (!setSecure())
        //     return;

        // if (client && !client->connected())
        //     if (!client->begin((const char *)FPSTR("www.googleapis.com"), 443, &response_code))
        //         return;

        // if (!m_AuthMan.tokenReady())
        //     return;

        // if (!client)
        // {
        //     Serial.println("Invalid client!");
        //     return;
        // }

        // Serial.println("Asdf");
        // int ret = client->send(REQ.c_str());
        // if (ret > 0)
        // {
        //     int httpcode = 0;
        //     MB_String response;
        //     ret = m_AuthMan.handleResponse(client, httpcode, response, false);
        //     if (!ret)
        //     {
        //         Serial.printf("Error during reading response: %s\n", response.c_str());
        //         return;
        //     }
        //     Serial.println(response.c_str());
        // }
        // else
        // {
        //     Serial.println("Error during send!");
        //     return;
        // }
    // }

    return ret;
}

string GoogleSpeechAPI::preparePayload(int &len)
{
    auto base64 = base64_encode((uint8_t*) recording, recLen * 2);

    string payload =
R"({
    "config": {
        "languageCode": "pl-PL",
        "encoding": "LINEAR16",
        "sampleRateHertz": 16000,
        "enableWordTimeOffsets": false
    },
    "audio": {
        "content": ")" + base64 + R"("
    }
}
)";
    len = payload.length();
    return payload;
}

string GoogleSpeechAPI::prepareRequest(int len)
{
    string req;
    req += "POST /v1/speech:recognize HTTP/1.1\r\n";
    req += "Authorization: Bearer ";
    req += m_Config.internal.auth_token.c_str();
    req += "\r\n";
    req += "Host: speech.googleapis.com\r\n";
    req += "Accept: application/json\r\n";
    req += "Content-Type: application/json\r\n";
    req += "Content-length: " + to_string(len) + "\r\n";
    // req += "Host: speech.googleapis.com\r\n";
    // req += "Content-Type: application/json\r\n";
    // req += "Connection: keep-alive\r\n";
    // req += "Keep-Alive: timeout=30, max=100\r\n";
    // req += "Accept-Encoding: identity;q=1,chunked;q=0.1,*;q=0\r\n";
    req += "\r\n";
    return req;
}

  // Serial.println(generate_api_token().c_str());

  // Set the callback for Google API access token generation status (for debug only)
  // GSheet.setTokenCallback(tokenStatusCallback);

  // // Set the seconds to refresh the auth token before expire (60 to 3540, default is 300 seconds)
  // GSheet.setPrerefreshSeconds(30 * 60);

  // // Begin the access token generation for Google API authentication
  // GSheet.begin(CLIENT_EMAIL, PROJECT_ID, PRIVATE_KEY);

  //LINEAR16
  // latest_short
  // pl-PL

  // m_Client.onMessage(
  //   [](WebsocketsMessage message)
  //   {
  //     Serial.print("Got Message: ");
  //     Serial.println(message.data());

  //     if (message.isText())
  //     {
  //       DynamicJsonDocument doc(1024);
  //       deserializeJson(doc, message.c_str(), message.length());

  //       if (doc.containsKey("result")) {
  //         const char* transcript = doc["result"][0]["alternative"][0]["transcript"];
  //         Serial.println("Transcript: " + String(transcript));
  //       }
  //     }
  //   }
  // );

  // m_Client.onEvent(
  //   [](WebsocketsEvent event, String data)
  //   {
  //     if(event == WebsocketsEvent::ConnectionOpened) {
  //         Serial.println("Connnection Opened");
  //     } else if(event == WebsocketsEvent::ConnectionClosed) {
  //         Serial.println("Connnection Closed");
  //     } else if(event == WebsocketsEvent::GotPing) {
  //         Serial.println("Got a Ping!");
  //     } else if(event == WebsocketsEvent::GotPong) {
  //         Serial.println("Got a Pong!");
  //     }
  //   }
  // );

  // m_Client.setPrivateKey(CLIENT_KEY);
  // m_Client.connect(SPEECH_API_URL);

  // m_Client.setPrivateKey(CLIENT_KEY);
  // m_WebSocket.begin(SPEECH_API_URL, F("speech.googleapis.com"), 443, "/v1beta1/speech:recognize");
  // m_WebSocket.setExtraHeaders(F("Sec-WebSocket-Protocol: base64"));

  // m_WebSocket.onEvent(
  //   [](WStype_t type, uint8_t* payload, size_t length) {
  //     if (type == WStype_TEXT) {
  //       DynamicJsonDocument doc(1024);
  //       deserializeJson(doc, payload, length);

  //       if (doc.containsKey("result")) {
  //         const char* transcript = doc["result"][0]["alternative"][0]["transcript"];
  //         Serial.println("Transcript: " + String(transcript));
  //       }
  //     }
  //   }
  // );


  // todo: create a new task for the below
  // client.poll();



//   const String ClientEmail{"speech@komputer-385115.iam.gserviceaccount.com"};
//   const String CurrentTime{time(nullptr)};
//   const String ExpireDate{time(nullptr) + TOKEN_LIFETIME};
//   const String JwtHeader{R"({"alg":"RS256","typ":"JWT"})"};
//   const String JwtPayload{
//     "{"
//       "\"iss\":\"" + ClientEmail + "\","
//       "\"scope\":\"https://www.googleapis.com/auth/cloud-platform\","
//       "\"aud\":\"" + String{TOKEN_API_URL} + "\","
//       "\"exp\":" + ExpireDate + ","
//       "\"iat\":" + CurrentTime + ""
//     "}"
//   };
//   const String JwtRequest{JwtHeader + "." + base64::encode(JwtPayload)};
//   Serial.println(JwtRequest);

//   String PrivateKey{"-----BEGIN PRIVATE KEY-----\nMIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQChuvAXJE1cHuJQ\nWwu9e9YgYM/jaePu+L/BOqOMols/sPCsyQUIsU033dZNDZ4XzbKqwsE4wN1LUfYF\nELl1jwcwyqlnm+6jJb3FLdrD8rmsD4Ml8qcZnPHndhY3ba7mlWRN0CsNeYhMsOtJ\nBcUJ+byQnIVPuYlE+YviqEYHzzaHdN7qRxJjZfEDaH4/QhHEcyiV40l4f0TPI5Xu\nK7kjdqtul3B3Q4WUP5JE+DvhS35u06bkx/9etKd4L3mOKQVpJU6Z6UlLtkhr4x+R\nN4G3yJ8HhnC3w1Mvb8U4TfoOUz434b7T+p951EEno16X130qdqXaulGuyRLTYBCh\nRBRuv0y7AgMBAAECggEABDgZ8r8BMtghak9w8N2hE+r119M+ELcCwm05Sz47219n\n0yVS95hi6p3joDks0lnzTi4ffhxviXswmMHVCY9hWvEzVl8eH7mooWy7tzHtWXyo\n6YQF8cHQGiXQy/Uu3P5+HxbBQ/kGlVpmNCgtrQylUfzeidpnjabLv4fi4wN39jRx\n8eIbvWj/2oLZJ0wl0nX1Qt/JZLUSogt8qvvUw/9+vrex+cGQSv6diQy2Pry+n2d+\nY632UvhNv7weRpGx9X2/004npSXUxJlcww49hsCjMlBgPaiRtlRrrzcHCf7auWup\nnHOPOLhpX9/Fai/rDsxXta5bUJPNtH/+KQdBsh5RdQKBgQDhqQl/GukpyJSV+iAh\n6lGApUYdjBoIl/4hQ8cbjERHLF7Xbv3SnT9cpz9U4MF3NxeoWDejT6aqWZ8j+8Dv\nFqldGaNJg7v7a9Ls5N5uBzXmHN8a5O0csvL8wpSuUZodLD6qgzM83Xw2SQgO3vR8\nFHbGuEZDzqw7vGd6T0c2UjoOpwKBgQC3eYCnE0BKi3liLTCfdWxmXdpEkg0UsrBR\nJzJEhkhlZQ7iCOzayQvW2tJT9MG0itWOCwV7G9pMO6Wjy3WmHRpQe+tMPCE+hDa+\nyp+xsKNwtQ2UjpJT8GgOyNbcNkNcRNoxD/alL3eqwe0CBD4ro0nPHAjnv4NKbeB8\nacNSP60HzQKBgQDCMUN43GG2kWQ8xGkOhqTovlHeb4ifyJzrZ3Za4JSJY0eiwWMw\n8h1q0IqRsnEK9KApQPFwWsUkZ8epoabgmXHaKov4YaNNs/Zp4nL/4LijdEkSzdq3\nDdBJb/GfKZ4kaMTs28cmI7wWwzZqBbgDZ2DnlgDxrVNOfM3X1Y2E07IelQKBgEFJ\nItZQq1fbOV4N5ndXOTNTwg8USUNMMiEuaVyoy6oIw7r6og1X7LaPNrhZJ3Qu0r6w\nfUrJJmxp7Q7TnxF02aA5+AgwfUPZmAFb6Gc6C25XfnWWJctoiB56qLnDNUTn2xwK\nKiGtOk+wob1N9TFh8SmZODH+m6EoZ/DdUXs63T8hAoGBAKNDu+SEjYhHjBXtHk1V\nD9veWhiwYLuJP0z/VHFS52p5bOSTbfyikTCBVyc++/l2aQF/emrnt4blShAmFx9+\naL6xHlv1lSj462qTXerIbHvuJltS1VZ9TsfPOue0vEvbawdY0PrF8wehbACFs4/H\nHTk5pf5UTCxcYbMm3RH6VjaP\n-----END PRIVATE KEY-----\n"};
//   PrivateKey.replace("\\n", "\n");

//   std::unique_ptr<char[]> PrivateKeyCharArray(new char[PrivateKey.length()]);
//   PrivateKey.toCharArray(PrivateKeyCharArray.get(), PrivateKey.length());

//   uint8_t SHA256Hash[32]{};
//   hmac<SHA256>(SHA256Hash, 32, PrivateKeyCharArray.get(), PrivateKey.length(), JwtRequest.c_str(), JwtRequest.length());

//   String SHA256HashBase64{base64::encode(SHA256Hash, 32)};
//   String JwtToken{JwtRequest + "." + SHA256HashBase64};
//   String PostData = "grant_type=urn%3Aietf%3Aparams%3Aoauth%3Agrant-type%3Ajwt-bearer&assertion=" + JwtToken;

//   m_Client.connect(TOKEN_API_URL, 443);
//   m_Client.println("POST /oauth2/v4/token HTTP/1.1");
//   m_Client.println("Host: www.googleapis.com");
//   m_Client.println("Content-Type: application/x-www-form-urlencoded");
//   m_Client.println("Content-Length: " + String(PostData.length()));
//   m_Client.println("Connection: close");
//   m_Client.println();
//   m_Client.print(PostData);

//   while (m_Client.connected() && !m_Client.available()) {
//     delay(10);
//   }

//   String Response;
//   while (m_Client.available()) {
//     Response += m_Client.readStringUntil('\n');
//   }

//   int TokenStart = Response.indexOf("\"access_token\":\"") + 16;
//   int TokenEnd = Response.indexOf("\"", TokenStart);
//   String AccessToken = Response.substring(TokenStart, TokenEnd);
//   return string{AccessToken.c_str()};
// }
