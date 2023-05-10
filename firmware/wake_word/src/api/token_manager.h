#ifndef TOKEN_MANAGER_H
#define TOKEN_MANAGER_H

#include "config.h"

#include <string>
#include <memory>

#include <WiFiClientSecure.h>
#include <NTPClient.h>

struct token_manager
{
  token_manager();
  const std::string& get_token();
  void fetch_new_token();
  bool token_expired();
  std::string extract_json(const std::string& input);
  std::string prepare_token_request();
  std::string get_jwt();

  WiFiClientSecure m_client;
  std::string m_token;
  unsigned long m_token_creation_time;
  WiFiUDP m_ntp_udp;
  NTPClient m_ntp_client{m_ntp_udp, "europe.pool.ntp.org"};
};

using token_manager_ptr = std::unique_ptr<token_manager>;

#endif
