#pragma once
#include <functional>
#include <unordered_map>
#include <server/Middleware.hpp>

class MiddlewareController {
public:
  MiddlewareController() {}

  void apply(HttpRequest& request) {
    for (auto& middleware: middlewares_) {
      middleware(request);
    }
  }

  void register_middleware(std::function<void(HttpRequest&)> middleware) {
    middlewares_.emplace_back(middleware);
  }

  ~MiddlewareController() = default;
private:
  std::vector<std::function<void(HttpRequest&)>> middlewares_ {};
};

