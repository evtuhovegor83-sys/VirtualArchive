#include "Logger.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Logger* Logger::instance = nullptr;

Logger::Logger() {
    logFile.open("history.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Warning: Could not open history.log\n";
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

Logger* Logger::getInstance() {
    if (instance == nullptr) {
        instance = new Logger();
    }
    return instance;
}

void Logger::destroy() {
    delete instance;
    instance = nullptr;
}

std::string Logger::getCurrentTime() const {
    time_t t = time(nullptr);
    tm now;
    localtime_s(&now, &t);

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << now.tm_mday << "."
        << std::setw(2) << now.tm_mon + 1 << "."
        << now.tm_year + 1900 << " "
        << std::setw(2) << now.tm_hour << ":"
        << std::setw(2) << now.tm_min << ":"
        << std::setw(2) << now.tm_sec;
    return oss.str();
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
    case LogLevel::INFO: return "INFO";
    case LogLevel::WARNING: return "WARNING";
    case LogLevel::ERROR: return "ERROR";
    default: return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);

    std::string timeStr = getCurrentTime();
    std::string levelStr = levelToString(level);

    if (logFile.is_open()) {
        logFile << "[" << timeStr << "] [" << levelStr << "] " << message << "\n";
        logFile.flush();
    }
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}