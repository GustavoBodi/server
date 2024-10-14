#include <server/HttpServer.hpp>

HttpServer::HttpServer(net::io_context& ioc, tcp::endpoint endpoint, Router router)
   : router_(router), acceptor_(net::make_strand(ioc)), io_context_(ioc),
     signals_(ioc, SIGINT, SIGTERM) {
   beast::error_code ec;

   acceptor_.open(endpoint.protocol(), ec);
   if (ec) fail(ec, "open");

   acceptor_.set_option(net::socket_base::reuse_address(true), ec);
   if (ec) fail(ec, "set_option");

   acceptor_.bind(endpoint, ec);
   if (ec) fail(ec, "bind");

   acceptor_.listen(net::socket_base::max_listen_connections, ec);
   if (ec) fail(ec, "listen");

   std::cout << "Server is listening on " << endpoint.port() << "\n";

   signals_.async_wait([this](beast::error_code ec, int signal_number) {
       if (!ec) {
           std::cout << "Signal received: " << signal_number << ", stopping io_context.\n";
           io_context_.stop();  // Clean shutdown on signal
       } else if (ec == net::error::operation_aborted) {
           std::cerr << "Signal handler operation canceled.\n";
       } else {
           std::cerr << "Error receiving signal: " << ec.message() << std::endl;
       }
   });
}

void HttpServer::run() {
    accept_connections();
}

void HttpServer::accept_connections() {
    acceptor_.async_accept(
        net::make_strand(io_context_),
        [self = shared_from_this()](beast::error_code ec, tcp::socket socket) {
            if (!ec) {
                self->configure_socket(socket);

                // Spawn a new session
                net::co_spawn(
                    socket.get_executor(),
                    self->handle_session(beast::tcp_stream(std::move(socket))),
                    net::detached);
            } else {
                self->fail(ec, "accept");
            }

            // Accept the next connection
            self->accept_connections();
        });
}

void HttpServer::configure_socket(tcp::socket& socket) {
    // Disable Nagle's algorithm for low-latency communication
    boost::system::error_code ec;
    socket.set_option(tcp::no_delay(true), ec);
    if (ec) {
        std::cerr << "Failed to set TCP_NODELAY: " << ec.message() << "\n";
    }

    // Optionally, set socket buffer sizes (use appropriate values)
    socket.set_option(boost::asio::socket_base::send_buffer_size(8192), ec);
    socket.set_option(boost::asio::socket_base::receive_buffer_size(8192), ec);
}

net::awaitable<void> HttpServer::handle_session(beast::tcp_stream stream) {
    beast::flat_static_buffer<8192> buffer;

    try {
        for (;;) {
            http::request<http::string_body> req;
            beast::error_code ec;

            // Read the request
            co_await http::async_read(stream, buffer, req,
                                      net::redirect_error(net::use_awaitable, ec));

            if (ec == http::error::end_of_stream) {
                // Client closed the connection gracefully
                break;
            } else if (ec) {
                // An unexpected error occurred
                throw boost::system::system_error{ec, "shutdown error"};
            }

            // Handle the request
            co_await handle_request(req, stream);

            // If the connection should not be kept alive, break the loop
            if (!req.keep_alive()) break;

            // Clear the buffer for the next request
            buffer.consume(buffer.size());
        }

        // Gracefully close the TCP connection
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_send, ec);
        if (ec && ec != net::error::not_connected) {
            throw boost::system::system_error{ec, "shutdown error"};
        }
    } catch (const beast::system_error& se) {
        if (se.code() != beast::websocket::error::closed &&
            se.code() != net::error::operation_aborted &&
            se.code() != net::error::eof &&
            se.code() != http::error::end_of_stream) {
            std::cerr << "Error: " << se.code().message() << "\n";
        }
        // Handle end_of_stream gracefully, no need to print error
    }
}

net::awaitable<void> HttpServer::handle_request(http::request<http::string_body>& req, beast::tcp_stream& stream) {
    std::string method = req.method_string();
    std::string target = req.target();
    std::string body = req.body();
    HttpResponse response = router_.dispatch(method, target, body);
    http::response<http::string_body> boost_response = to_boost_response<http::string_body>(response, req.version());
    co_await http::async_write(stream, boost_response, net::use_awaitable);
}

void HttpServer::fail(beast::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}
