//
// Created by zyl on 2024/1/9.
//

#ifndef RPC_LOG_H
#define RPC_LOG_H

#include <string>
#include <queue>
#include <memory>
#include <sys/time.h>

#include "config.h"
#include "mutex.h"

namespace rocket{

template<typename... Args>
std::string formatString(const char* str, Args&&... args){
    int size = snprintf(nullptr, 0, str, args...);

    std::string result;
    if(size > 0){
        result.resize(size);
        snprintf(&result[0], size+1, str, args...);
    }
    return result;
}

  

#define DEBUGLOG(str, ...) \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() && rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Debug) \
    { \
        rocket::Logger::GetGlobalLogger()->pushLog(rocket::LogEvent(rocket::LogLevel::Debug).toString() \
        + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n");\
        rocket::Logger::GetGlobalLogger()->log(); \
    } \

#define INFOLOG(str, ...) \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Info) \
    { \
        rocket::Logger::GetGlobalLogger()->pushLog(rocket::LogEvent(rocket::LogLevel::Info).toString() \
        + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n");\
        rocket::Logger::GetGlobalLogger()->log(); \
    } \

#define ERRORLOG(str, ...) \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Error) \
    { \
        rocket::Logger::GetGlobalLogger()->pushLog(rocket::LogEvent(rocket::LogLevel::Error).toString() \
        + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n");\
        rocket::Logger::GetGlobalLogger()->log(); \
    } \

#define APPDEBUGLOG(str, ...) \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Debug) \
    { \
        rocket::Logger::GetGlobalLogger()->pushAppLog(rocket::LogEvent(rocket::LogLevel::Debug).toString() \
        + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n");\
        rocket::Logger::GetGlobalLogger()->log(); \
    } \

#define APPINFOLOG(str, ...) \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Info) \
    { \
    rocket::Logger::GetGlobalLogger()->pushAppLog(rocket::LogEvent(rocket::LogLevel::Info).toString() \
    + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n");\
    rocket::Logger::GetGlobalLogger()->log(); \
    } \

#define APPERRORLOG(str, ...) \
    if (rocket::Logger::GetGlobalLogger()->getLogLevel() <= rocket::Error) \
    { \
        rocket::Logger::GetGlobalLogger()->pushAppLog(rocket::LogEvent(rocket::LogLevel::Error).toString() \
        + "[" + std::string(__FILE__) + ":" + std::to_string(__LINE__) + "]\t" + rocket::formatString(str, ##__VA_ARGS__) + "\n");\
        rocket::Logger::GetGlobalLogger()->log(); \
    } \



// 这是一个条件语句，用于检查全局日志记录器的日志级别是否允许记录调试信息。如果允许，则执行下面的代码块。
// 这个宏定义的目的是在满足一定条件时，将调试信息添加到日志中。在条件中，首先检查全局日志记录器的日志级别，
// 确保允许记录调试信息。如果满足条件，就构建一个包含调试信息的日志消息，并将其推送到全局日志记录器。
// 这个宏定义的好处是，它只在调试日志被启用时才会产生额外的运行时开销。
// rocket::Debug是枚举中的


enum LogLevel{
    Unkonwn = 0,
    Debug = 1,
    Info = 2,
    Error = 3
};

// 日志器，打印日志
class Logger{
public:
    typedef std:: shared_ptr<Logger> s_ptr;

    Logger(LogLevel level) : m_set_level(level){}
    // Logger(LogLevel level, int tpye = 1) : m_set_level(level) {}

    void pushLog(const std::string& msg); // 把msg放入m_buffer中

    // void init();

    void log();

    LogLevel getLogLevel() const {
        return m_set_level;
    }

public:
    // 这是一个静态成员函数，通常用于获取或创建全局唯一的日志记录器实例。
    // 该函数实现了单例模式，确保在整个程序中只有一个日志记录器实例。
    static Logger* GetGlobalLogger();
    static void InitGlobalLogger(int type = 1);

private:
    LogLevel m_set_level; // 日志级别
    std::queue<std::string> m_buffer; // 日志内容
    int m_type = 0;
    Mutex m_mutex;
};

std::string LogLevelToString(LogLevel level);
LogLevel StringToLogLevel(const std::string& log_level);

class LogEvent{
public:
    LogEvent(LogLevel level) : m_level(level) {}

    std::string getFileName() const {
        return m_file_name;
    }

    LogLevel getLogLevel() const {
        return m_level;
    }

    std::string toString();
private:
    std::string m_file_name; // 文件名
    int32_t m_file_line; // 行号
    int32_t m_pid; // 进程号
    int32_t m_thread_id; // 线程号

    LogLevel m_level; // 日志级别

};

}

#endif //RPC_LOG_H
