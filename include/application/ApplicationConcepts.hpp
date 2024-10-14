#pragma once
#include <concepts>
#include <optional>

template <typename Handler, typename Command, typename Response>
concept CommandHandler = requires(Handler handler, Command command) {
  { handler.handle(command) } -> std::same_as<std::optional<Response>>;
};

template <typename Handler, typename Query, typename Response>
concept QueryHandler = requires(Handler handler, Query query) {
  { handler.handle(query) } -> std::same_as<std::optional<Response>>;
};
