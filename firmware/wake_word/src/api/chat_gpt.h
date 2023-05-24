#ifndef CHAT_GPT_H
#define CHAT_GPT_H

#include <memory>

#include <WiFiClientSecure.h>

struct chat_gpt
{
  chat_gpt();

  std::string prepare_request() const;
  std::string prepare_payload() const;
  std::string get_api_token() const;
  std::string get_startup_message() const;
  void send_message(const std::string& message);

  std::string m_message;
  WiFiClientSecure m_client;
};

using chat_gpt_ptr = std::unique_ptr<chat_gpt>;

#endif
