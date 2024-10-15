#pragma once
#include <string>
#include <unordered_map>
#include <string_view>

class HttpRequest {
public:
  HttpRequest() {}
  ~HttpRequest() = default;
private:
  std::unordered_map<std::string, std::string> headers {};
  std::string body {};
  std::unordered_map<std::string, std::string> injectedValues {};
};
