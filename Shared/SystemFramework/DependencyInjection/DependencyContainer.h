#pragma once

#include <unordered_map>
#include <functional>
#include <typeindex>
#include "ScopedContainer.h"

namespace SystemFramework::DependencyInjection {
    class DependencyContainer {
    public:
        template<typename TInterface> void Register(std::function<void*(ScopedContainer&)> factory) {
            _factories[std::type_index(typeid(TInterface))] = factory;
        }

        ScopedContainer CreateScope() {
            return ScopedContainer(_factories);
        }

    private:
        std::unordered_map<std::type_index, std::function<void*(ScopedContainer&)>> _factories;
    };
}
