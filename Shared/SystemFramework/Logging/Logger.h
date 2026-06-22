#pragma once

#include <string>
#include <iostream>
#include "ILogger.h"
#include "LoggerColours.h"

namespace SystemFramework::Logging {
    class Logger : public SystemFramework::Logging::ILogger {
    public:
        void Log(const std::string& message);
        void LogError(const std::string& message);
        void LogWarning(const std::string& message);
        ~Logger() = default;
    };
}