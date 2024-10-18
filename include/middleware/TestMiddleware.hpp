#pragma once
#include <http/HttpRequest.hpp>
#include <iostream>

struct TestMiddleware {
  static void apply(HttpRequest &request) {
    request.setBody("Muitos testes aqui");
    std::cout << "Teste" << std::endl;
    (void) request;
  }
};
