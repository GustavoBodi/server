#pragma once
#include "controller/Controller.hpp"
#include "http/HttpResponse.hpp"
#include "server/ServiceLocator.hpp"
#include <server/MiddlewareController.hpp>
#include <uWebSockets/App.h>

class Server {
public:
  Server(unsigned int port = 8080): port_{port} {}

  void run() {
    app_
      .listen(port_, [this](auto *token) {
        if (token) {
        std::cout << "Server started on port " << port_ << std::endl;
      } else {
        std::cout << "Failed to start server" << std::endl;
      }
    })
    .run();
  }

  template <typename Ctrl, HttpVerbs Verb, typename... RouteArgs>
  requires ControllerConcept<Ctrl, RouteArgs...>
  constexpr void register_route(const std::string& path, HttpResponse (Ctrl::*method)(ControllerRequest&, RouteArgs...) const);

  ServiceLocator& serviceLocator() { return serviceLocator_; }

  MiddlewareController middlewareController() { return middlewareController_; }

  MiddlewareController middlewareController_ {};

  ~Server() {}
private:

  // Helper function to create the tuple from vector elements
  template <std::size_t... I>
  auto vector_to_tuple(std::shared_ptr<std::vector<std::string>> params, std::index_sequence<I...>) {
      return std::make_tuple((*params)[I]...);
  }

  ServiceLocator serviceLocator_ {};
  //MiddlewareController<Middlewares...> middlewareController_ {};
  unsigned int port_;
  uWS::App app_ {};
};

template <typename Ctrl, HttpVerbs Verb, typename... RouteArgs>
requires ControllerConcept<Ctrl, RouteArgs...>
constexpr void Server::register_route(const std::string &path, HttpResponse (Ctrl::*method)(ControllerRequest&, RouteArgs...) const) {
    auto func = [this, method](auto *res, auto *req) {
        auto body = std::make_shared<std::string>();
        auto params = std::make_shared<std::vector<std::string>>();

        constexpr std::size_t sz = std::tuple_size<std::tuple<RouteArgs...>>::value;

        for (size_t i = 0; i < sz; ++i) {
            params->push_back(std::string(req->getParameter(i)));
        }

        res->onData([this, body, res, params, method](std::string_view data, bool isLast) {
            body->append(data.data(), data.length());
            if (isLast) {
                std::shared_ptr<Ctrl> controller = serviceLocator_.getService<Ctrl>();

                auto tuple = vector_to_tuple(params, std::make_index_sequence<sizeof...(RouteArgs)>{});
                HttpRequest request {};
                auto tuple_end = std::tuple_cat(std::tie(request), tuple);

                middlewareController_.apply(request);

                HttpResponse response = std::apply(
                    [controller, method](auto&&... args) -> HttpResponse {
                        return (controller.get()->*method)(std::forward<decltype(args)>(args)...);
                    },
                    tuple_end
                ); 
                res->writeHeader("Content-Type", "text/html; charset=utf-8");
                res->end("teste");
            }
        });

        res->onAborted([]() {
            std::cout << "Request was aborted!" << std::endl;
        });
    };


  if constexpr (Verb == HttpVerbs::Get) {
    app_.get(path, func);
  } else if constexpr (Verb == HttpVerbs::Post) {
    app_.post(path, func);
  } else if constexpr (Verb == HttpVerbs::Patch) {
    app_.patch(path, func);
  } else if constexpr (Verb == HttpVerbs::Put) {
    app_.put(path, func);
  } else if constexpr (Verb == HttpVerbs::Delete) {
    app_.del(path, func);
  } else {
    static_assert(!std::is_same<Ctrl, Ctrl>::value, "Invalid HttpVerb on Server::register_route(), not implemented.");
  }

}

