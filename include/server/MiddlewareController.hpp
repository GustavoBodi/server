#include <functional>
#include <unordered_map>
#include <server/Middleware.hpp>

template <Middleware ...Args>
class MiddlewareController {
public:
  MiddlewareController(Args ...args) {
    std::index_sequence<> index = std::make_index_sequence<sizeof...(Args)>{};
    middlewares_ = tup(args...);
  }

  template <typename ...Types>
  std::tuple<Types...> tup (Types&&... refs) {
    return std::tuple<Types...>{ std::forward<Types>(refs)... };
  }

  void apply(HttpRequest& request) {
    for (std::size_t i = 0; i < registered_types_; ++i) {
      middlewares_[i](request);
    }
  }

  ~MiddlewareController() = default;
private:
  static constexpr std::size_t registered_types_ = std::tuple_size<std::tuple<Args...>>::value;
  static constexpr std::array<Args..., registered_types_> middlewares_;
};

