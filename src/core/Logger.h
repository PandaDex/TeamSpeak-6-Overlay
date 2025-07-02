#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <QDebug>
#include <QDateTime>

class Logger {
public:
    enum Level {
        Debug = 0,
        Info = 1,
        Warning = 2,
        Error = 3
    };
    
    static void setLogLevel(Level level);
    static void debug(const QString& message);
    static void info(const QString& message);
    static void warning(const QString& message);
    static void error(const QString& message);
    
private:
    static void log(Level level, const QString& message);
    static Level currentLogLevel;
    static QString levelToString(Level level);
};

#define LOG_DEBUG(msg) Logger::debug(msg)
#define LOG_INFO(msg) Logger::info(msg)
#define LOG_WARNING(msg) Logger::warning(msg)
#define LOG_ERROR(msg) Logger::error(msg)

#endif // LOGGER_H