#include "controller/DataTablesController.hpp"
#include "controller/UserController.hpp"
#include "server/ServerConfiguration.hpp"
#include <iostream>
#include <thread>
#include <server/Server.hpp>
#include <server/HttpServer.hpp>
#include <server/Router.hpp>
#include <server/ServiceLocator.hpp>
#include <infrastructure/infrastructure.hpp>
#include <domain/Entity.hpp>

// Register Route
// Register Services and Controllers on DI
// Register Commmand and Queries on Mediatr
int main() {
    ServerConfiguration serverConfig (8080);
    serverConfig.register_route<UserController>("/user/{id}", &UserController::Get);
    serverConfig.register_route<DataTablesController>("/company/{id}/dre", &DREController::Get);
    serverConfig.runServer();

    return 0;
}

