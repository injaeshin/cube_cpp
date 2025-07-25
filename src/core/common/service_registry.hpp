#ifndef SERVICE_REGISTRY_HPP
#define SERVICE_REGISTRY_HPP

#include "core/core_pch.hpp"

namespace core {

class ServiceRegistry {
public:
    template <typename T>
    void Registry(std::shared_ptr<T> service) {
        _services[typeid(T)] = service;
    }

    template <typename T>
    std::shared_ptr<T> Get() const {
        const auto it = _services.find(typeid(T));
        if (it == _services.end()) return nullptr;
        return std::static_pointer_cast<T>(it->second);
    }

private:
    std::unordered_map<std::type_index, std::shared_ptr<void>> _services;
};

}
#endif  // SERVICE_REGISTRY_HPP
