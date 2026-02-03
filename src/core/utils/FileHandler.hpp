//==============================================================================
// FileHandler.hpp
// Vento - RAII File Management Utility
//==============================================================================
#pragma once

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QIODevice>
#include <memory>
#include <optional>

namespace Vento::Utils {

//==============================================================================
// FileHandler - RAII wrapper para manejo seguro de archivos
//==============================================================================
class FileHandler {
public:
    //==========================================================================
    // Constructores y Destructor
    //==========================================================================
    explicit FileHandler(const QString& filePath, QIODevice::OpenMode mode = QIODevice::ReadWrite | QIODevice::Text);
    ~FileHandler();
    
    // No permitir copia (RAII)
    FileHandler(const FileHandler&) = delete;
    FileHandler& operator=(const FileHandler&) = delete;
    
    // Permitir movimiento
    FileHandler(FileHandler&& other) noexcept;
    FileHandler& operator=(FileHandler&& other) noexcept;
    
    //==========================================================================
    // Operaciones de archivo
    //==========================================================================
    bool isOpen() const noexcept;
    bool exists() const noexcept;
    QString errorString() const noexcept;
    
    // Escritura segura
    bool writeLine(const QString& line);
    bool writeText(const QString& text);
    bool writeRawData(const QByteArray& data);
    
    // Lectura segura
    std::optional<QString> readLine();
    std::optional<QString> readAll();
    std::optional<QByteArray> readRawData();
    
    // Utilidades
    bool flush();
    bool seek(qint64 pos);
    qint64 size() const;
    
    //==========================================================================
    // Métodos estáticos de conveniencia
    //==========================================================================
    static bool writeTextToFile(const QString& filePath, const QString& text);
    static std::optional<QString> readTextFromFile(const QString& filePath);
    static bool appendToFile(const QString& filePath, const QString& text);
    static bool createDirectory(const QString& dirPath);
    static bool ensureDirectoryExists(const QString& filePath);

private:
    std::unique_ptr<QFile> m_file;
    std::unique_ptr<QTextStream> m_textStream;
    QString m_filePath;
    QIODevice::OpenMode m_mode;
    bool m_isOpen = false;
    
    void close();
    void reset();
};

} // namespace Vento::Utils
