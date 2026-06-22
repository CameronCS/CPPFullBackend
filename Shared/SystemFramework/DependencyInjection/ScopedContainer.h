#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>
#include <stdexcept>

namespace SystemFramework::DependencyInjection {
    class ScopedContainer {
    public:
        ScopedContainer(std::unordered_map<std::type_index, std::function<void*(ScopedContainer&)>>& factories)
            : _factories(factories) {}

        ScopedContainer(const ScopedContainer&) = delete;
        ScopedContainer& operator=(const ScopedContainer&) = delete;

        template<typename TInterface> TInterface* Resolve() {
            std::type_index key = std::type_index(typeid(TInterface));

            std::unordered_map<std::type_index, std::pair<void*, std::function<void(void*)>>>::iterator it = _instances.find(key);
            if (it != _instances.end()) {
                return static_cast<TInterface*>(it->second.first);
            }

            std::unordered_map<std::type_index, std::function<void*(ScopedContainer&)>>::iterator factoryIt = _factories.find(key);
            if (factoryIt == _factories.end()) {
                throw std::runtime_error("Type not registered in container");
            }

            void* instance = factoryIt->second(*this);
            _instances[key] = { instance, [](void* p) { delete static_cast<TInterface*>(p); } };
            return static_cast<TInterface*>(instance);
        }

        ~ScopedContainer() {
            for (std::pair<const std::type_index, std::pair<void*, std::function<void(void*)>>>& entry : _instances) {
                entry.second.second(entry.second.first);
            }
        }

    private:
        std::unordered_map<std::type_index, std::function<void*(ScopedContainer&)>>& _factories;
        std::unordered_map<std::type_index, std::pair<void*, std::function<void(void*)>>> _instances;
    };
}
