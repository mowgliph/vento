//==============================================================================
// Logger.hpp
// Vento - Sistema de logging mejorado
//==============================================================================
#pragma once

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>

#include <functional>

namespace Vento {

enum class LogLevel {
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Fatal = 5
};

struct LogEntry {
    QDateTime timestamp;
    LogLevel level;
    QString message;
    QString category;
    QString file;
    int line;
};

class Logger : public QObject {
    Q_OBJECT
    
public:
    using LogHandler = std::function<void(const LogEntry&)>;
    
    static Logger& instance();
    
    void setLogLevel(LogLevel level);
    void setLogToFile(const QString& filePath);
    void setLogToConsole(bool enabled);
    void addHandler(LogHandler handler);
    
    void log(LogLevel level, const QString& message, 
             const QString& category = {},
             const char* file = nullptr, int line = 0);
    
    void trace(const QString& message, const QString& category = {});
    void debug(const QString& message, const QString& category = {});
    void info(const QString& message, const QString& category = {});
    void warning(const QString& message, const QString& category = {});
    void error(const QString& message, const QString& category = {});
    void fatal(const QString& message, const QString& category = {});
    
    LogLevel logLevel() const { return m_level; }
    
    static QString levelToString(LogLevel level);
    static QString levelToEmoji(LogLevel level);
    
signals:
    void logMessage(const QString& formattedMessage);
    
private:
    Logger();
    ~Logger() override;
    
    void writeToFile(const QString& message);
    void writeToConsole(const LogEntry& entry);
    QString formatEntry(const LogEntry& entry);
    
    LogLevel m_level = LogLevel::Info;
    bool m_consoleEnabled = true;
    
    QFile m_logFile;
    QTextStream m_fileStream;
    mutable QMutex m_mutex;
    
    QList<LogHandler> m_handlers;
};

#define VENTO_LOG(level, msg) \
    Vento::Logger::instance().log(level, msg, {}, __FILE__, __LINE__)

#define VENTO_TRACE(msg) VENTO_LOG(Vento::LogLevel::Trace, msg)
#define VENTO_DEBUG(msg) VENTO_LOG(Vento::LogLevel::Debug, msg)
#define VENTO_INFO(msg)  VENTO_LOG(Vento::LogLevel::Info, msg)
#define VENTO_WARN(msg)  VENTO_LOG(Vento::LogLevel::Warning, msg)
#define VENTO_ERROR(msg) VENTO_LOG(Vento::LogLevel::Error, msg)
#define VENTO_FATAL(msg) VENTO_LOG(Vento::LogLevel::Fatal, msg)

} // namespace Vento
