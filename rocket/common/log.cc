//
// Created by zyl on 2024/1/10.
//
#include <sys/time.h>
#include <sstream>
#include <stdio.h>
#include <assert.h>
#include <signal.h>

#include "log.h"
#include "util.h"

namespace rocket{
    static Logger* g_logger = NULL; // 单例
    Logger* Logger::GetGlobalLogger() {
        if(g_logger){
            return g_logger;
        }
        LogLevel global_log_level = StringToLogLevel(Config::GetGlobalConfig()->m_log_level); // 获取rocket.xml中的log_level标签值
        g_logger = new Logger(global_log_level);
        return g_logger;
    }

    std::string LogLevelToString(LogLevel level){
        switch (level) {
            case Debug:
                return "DEBUG";
            case Info:
                return "INFO";
            case Error:
                return "ERROR";
            default:
                return "UNKNOWN";
        }
    }
    LogLevel StringToLogLevel(const std::string& log_level){
        if(log_level == "DEBUG"){
            return Debug;
        }else if(log_level == "INFO"){
            return Info;
        }else if(log_level == "ERROR"){
            return Error;
        }else{
            return Unkonwn;
        }
    }

    void Logger::InitGlobalLogger(int type){
        LogLevel global_log_level = StringToLogLevel(Config::GetGlobalConfig()->m_log_level);
        printf("Init log level [%s]\n", LogLevelToString(global_log_level).c_str());
        g_logger = new Logger(global_log_level);
    }
    // void Logger::InitGlobalLogger(int type /*=1*/) {
    //     LogLevel global_log_level = StringToLogLevel(Config::GetGlobalConfig()->m_log_level);
    //     printf("Init log level [%s]\n", LogLevelToString(global_log_level).c_str());
    //     g_logger = new Logger(global_log_level, type);
    //     g_logger->init();
    // }

    // void Logger::init() {
    //     if (m_type == 0) {
    //         return;
    //     }
    //     // m_timer_event = std::make_shared<TimerEvent>(Config::GetGlobalConfig()->m_log_sync_inteval, true, std::bind(&Logger::syncLoop, this));
    //     // EventLoop::GetCurrentEventLoop()->addTimerEvent(m_timer_event);
    //     // signal(SIGSEGV, CoredumpHandler);
    //     // signal(SIGABRT, CoredumpHandler);
    //     // signal(SIGTERM, CoredumpHandler);
    //     // signal(SIGKILL, CoredumpHandler);
    //     // signal(SIGINT, CoredumpHandler);
    //     // signal(SIGSTKFLT, CoredumpHandler);
    // }

    

    std:: string LogEvent::toString(){
        struct timeval now_time;

        gettimeofday(&now_time, nullptr);

        struct tm now_time_t;
        localtime_r(&(now_time.tv_sec), &now_time_t);

        char buf[128];
        strftime(&buf[0], 128, "%y-%m-%d %H:%M:%S", &now_time_t);
        std::string time_str(buf);
        int ms = now_time.tv_usec / 1000;
        time_str = time_str + "." + std::to_string(ms);

        m_pid = getPid(); // 获取进程id
        m_thread_id = getThreadId(); // 获取线程id

        std::stringstream ss;

        ss << "[" << LogLevelToString(m_level) << "]\t"
        << "[" << time_str << "]\t"
        << m_pid << ":" << m_thread_id << "]\t";

        return ss.str();
    }

    void Logger::pushLog(const std::string &msg) {
        ScopeMutex<Mutex> lock(m_mutex); // 上锁
        m_buffer.push(msg);
        lock.unlock(); // 这步可以省略，因为析构中做了释放锁的操作
    }

    // Logger本质是单例的全局对象，多个线程同时操作log方法需要加锁
    void Logger::log() {
        ScopeMutex<Mutex> lock(m_mutex); // 上锁
        std::queue<std::string> tmp;
        m_buffer.swap(tmp); // 先拿出来，避免占用时间过长

        // 异步日志修改这里

        lock.unlock();
        while(!tmp.empty()){
            std::string msg = tmp.front();
            tmp.pop();
            printf("%s", msg.c_str());
        }
    }

    // void Logger::pushLog(const std::string &msg){
    //     m_buffer.push(msg);
    // }

    // void Logger::log(){
    //     while(!m_buffer.empty()){
    //         std::string msg = m_buffer.front();
    //         m_buffer.pop();

    //         printf("%s", msg.c_str());
    //     }
    // }
}