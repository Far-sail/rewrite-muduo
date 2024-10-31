#pragma once 

#include <string>

#include "noncopyable.h"

// 定义日志级别 INFO ERROR FATAL DEBUG
enum LogLevel
{
    INFO, // 普通信息
    ERROR, // 错误信息
    FATAL, // core信息
    DEBUG, // 调试信息
};


// 输出一个日志类
class Logger : private noncopyable
{
public:
    // 获取唯一的实例对象
    static Logger& getInstance();
    //设置日志级别
    void setLogLevel(int level);
    // 写日志
    void log(std::string msg);
private:
    int logLevel_;
    Logger() = default;
};

// LOG_INFO("%s %d", arg1, arg2)
#define LOG_INFO(format, ...) \
    do \
    { \
        Logger& logger = Logger::getInstance(); \
        logger.setLogLevel(INFO); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, format, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0)

#define LOG_ERROR(format, ...) \
    do \
    { \
        Logger& logger = Logger::getInstance(); \
        logger.setLogLevel(ERROR); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, format, ##__VA_ARGS__); \
        logger.log(buf); \
    } while(0)

#define LOG_FATAL(format, ...) \
    do \
    { \
        Logger& logger = Logger::getInstance(); \
        logger.setLogLevel(FATAL); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, format, ##__VA_ARGS__); \
        logger.log(buf); \
        exit(-1); \
    } while(0)
    
#ifdef MUDEBUG
    #define LOG_DEBUG(format, ...) \
        do \
        { \
            Logger& logger = Logger::getInstance(); \
            logger.setLogLevel(DEBUG); \
            char buf[1024] = {0}; \
            snprintf(buf, 1024, format, ##__VA_ARGS__); \
            logger.log(buf); \
        } while(0)
#else
    #define LOG_DEBUG(format, ...)
#endif