#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <algorithm>

// 日志级别枚举
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

// 获取当前时间字符串
std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    
    // 获取毫秒
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") 
        << "." << std::setfill('0') << std::setw(3) << milliseconds.count();
    
    return oss.str();
}

// 获取日志级别字符串
std::string getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

// 日志级别字符串转为日志等级
LogLevel getLevelFromString(std::string levelStr) {
    // 转换为小写
    std::transform(levelStr.begin(), levelStr.end(), levelStr.begin(), ::tolower);
    LogLevel level{LogLevel::INFO};
    if (levelStr == "info") 
    {
        level = LogLevel::INFO;
    }
    else if (levelStr == "debug") 
    {
        level = LogLevel::DEBUG;
    }
    else if (levelStr == "WARNING")
    {
        level = LogLevel::WARNING;
    }
    else if (levelStr == "ERROR")
    {
        level = LogLevel::ERROR;
    }
    return level;
}


// 日志输出函数
void logMessage(std::string fmt, LogLevel level, const std::string& file, int lineNumber, const std::string& message) {    
    // 替换占位符
    std::string output = fmt;
    
    // 替换时间
    size_t pos = output.find("{time}");
    if (pos != std::string::npos) {
        output.replace(pos, 6, getCurrentTime());
    }
    
    // 替换级别
    pos = output.find("{level}");
    if (pos != std::string::npos) {
        output.replace(pos, 7, getLevelString(level));
    }
    
    // 替换文件名
    pos = output.find("{file}");
    if (pos != std::string::npos) {
        output.replace(pos, 6, file);
    }
    
    // 替换行号
    pos = output.find("{lineNumber}");
    if (pos != std::string::npos) {
        output.replace(pos, 12, std::to_string(lineNumber));
    }
    
    // 替换消息
    pos = output.find("{msg}");
    if (pos != std::string::npos) {
        output.replace(pos, 5, message);
    }
    
    std::cout << output << std::endl;
}

// 使用宏简化日志调用
#define LOG(fmt, level, msg) logMessage(fmt, level, __FILE__, __LINE__, msg)

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "usage: " << argv[0] << " level " << "[fmt]" << std::endl;
        return 0;
    }
    LogLevel level{getLevelFromString(argv[1])};

    std::string fmt{"{time}-{level}-{file}:{lineNumber}-{msg}"};
    if (argc > 2) {
        fmt = argv[2];
    }

    switch (level)
    {
    case LogLevel::DEBUG:
        LOG(fmt, LogLevel::DEBUG, "Debug information");
        break;
    case LogLevel::INFO:
        LOG(fmt, LogLevel::INFO, "Application started");
        break;
    case LogLevel::WARNING:
        LOG(fmt, LogLevel::WARNING, "This is a warning");
        break;
    case LogLevel::ERROR:
        LOG(fmt, LogLevel::ERROR, "An error occurred");
        break;
    default:
        LOG(fmt, LogLevel::INFO, "Application started");
        break;
    }

    return 0;
}