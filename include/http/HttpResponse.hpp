#pragma once
#include <string>
#include <unordered_map>

class HttpResponse {
public:
    HttpResponse(int code, std::string message)
        : status_code(code), status_message(std::move(message)) {}
    
    HttpResponse(int code, std::string message, std::string body)
      : status_code(code), status_message(message), body(body) {}

    void set_header(const std::string& name, const std::string& value) {
        headers[name] = value;
    }

    HttpResponse set_body(const std::string& content) {
        body = content;
        set_header("Content-Length", std::to_string(content.size()));
        return *this;
    }

    std::unordered_map<std::string, std::string> getHeaders() const {
      return headers;
    }

    int getStatusCode() const {
      return status_code;
    }

    std::string getBody() const {
      return body;
    }


    std::string to_string() const {
        std::string response = http_version + " " + std::to_string(status_code) + " " + status_message + "\r\n";
        for (const auto& header : headers) {
            response += header.first + ": " + header.second + "\r\n";
        }
        response += "\r\n" + body;
        return response;
    }

    void setKeepAlive(bool alive) {
      keep_alive_ = alive;
    }

private:
    int status_code = 200;
    std::string status_message;
    std::string http_version = "HTTP/1.1";
    std::unordered_map<std::string, std::string> headers;
    std::string body;
    bool keep_alive_ = true;
};
