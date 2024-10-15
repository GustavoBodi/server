#pragma once
#include <string>
#include <iostream>
#include <concepts>
#include <memory>
#include "../http/HttpResponse.hpp"
#include "http/HttpRequest.hpp"
#include <server/Logger.hpp>

class UserController {
public:
  using dependencies = std::tuple<ILogger>;

  UserController(std::shared_ptr<ILogger> logger)
    : _logger(std::move(logger)) {}

  HttpResponse Get(HttpRequest &request, std::string param2) const {
    _logger->testPrint();
    (void) request;
    std::cout << param2 << std::endl;
    return HttpResponse(200, "Ok").set_body("This is the response: " + std::string(param2));
  }

private:
  std::shared_ptr<ILogger> _logger;
};
