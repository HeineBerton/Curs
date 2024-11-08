#include "Logger.h"

Logger::Logger(const std::string& filename) : log_file(filename, std::ios::app) {
    if (!log_file.is_open()) {
        handleError("Не удалось открыть файл журнала: " + filename);
    }
}

Logger::~Logger() {
    if (log_file.is_open()) {
        log_file.close();
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (log_file.is_open()) {
        log_file << currentDateTime() << " [" << logLevelToString(level) << "] " << message << std::endl;
        std::cout << currentDateTime() << " [" << logLevelToString(level) << "] " << message << std::endl;
        if (level == CRITICAL) {
            std::cerr << "Критическая ошибка: " << message << std::endl;
        }
    } else {
        handleError("Файл журнала не открыт, сообщение не может быть записано: " + message);
    }
}

void Logger::handleError(const std::string& error_message) {
    std::cerr << error_message << std::endl;
    throw std::runtime_error(error_message);
}

std::string Logger::currentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    localtime_r(&now, &tstruct);

    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
    return buf;
}

std::string Logger::logLevelToString(LogLevel level) {
    switch (level) {
        case INFO: return "INFO";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        case CRITICAL: return "CRITICAL";
        default: return "UNKNOWN";
    }
}
