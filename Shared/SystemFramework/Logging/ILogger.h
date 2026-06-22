#pragma once

#include <string>

namespace SystemFramework::Logging {
    class ILogger {
    public:
        virtual void Log(const std::string& message) = 0;
        virtual void LogError(const std::string& message) = 0;
        virtual void LogWarning(const std::string& message) = 0;
        virtual ~ILogger() = default;
        void SetServiceLevel(const std::string& level, const std::string& type) {
            _serviceType = level + "(" + type + ")";
        }
    protected:
        std::string _serviceType;
    };
}