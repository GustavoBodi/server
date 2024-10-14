#pragma once
#include <concepts>
#include <domain/Entity.hpp>

template <typename T, typename Resp, typename Id>
concept GetEntityByIdConcept = IdConcept<Id> && requires(T a, Resp resp, Id id) {
  { a.Get(id) } -> std::same_as<Resp>;
};

template <typename T, typename Resp, typename Params>
concept GetEntityByParamsConcept = requires(T a, Resp resp, Params params) {
    { a.Get(params) } -> std::same_as<Resp>;
};

template <typename T, typename Resp, typename Entity, typename Id>
concept DeleteEntityConcept = IdConcept<Id> && requires(T a, Resp resp, Id id, Entity entity) {
    { a.Delete(id) } -> std::convertible_to<Resp>;
    { a.Delete(entity) } -> std::convertible_to<Resp>;
};

template <typename T, typename Entity>
concept UpdateEntityConcept = requires(T a, Entity entity) {
    { a.Update(entity) } -> std::same_as<bool>;
};

template<typename T, typename Resp, typename Params, typename Id>
concept GetEntity = GetEntityByIdConcept<T, Resp, Id> && GetEntityByParamsConcept<T, Resp, Params>;

template<typename T, typename Resp, typename Entity, typename Id>
concept ModifiableEntity = IdConcept<Id> && DeleteEntityConcept<T, Resp, Entity, Id> && UpdateEntityConcept<T, Entity>;
