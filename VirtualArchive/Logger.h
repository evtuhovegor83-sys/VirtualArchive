#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <ctime>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

class Logger {
private:
    static Logger* instance;
    std::ofstream logFile;
    std::mutex mtx;

    Logger();
    ~Logger();

    std::string getCurrentTime() const;
    std::string levelToString(LogLevel level) const;

public:
    static Logger* getInstance();
    static void destroy();

    void log(LogLevel level, const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};