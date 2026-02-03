//==============================================================================
// FileHandler.cpp
// Vento - RAII File Management Utility Implementation
//==============================================================================

#include "FileHandler.hpp"
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

namespace Vento::Utils {

//==============================================================================
// Constructor
//==============================================================================
FileHandler::FileHandler(const QString& filePath, QIODevice::OpenMode mode)
    : m_filePath(filePath)
    , m_mode(mode)
{
    m_file = std::make_unique<QFile>(m_filePath);
    
    // Asegurar que el directorio exista
    ensureDirectoryExists(m_filePath);
    
    // Intentar abrir el archivo
    m_isOpen = m_file->open(m_mode);
    
    if (m_isOpen && (m_mode & QIODevice::Text)) {
        m_textStream = std::make_unique<QTextStream>(m_file.get());
        m_textStream->setEncoding(QStringConverter::Utf8);
    }
}

//==============================================================================
// Destructor - RAII: cierra el archivo automáticamente
//==============================================================================
FileHandler::~FileHandler() {
    close();
}

//==============================================================================
// Move Constructor
//==============================================================================
FileHandler::FileHandler(FileHandler&& other) noexcept
    : m_file(std::move(other.m_file))
    , m_textStream(std::move(other.m_textStream))
    , m_filePath(std::move(other.m_filePath))
    , m_mode(other.m_mode)
    , m_isOpen(other.m_isOpen)
{
    other.reset();
}

//==============================================================================
// Move Assignment
//==============================================================================
FileHandler& FileHandler::operator=(FileHandler&& other) noexcept {
    if (this != &other) {
        close();
        
        m_file = std::move(other.m_file);
        m_textStream = std::move(other.m_textStream);
        m_filePath = std::move(other.m_filePath);
        m_mode = other.m_mode;
        m_isOpen = other.m_isOpen;
        
        other.reset();
    }
    return *this;
}

//==============================================================================
// Estado del archivo
//==============================================================================
bool FileHandler::isOpen() const noexcept {
    return m_isOpen && m_file && m_file->isOpen();
}

bool FileHandler::exists() const noexcept {
    return m_file && m_file->exists();
}

QString FileHandler::errorString() const noexcept {
    return m_file ? m_file->errorString() : QString("Archivo no inicializado");
}

//==============================================================================
// Operaciones de escritura
//==============================================================================
bool FileHandler::writeLine(const QString& line) {
    if (!isOpen()) {
        return false;
    }
    
    if (m_textStream) {
        *m_textStream << line << Qt::endl;
        return m_textStream->status() == QTextStream::Ok;
    } else {
        QByteArray data = (line + "\n").toUtf8();
        return m_file->write(data) == data.size();
    }
}

bool FileHandler::writeText(const QString& text) {
    if (!isOpen()) {
        return false;
    }
    
    if (m_textStream) {
        *m_textStream << text;
        return m_textStream->status() == QTextStream::Ok;
    } else {
        QByteArray data = text.toUtf8();
        return m_file->write(data) == data.size();
    }
}

bool FileHandler::writeRawData(const QByteArray& data) {
    if (!isOpen()) {
        return false;
    }
    
    return m_file->write(data) == data.size();
}

//==============================================================================
// Operaciones de lectura
//==============================================================================
std::optional<QString> FileHandler::readLine() {
    if (!isOpen()) {
        return std::nullopt;
    }
    
    if (m_textStream) {
        if (!m_textStream->atEnd()) {
            QString line = m_textStream->readLine();
            if (m_textStream->status() == QTextStream::Ok) {
                return line;
            }
        }
    } else {
        QByteArray data = m_file->readLine();
        if (!data.isEmpty()) {
            return QString::fromUtf8(data);
        }
    }
    
    return std::nullopt;
}

std::optional<QString> FileHandler::readAll() {
    if (!isOpen()) {
        return std::nullopt;
    }
    
    if (m_textStream) {
        QString content = m_textStream->readAll();
        if (m_textStream->status() == QTextStream::Ok) {
            return content;
        }
    } else {
        QByteArray data = m_file->readAll();
        if (!data.isEmpty()) {
            return QString::fromUtf8(data);
        }
    }
    
    return std::nullopt;
}

std::optional<QByteArray> FileHandler::readRawData() {
    if (!isOpen()) {
        return std::nullopt;
    }
    
    QByteArray data = m_file->readAll();
    if (!data.isEmpty()) {
        return data;
    }
    
    return std::nullopt;
}

//==============================================================================
// Utilidades
//==============================================================================
bool FileHandler::flush() {
    if (!isOpen()) {
        return false;
    }
    
    if (m_textStream) {
        m_textStream->flush();
    }
    
    return m_file->flush();
}

bool FileHandler::seek(qint64 pos) {
    return isOpen() && m_file->seek(pos);
}

qint64 FileHandler::size() const {
    return m_file ? m_file->size() : 0;
}

//==============================================================================
// Métodos estáticos
//==============================================================================
bool FileHandler::writeTextToFile(const QString& filePath, const QString& text) {
    FileHandler handler(filePath, QIODevice::WriteOnly | QIODevice::Text);
    return handler.writeText(text);
}

std::optional<QString> FileHandler::readTextFromFile(const QString& filePath) {
    FileHandler handler(filePath, QIODevice::ReadOnly | QIODevice::Text);
    return handler.readAll();
}

bool FileHandler::appendToFile(const QString& filePath, const QString& text) {
    FileHandler handler(filePath, QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    return handler.writeText(text);
}

bool FileHandler::createDirectory(const QString& dirPath) {
    QDir dir;
    return dir.mkpath(dirPath);
}

bool FileHandler::ensureDirectoryExists(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.absoluteDir();
    
    if (!dir.exists()) {
        return dir.mkpath(dir.absolutePath());
    }
    
    return true;
}

//==============================================================================
// Métodos privados
//==============================================================================
void FileHandler::close() {
    if (m_textStream) {
        m_textStream.reset();
    }
    
    if (m_file && m_file->isOpen()) {
        m_file->close();
    }
    
    m_isOpen = false;
}

void FileHandler::reset() {
    m_file.reset();
    m_textStream.reset();
    m_filePath.clear();
    m_mode = QIODevice::NotOpen;
    m_isOpen = false;
}

} // namespace Vento::Utils
