#include "Logger.h"

Logger::Level Logger::currentLogLevel = Logger::Debug;

void Logger::setLogLevel(Level level) {
    currentLogLevel = level;
}

void Logger::debug(const QString& message) {
    log(Debug, message);
}

void Logger::info(const QString& message) {
    log(Info, message);
}

void Logger::warning(const QString& message) {
    log(Warning, message);
}

void Logger::error(const QString& message) {
    log(Error, message);
}

void Logger::log(Level level, const QString& message) {
    if (level < currentLogLevel) {
        return;
    }
    
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    QString logMessage = QString("[%1] [%2] %3").arg(timestamp, levelStr, message);
    
    switch (level) {
        case Debug:
            qDebug().noquote() << logMessage;
            break;
        case Info:
            qInfo().noquote() << logMessage;
            break;
        case Warning:
            qWarning().noquote() << logMessage;
            break;
        case Error:
            qCritical().noquote() << logMessage;
            break;
    }
}

QString Logger::levelToString(Level level) {
    switch (level) {
        case Debug: return "DEBUG";
        case Info: return "INFO";
        case Warning: return "WARN";
        case Error: return "ERROR";
        default: return "UNKNOWN";
    }
}