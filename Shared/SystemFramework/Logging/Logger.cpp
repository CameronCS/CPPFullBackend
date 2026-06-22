#include "Logger.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <mutex>
#include <filesystem>

namespace LogCol = SystemFramework::Logging::Color;

static std::mutex s_mutex;
static std::ofstream s_logFile;
static std::string s_currentDate;

static std::string GetDate() {
	std::chrono::time_point now = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
	localtime_s(&tm, &time);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y-%m-%d");
	return oss.str();
}

static std::string GetTimestamp() {
	std::chrono::time_point now = std::chrono::system_clock::now();
	std::time_t time = std::chrono::system_clock::to_time_t(now);
	std::tm tm{};
	localtime_s(&tm, &time);
	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
	return oss.str();
}

static void EnsureLogFile() {
	std::string today = GetDate();
	if (today != s_currentDate) {
		if (s_logFile.is_open()) s_logFile.close();
		s_currentDate = today;
		std::filesystem::create_directories("logs");
		s_logFile.open("logs/" + today + ".log", std::ios::app);
	}
}

void SystemFramework::Logging::Logger::Log(const std::string& message) {
	std::lock_guard<std::mutex> lock(s_mutex);
	std::cout << LogCol::Green << "[Info]" << LogCol::Cyan << " <" << _serviceType << "> " << LogCol::Reset << message << "\n";
	EnsureLogFile();
	s_logFile << "[Info] [" << GetTimestamp() << "] <" << _serviceType << "> " << message << std::endl;
}

void SystemFramework::Logging::Logger::LogError(const std::string& message) {
	std::lock_guard<std::mutex> lock(s_mutex);
	std::cout << LogCol::Red << "[Fail]" << LogCol::Cyan << " <" << _serviceType << "> " << LogCol::Reset << message << "\n";
	EnsureLogFile();
	s_logFile << "[Fail] [" << GetTimestamp() << "] <" << _serviceType << "> " << message << std::endl;
}

void SystemFramework::Logging::Logger::LogWarning(const std::string& message) {
	std::lock_guard<std::mutex> lock(s_mutex);
	std::cout << LogCol::Yellow << "[Warning]" << LogCol::Cyan << " <" << _serviceType << "> " << LogCol::Reset << message << "\n";
	EnsureLogFile();
	s_logFile << "[Warning] [" << GetTimestamp() << "] <" << _serviceType << "> " << message << std::endl;
}