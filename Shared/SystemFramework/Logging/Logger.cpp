#include "Logger.h"

namespace LogCol = SystemFramework::Logging::Color;

void SystemFramework::Logging::Logger::Log(const std::string& message) {
	std::cout << LogCol::Green << "[Info]" << LogCol::Cyan << " <" << this->_serviceType << "> " << LogCol::Reset << message << "\n";
}

void SystemFramework::Logging::Logger::LogError(const std::string& message) {
	std::cout << LogCol::Red << "[Fail]" << LogCol::Cyan << " <" << this->_serviceType << "> " << LogCol::Reset << message << "\n";
}

void SystemFramework::Logging::Logger::LogWarning(const std::string& message) {
	std::cout << LogCol::Yellow << "[Warning]" << LogCol::Cyan << " <" << this->_serviceType << "> " << LogCol::Reset << message << "\n";
}