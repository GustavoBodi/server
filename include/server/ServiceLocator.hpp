#pragma once
#include <memory>
#include <typeindex>
#include <set>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <tuple>
#include <utility>
#include <stdexcept>

enum class Lifetime {
    Singleton,
    Transient,
    Scoped
};

class ServiceLocator {
public:
    // Register a service with its concept and implementation
    template<typename Concept, typename Implementation>
    void registerService(Lifetime lifetime) {
        static_assert(std::derived_from<Implementation, Concept>, "Implementation must derive from Concept.");
        auto factory = [this]() -> std::shared_ptr<void> {
            // Create Implementation by resolving its dependencies
            if constexpr (requires { typename Implementation::dependencies; }) {
                auto dependencies = resolveDependencies<typename Implementation::dependencies>();
                return std::apply(
                    [](auto&&... args) { return std::make_shared<Implementation>(std::forward<decltype(args)>(args)...); },
                    dependencies
                );
            } else {
                return std::make_shared<Implementation>();
            }
        };
        services_[typeid(Concept)] = { nullptr, lifetime, factory };
    }

    // Register a concrete implementation without a concept
    template <typename Implementation>
    void registerService(Lifetime lifetime) {
        auto factory = [this]() -> std::shared_ptr<void> {
            // Create Implementation by resolving its dependencies
            if constexpr (requires { typename Implementation::dependencies; }) {
                auto dependencies = resolveDependencies<typename Implementation::dependencies>();
                return std::apply(
                    [](auto&&... args) { return std::make_shared<Implementation>(std::forward<decltype(args)>(args)...); },
                    dependencies
                );
            } else {
                return std::make_shared<Implementation>();
            }
        };
        services_[typeid(Implementation)] = { nullptr, lifetime, factory };
    }

    // Retrieve a service by its concept
    template<typename Concept>
    std::shared_ptr<Concept> getService() {
        auto it = services_.find(typeid(Concept));
        if (it != services_.end()) {
            auto& [service, lifetime, factory] = it->second;

            switch (lifetime) {
                case Lifetime::Singleton:
                    if (!service) {
                        service = factory();
                    }
                    return std::static_pointer_cast<Concept>(service);
                case Lifetime::Transient:
                    return std::static_pointer_cast<Concept>(factory());
                case Lifetime::Scoped:
                    return getScopedService<Concept>(factory);
                default:
                    throw std::runtime_error("Unknown lifetime type");
            }
        } else {
            throw std::runtime_error("Service not registered for the requested concept");
        }
    }

private:
    struct ServiceEntry {
        std::shared_ptr<void> service;
        Lifetime lifetime;
        std::function<std::shared_ptr<void>()> factory;
    };

    std::unordered_map<std::type_index, ServiceEntry> services_;
    std::unordered_map<std::type_index, std::shared_ptr<void>> scopedServices_;
    std::set<std::type_index> creatingServices_;

    template<typename Concept>
    std::shared_ptr<Concept> getScopedService(const std::function<std::shared_ptr<void>()>& factory) {
        auto it = scopedServices_.find(typeid(Concept));
        if (it == scopedServices_.end()) {
            auto newService = std::static_pointer_cast<Concept>(factory());
            scopedServices_[typeid(Concept)] = newService;
            return newService;
        }
        return std::static_pointer_cast<Concept>(it->second);
    }

    // Helper to unpack dependencies tuple and retrieve each dependency
    template <typename Tuple, std::size_t... I>
    auto resolveDependenciesImpl(std::index_sequence<I...>) {
        return std::make_tuple(getService<std::tuple_element_t<I, Tuple>>()...);
    }

    template <typename Tuple>
    auto resolveDependencies() {
        return resolveDependenciesImpl<Tuple>(std::make_index_sequence<std::tuple_size_v<Tuple>>{});
    }

    // Create a service by resolving its dependencies
    template <typename T>
    std::shared_ptr<T> createService() {
        if (creatingServices_.count(typeid(T))) {
            throw std::runtime_error("Circular dependency detected for type " + std::string(typeid(T).name()));
        }

        creatingServices_.insert(typeid(T));

        auto it = services_.find(typeid(T));
        if (it != services_.end()) {
            auto& [service, lifetime, factory] = it->second;
            if (factory) {
                // Use the factory to create the instance
                auto instance = std::static_pointer_cast<T>(factory());
                creatingServices_.erase(typeid(T));
                return instance;
            } else {
                // If factory is not present, assume concrete registration without dependencies
                std::shared_ptr<T> instance = std::make_shared<T>();
                creatingServices_.erase(typeid(T));
                return instance;
            }
        }

        // If T is not registered, throw an error
        throw std::runtime_error("Service not registered for type " + std::string(typeid(T).name()));
    }
};
