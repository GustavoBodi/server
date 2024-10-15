#include <concepts>
#include <http/HttpRequest.hpp>
#include "http/HttpResponse.hpp"

template <typename T>
concept Middleware = requires(T a, HttpRequest &req) {
  { a.apply(req) } -> std::same_as<void>;
};

