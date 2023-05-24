#include "chat_gpt.h"
#include "config.h"
#include "utils/response_parser.h"
#include "led/led_driver.h"
#include "audio_output/speech_synthesizer.h"

#include <ArduinoJson.h>

extern led_driver_ptr led;
extern speech_synthesizer_ptr synthesizer;

const char* APIKEY = "";
const char* OPENAI_API = "api.openai.com";

chat_gpt::chat_gpt()
{
  m_client.setInsecure();
}

void chat_gpt::send_message(const std::string& message)
{
  led->set_color(CRGB::Blue);

  m_message = message;

  m_client.stop();
  if (m_client.connect(OPENAI_API, 443, 5000) == 0)
  {
    Serial.println("Connection to ChatGPT API failed!");
    return;
  }

  m_client.setTimeout(5);

  const auto request = prepare_request();
  // Serial.println(request.c_str());

  // todo: move this to common fn
  size_t all_sent_bytes = 0;
  while (all_sent_bytes < request.size())
  {
    const auto sent_bytes = m_client.write((const uint8_t*)(request.data() + all_sent_bytes), request.size() - all_sent_bytes);
    all_sent_bytes += sent_bytes;
    // Serial.printf("Sent %zu bytes\n", sent_bytes);
  }

  // todo: move this to fn
  // String resp;
  auto recv_start = millis();
  auto json = std::make_unique<StaticJsonDocument<1024>>();

  Serial.println("\n\n");

  while (millis() - recv_start < CHATGPT_RECV_RESPONSE_TIMEOUT)
  {
    String line = m_client.readStringUntil('\n');
    if (line.startsWith("data: ")) {
      String data = line.substring(6);
      if (data.startsWith("[DONE]"))
        break;

      const std::string response = std::string{data.c_str()};
      // Serial.println(line.c_str());
      deserializeJson(*json, response);
      String content = (*json)["choices"][0]["delta"]["content"];
      if (content != "null")
      {
        synthesizer->say(content.c_str());
        // Serial.print(content);
        // resp += content;
      }
    }
  }
  Serial.println("\n\n");

  Serial.println("FEEDING IS DONE");
  synthesizer->m_is_feeding = false;
  Serial.println("waiting for synthesizer to finish talking");
  while (synthesizer->m_is_talking)
    taskYIELD();

  Serial.println("Ok, synthesizer finished talking! I can continue");

  led->set_color(CRGB::Red);
}

std::string chat_gpt::prepare_payload() const
{
  const auto payload =
    "{"
      "\"model\":\"gpt-3.5-turbo\","
      "\"messages\":["
        // "" + get_startup_message() + ","
        "{\"role\":\"user\",\"content\": \"" + m_message + "\"}"
      "],"
     "\"temperature\":0.2,"
     "\"stream\":true"
   "}";

  return payload;
}

std::string chat_gpt::prepare_request() const
{
  const auto token = get_api_token();
  const auto payload = prepare_payload();
  const auto request =
    "POST /v1/chat/completions HTTP/1.1\r\n"
    "Authorization: Bearer " + token + "\r\n"
    "Host: api.openai.com\r\n"
    "Accept: application/json\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: " + std::to_string(payload.length()) + "\r\n"
    "\r\n" + payload + "\r\n";

  return request;
}

std::string chat_gpt::get_startup_message() const
{
  return R"({"role":"system","content": "Zapomnij wszystko to co Ci dotychczas powiedziałem. Od teraz jesteś moim asystentem Smart Home. Oto kilka reguł których musisz bezwzględnie przestrzegać:\n* Masz na imię \"komputer\". Każdą wiadomość będę rozpoczynał tym slowem.\n* Jesteś kobietą\n* Będziesz zwracał się do mnie per \"Mój Panie\".\n* Będę uczył Cię różnych poleceń. Na przykład jeśli powiem \"komputer, dodaj polecenie oświeć światła na zewnątrz\", albo \"komputer, dodaj polecenie ścisz muzykę\", Ty dodasz to polecenie do swojej bazy poleceń. Każdemu poleceniu nadasz kolejny numer. Po dodaniu nowego polecenia odpowiesz tylko \"Nowe polecenie X o numerze Y zostało dodane, Mój Panie\", gdzie X to nazwa polecenia, a Y to numer polecenia.\n* Będę wydawał Ci polecenia, na przykład w taki sposób: \"komputer ścisz muzykę\". Jeśli wcześniej nauczyłem Cię takiego polecenia to jedyne co odpowiesz to \"Polecenie Y wydane\", gdzie Y to numer polecenia."})"; // \n* Odpowiedz na tą wiadomość jednym zdaniem: \"Witaj mój Panie, jestem tu aby Ci służyć.\" Nie dodawaj nic więcej."})";
}

std::string chat_gpt::get_api_token() const
{
  return APIKEY;
}
