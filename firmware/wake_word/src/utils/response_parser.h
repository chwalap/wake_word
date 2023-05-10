#ifndef RESPONSE_PARSER_H
#define RESPONSE_PARSER_H

#include <map>
#include <string>

std::string extract_json(const std::string& response);
std::map<std::string, std::string> parse_http_response(const std::string& response);

#endif
