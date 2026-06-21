// SystemFramework/Gateway/GatewayBase.h
#pragma once
#include <nlohmann/json.hpp>
#include <optional>
#include <iostream>

namespace SystemFramework::Gateway {
    class GatewayBase {
    protected:
        constexpr static const char* json_content_type = "application/json";

        template<typename T> std::string ToJson(T object) {
            try {
                nlohmann::json j = object;
                return j.dump();
            }
            catch (nlohmann::json::type_error& e) {
                std::cout << "Json Error: " << e.what() << std::endl;
                return "";
            }
        }

        template<typename T> std::optional<T> ToObject(nlohmann::json json) {
            try {
                T jObj = json.get<T>();
                return jObj;
            }
            catch (const nlohmann::json::exception& e) {
                std::cout << "Could Not Make Object: " << e.what() << std::endl;
                return std::nullopt;
            }
        }
    };
}