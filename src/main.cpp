#include "controller/DataTablesController.hpp"
#include "controller/UserController.hpp"
#include <iostream>
#include <thread>
#include <server/Server.hpp>
#include <server/Router.hpp>
#include <server/ServiceLocator.hpp>
#include <server/Middleware.hpp>
#include <infrastructure/infrastructure.hpp>
#include <domain/Entity.hpp>

// Register Route
// Register Services and Controllers on DI
// Register Commmand and Queries on Mediatr
int main() {
    Server server (8080);
    server.register_route<UserController, HttpVerbs::Get, std::string>("/user/:id", &UserController::Get);
    server.register_route<DataTablesController, HttpVerbs::Get, std::string>("/company/:id/dre", &DREController::Get);
    server.serviceLocator().registerService<ILogger, Logger>(Lifetime::Scoped);
    server.serviceLocator().registerService<UserController>(Lifetime::Scoped);
    server.serviceLocator().registerService<DataTablesController>(Lifetime::Scoped);
    server.run();
    return 0;
}

