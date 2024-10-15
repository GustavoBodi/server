#pragma once
#include <concepts>
#include <string>
#include "../http/HttpVerbs.hpp"
#include "../http/HttpResponse.hpp"
#include "http/HttpRequest.hpp"

using ControllerRequest = HttpRequest;

template <typename T, typename ...Args>
concept HasGet = requires(const T a , ControllerRequest &req, Args ...args) {
  { a.Get(req, args...) } -> std::convertible_to<HttpResponse>;
};

template <typename T, typename ...Args>
concept HasPost = requires(const T a, ControllerRequest &req, Args ...args) {
  { a.Post(req, args...) } -> std::same_as<HttpResponse>;
};

template <typename T, typename ...Args>
concept HasDelete = requires(const T a, ControllerRequest &req, Args ...args) {
  { a.Delete(req, args...) } -> std::same_as<HttpResponse>;
};

template <typename T, typename ...Args>
concept HasPatch = requires(const T a, ControllerRequest &req, Args ...args) {
  { a.Patch(req, args...) } -> std::same_as<HttpResponse>;
};

template <typename T, typename ...Args>
concept HasPut = requires(const T a, ControllerRequest &req, Args ...args) {
  { a.Put(req, args...) } -> std::same_as<HttpResponse>;
};

template <typename T, typename ...Args>
concept ControllerConcept = HasGet<T, Args...> || HasPost<T, Args...> || HasDelete<T, Args...> || HasPatch<T, Args...> || HasPut<T, Args...>;
