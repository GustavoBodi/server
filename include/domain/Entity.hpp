#pragma once
#include <concepts>
#include <compare>

template <typename T>
concept IdConcept = requires(T a) {
    { a.getId() } -> std::same_as<int>;
};

template <typename T, typename Id>
concept EntityConcept = requires(T a) {
    { a.entityId() } -> std::same_as<Id>;
};

template <typename T, typename Id>
concept ValueObjectConcept = std::copy_constructible<T> && std::equality_comparable<T> && !EntityConcept<T, Id>;