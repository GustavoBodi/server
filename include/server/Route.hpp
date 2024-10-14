#pragma once
#include <http/HttpResponse.hpp>
#include <tuple>
#include <regex>
#include <controller/Controller.hpp>

template <typename... Args>
class Route {
public:
    Route(const std::string& path) : path_(path) {
        // Remove trailing slash for consistency
        std::string modified_path = path;
        if (!modified_path.empty() && modified_path.back() == '/') {
            modified_path.pop_back();
        }

        // Replace {param} with regex capture groups
        std::string pattern = std::regex_replace(modified_path, std::regex("\\{\\w+\\}"), "([\\w-]+)");

        // Make trailing slash optional
        pattern += "/?";

        // Compile the final regex pattern
        regex_pattern_ = std::regex("^" + pattern + "$");
    }

    std::tuple<Args...> extract(const std::string& request) const {
        std::smatch matches;
        if (std::regex_match(request, matches, regex_pattern_)) {
            return extract_impl(matches, std::index_sequence_for<Args...>{});
        }
        throw std::invalid_argument("Request does not match route pattern for path: " + path_);
    }

private:
    std::string path_;
    std::regex regex_pattern_;

    // Helper function to extract parameters using index sequence
    template <std::size_t... Is>
    std::tuple<Args...> extract_impl(const std::smatch& matches, std::index_sequence<Is...>) const {
        return std::make_tuple(convert<Args>(matches[Is + 1].str())...);
    }

    // Convert a string to the desired type
    template <typename T>
    T convert(const std::string& str) const {
        if constexpr (std::is_same_v<T, std::string>) {
            return str;
        } else if constexpr (std::is_integral_v<T>) {
            return static_cast<T>(std::stoul(str));
        } else {
            static_assert(always_false<T>::value, "Unsupported type for conversion.");
        }
    }

    // Helper template to trigger static_assert for unsupported types
    template <typename>
    struct always_false : std::false_type {};
};

