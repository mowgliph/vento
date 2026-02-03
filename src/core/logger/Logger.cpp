//==============================================================================
// Logger.cpp
// Vento - Implementación del sistema de logging
//==============================================================================

#include "Logger.hpp"
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <iostream>

namespace Vento {

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : QObject(nullptr) {}

Logger::~Logger() {
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
}

void Logger::setLogLevel(LogLevel level) {
    QMutexLocker locker(&m_mutex);
    m_level = level;
}

void Logger::setLogToFile(const QString& filePath) {
    QMutexLocker locker(&m_mutex);
    
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
    
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    m_logFile.setFileName(filePath);
    if (m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        m_fileStream.setDevice(&m_logFile);
        info(QString("Log file opened: %1").arg(filePath));
    } else {
        qWarning() << "Could not open log file:" << filePath;
    }
}

void Logger::setLogToConsole(bool enabled) {
    QMutexLocker locker(&m_mutex);
    m_consoleEnabled = enabled;
}

void Logger::addHandler(LogHandler handler) {
    QMutexLocker locker(&m_mutex);
    m_handlers.append(std::move(handler));
}

void Logger::log(LogLevel level, const QString& message, 
                 const QString& category, const char* file, int line) {
    if (level < m_level) {
        return;
    }
    
    QMutexLocker locker(&m_mutex);
    
    LogEntry entry{
        QDateTime::currentDateTime(),
        level,
        message,
        category,
        file ? QFileInfo(file).fileName() : QString(),
        line
    };
    
    QString formatted = formatEntry(entry);
    
    if (m_consoleEnabled) {
        writeToConsole(entry);
    }
    
    if (m_logFile.isOpen()) {
        writeToFile(formatted);
    }
    
    for (const auto& handler : m_handlers) {
        handler(entry);
    }
    
    emit logMessage(formatted);
}

QString Logger::formatEntry(const LogEntry& entry) {
    QString result = QString("[%1] [%2]")
        .arg(entry.timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz"))
        .arg(levelToString(entry.level).leftJustified(7));
    
    if (!entry.category.isEmpty()) {
        result += QString(" [%1]").arg(entry.category);
    }
    
    result += QString(" %1").arg(entry.message);
    
    if (!entry.file.isEmpty()) {
        result += QString(" (%1:%2)").arg(entry.file).arg(entry.line);
    }
    
    return result;
}

void Logger::writeToFile(const QString& message) {
    m_fileStream << message << "\n";
    m_fileStream.flush();
}

void Logger::writeToConsole(const LogEntry& entry) {
    QString emoji = levelToEmoji(entry.level);
    QString output = QString("%1 %2").arg(emoji, entry.message);
    
    switch (entry.level) {
        case LogLevel::Trace:
        case LogLevel::Debug:
            qDebug().noquote() << output;
            break;
        case LogLevel::Info:
            qInfo().noquote() << output;
            break;
        case LogLevel::Warning:
            qWarning().noquote() << output;
            break;
        case LogLevel::Error:
        case LogLevel::Fatal:
            qCritical().noquote() << output;
            break;
    }
}

QString Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Trace:   return "TRACE";
        case LogLevel::Debug:   return "DEBUG";
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARN";
        case LogLevel::Error:   return "ERROR";
        case LogLevel::Fatal:   return "FATAL";
        default:                return "UNKNOWN";
    }
}

QString Logger::levelToEmoji(LogLevel level) {
    switch (level) {
        case LogLevel::Trace:   return "🔍";
        case LogLevel::Debug:   return "🐛";
        case LogLevel::Info:    return "ℹ️ ";
        case LogLevel::Warning: return "⚠️ ";
        case LogLevel::Error:   return "❌";
        case LogLevel::Fatal:   return "💀";
        default:                return "  ";
    }
}

void Logger::trace(const QString& message, const QString& category) {
    log(LogLevel::Trace, message, category);
}

void Logger::debug(const QString& message, const QString& category) {
    log(LogLevel::Debug, message, category);
}

void Logger::info(const QString& message, const QString& category) {
    log(LogLevel::Info, message, category);
}

void Logger::warning(const QString& message, const QString& category) {
    log(LogLevel::Warning, message, category);
}

void Logger::error(const QString& message, const QString& category) {
    log(LogLevel::Error, message, category);
}

void Logger::fatal(const QString& message, const QString& category) {
    log(LogLevel::Fatal, message, category);
}

} // namespace Vento
