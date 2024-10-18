#pragma once
#include <string>
#include <unordered_map>
#include <string_view>

class HttpRequest {
public:
  HttpRequest() {}
  ~HttpRequest() = default;
  void setBody(std::string body) { body_ = body; }
private:
  std::unordered_map<std::string, std::string> headers_ {};
  std::string body_ {};
  std::unordered_map<std::string, std::string> injectedValues_ {};
};
