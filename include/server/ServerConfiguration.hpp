#pragma once
#include "controller/DataTablesController.hpp"
#include "controller/UserController.hpp"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>
#include <server/Router.hpp>
#include <server/ServiceLocator.hpp>
#include <server/HttpServer.hpp>
#include <server/Logger.hpp>
#include <controller/Controller.hpp>

class ServerConfiguration {
public:
  ServerConfiguration( const unsigned short port): port_{port} {
    registerServices();
  }

  template <typename Ctrl, typename ...RouteArgs>
  requires HasGet<Ctrl, RouteArgs...>
  void register_route(std::string path, HttpResponse (Ctrl::*method)(const std::string&, RouteArgs...) const) {
    router_.register_get(path, method);
  }
  void registerServices();
  void runServer();
  ~ServerConfiguration() = default;
private:
  ServiceLocator serviceLocator_ {};
  Router router_ {serviceLocator_};
  const unsigned short port_;
};

void ServerConfiguration::runServer() {
    unsigned int num_threads = std::thread::hardware_concurrency();
    net::io_context io_context(num_threads);

    auto work_guard = net::make_work_guard(io_context);

    tcp::endpoint endpoint{tcp::v4(), port_};
    auto server = std::make_shared<HttpServer>(io_context, endpoint, router_);
    server->run();

    std::vector<std::thread> thread_pool;

    for (unsigned int i = 0; i < num_threads; ++i) {
        thread_pool.emplace_back([&io_context] {
            io_context.run();
        });
    }

    for (auto& thread : thread_pool) {
        thread.join();
    }
}

void ServerConfiguration::registerServices() {
  serviceLocator_.registerService<ILogger, Logger>(Lifetime::Scoped);
  serviceLocator_.registerService<UserController>(Lifetime::Scoped);
  serviceLocator_.registerService<DataTablesController>(Lifetime::Scoped);
}
