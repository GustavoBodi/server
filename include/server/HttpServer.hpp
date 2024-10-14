#pragma once
#include <http/HttpResponse.hpp>
#include <server/Router.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/detached.hpp>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

class HttpServer : public std::enable_shared_from_this<HttpServer> {
public:
    HttpServer(net::io_context& ioc, tcp::endpoint endpoint, Router router);
    void run();

private:
    Router router_;
    tcp::acceptor acceptor_;
    net::io_context& io_context_;
    net::signal_set signals_;

    void accept_connections();
    void configure_socket(tcp::socket& socket);
    net::awaitable<void> handle_session(beast::tcp_stream stream);
    net::awaitable<void> handle_request(http::request<http::string_body>& req, beast::tcp_stream& stream);
    void fail(beast::error_code ec, char const* what);

    template <typename Body>
    http::response<Body> to_boost_response(const HttpResponse& custom_response, int http_version) {
        http::response<Body> boost_response(static_cast<http::status>(custom_response.getStatusCode()), http_version);
        for (const auto& header : custom_response.getHeaders()) {
            boost_response.set(header.first, header.second);
        }
        boost_response.body() = custom_response.getBody();
        boost_response.prepare_payload();
        return boost_response;
    }
};
