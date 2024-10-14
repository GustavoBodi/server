#include <uWebSockets/App.h>

class Server {
  Server() {}
  void run() {
    uWS::App()
        .get("/*", [](auto *res, auto *req) {
            (void) req;
            // Set the content type for the response
            res->writeHeader("Content-Type", "text/html; charset=utf-8");

            // Write the response body
            res->end("Hello, World!");
        })
        .listen(3000, [](auto *token) {
            if (token) {
                std::cout << "Server started on port 3000" << std::endl;
            } else {
                std::cout << "Failed to start server" << std::endl;
            }
        })
        .run();
  }
  ~Server() {}
};
