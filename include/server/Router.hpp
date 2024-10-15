#pragma once
#include "controller/UserController.hpp"
#include "server/ServiceLocator.hpp"
#include <http/HttpResponse.hpp>
#include <unordered_map>
#include <functional>
#include <tuple>
#include <sstream>
#include <regex>
#include <iostream>
#include <controller/Controller.hpp>

class Router {
public:
    using RouteHandler = std::function<HttpResponse(const std::string& path, const std::string& body)>;

    Router(ServiceLocator &serviceLocator): serviceLocator_{serviceLocator} {}

    template <typename Ctrl, typename... RouteArgs>
    requires HasGet<Ctrl, RouteArgs...>
    void register_get(const std::string& path, HttpResponse (Ctrl::*method)(const std::string&, RouteArgs...) const)
    {
        auto route = Route<std::decay_t<RouteArgs>...>(path);
        routes["GET"].emplace_back([this, method, route](const std::string& request_path, const std::string& request_body) -> HttpResponse {
            auto controller = serviceLocator_.getService<Ctrl>();
            auto params = route.extract(request_path);
            return this->invoke(*controller, method, request_body, params);
        });
    }

    HttpResponse dispatch(const std::string& method, const std::string& path, const std::string& body) const {
        auto it = routes.find(method);
        if (it != routes.end()) {
            for (const auto& handler : it->second) {
                try {
                    return handler(path, body);
                } catch (const std::invalid_argument&) {
                    // Extraction failed; try the next handler
                    continue;
                } catch (const std::exception& e) {
                    // Handle other exceptions if necessary
                    continue;
                }
            }
        }
        // Return a default response or an error response if no route matches
        return HttpResponse(404, "Content-Type: text/plain", "404 Not Found");
    }

private:
    std::unordered_map<std::string, std::vector<RouteHandler>> routes;
    ServiceLocator &serviceLocator_;

    template <typename Ctrl, typename... RouteArgs>
    requires ControllerConcept<Ctrl, RouteArgs...>
    HttpResponse invoke(Ctrl& controller, HttpResponse (Ctrl::*method)(const std::string&, RouteArgs...) const, const std::string& body, const std::tuple<RouteArgs...>& params) {
        auto all_params = std::tuple_cat(std::make_tuple(body), params);
        return std::apply([&controller, method](const std::string& req, RouteArgs... args) -> HttpResponse {
            return (controller.*method)(req, args...);
        }, all_params);
    }
};

