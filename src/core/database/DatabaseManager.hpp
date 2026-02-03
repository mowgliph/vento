//==============================================================================
// DatabaseManager.hpp
// Vento - Gestor de conexión y operaciones de base de datos SQLite
//
// Principios SOLID aplicados:
// - Single Responsibility: Solo gestiona conexión e inicialización
// - Open/Closed: Extensible vía DatabaseSchema sin modificar esta clase
// - Dependency Inversion: Usa abstracciones (interfaces) donde es posible
//==============================================================================
#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include <QMutex>

#include <optional>
#include <functional>

namespace Vento {

//==============================================================================
// Resultado de operación de base de datos
//==============================================================================
struct DatabaseResult {
    bool success = false;
    QString errorMessage;
    QVariant data;
    
    explicit operator bool() const { return success; }
    
    static DatabaseResult ok(const QVariant& data = {}) {
        return {true, {}, data};
    }
    
    static DatabaseResult error(const QString& message) {
        return {false, message, {}};
    }
};

//==============================================================================
// Configuración de la base de datos
//==============================================================================
struct DatabaseConfig {
    QString path;
    QString connectionName = "VentoConnection";
    bool enableForeignKeys = true;
    bool enableWAL = true;
    int busyTimeout = 5000;
};

//==============================================================================
// DatabaseManager - Singleton Thread-Safe
//==============================================================================
class DatabaseManager : public QObject {
    Q_OBJECT
    
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionChanged)
    Q_PROPERTY(QString databasePath READ databasePath NOTIFY connectionChanged)
    Q_PROPERTY(int schemaVersion READ schemaVersion CONSTANT)
    
public:
    static DatabaseManager& instance();
    
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    DatabaseManager(DatabaseManager&&) = delete;
    DatabaseManager& operator=(DatabaseManager&&) = delete;
    
    [[nodiscard]] DatabaseResult initialize(const DatabaseConfig& config);
    [[nodiscard]] DatabaseResult initialize(const QString& dbPath);
    void close();
    [[nodiscard]] bool isConnected() const;
    [[nodiscard]] QString databasePath() const;
    [[nodiscard]] int schemaVersion() const;
    
    [[nodiscard]] DatabaseResult execute(
        const QString& sql, 
        const QVariantList& params = {}
    );
    
    [[nodiscard]] DatabaseResult executeScalar(
        const QString& sql, 
        const QVariantList& params = {}
    );
    
    [[nodiscard]] QList<QVariantMap> executeQuery(
        const QString& sql, 
        const QVariantList& params = {}
    );
    
    [[nodiscard]] DatabaseResult executeTransaction(
        const QStringList& statements
    );
    
    [[nodiscard]] DatabaseResult executeInTransaction(
        std::function<bool(QSqlQuery&)> func
    );
    
    [[nodiscard]] bool tableExists(const QString& tableName);
    [[nodiscard]] int rowCount(const QString& tableName);
    [[nodiscard]] qint64 lastInsertId();
    [[nodiscard]] QSqlDatabase& database();
    [[nodiscard]] QSqlQuery createQuery();
    [[nodiscard]] DatabaseResult vacuum();
    [[nodiscard]] QVariantMap getStatistics();
    
signals:
    void connectionChanged(bool connected);
    void errorOccurred(const QString& error);
    void tablesInitialized();
    void migrationCompleted(int fromVersion, int toVersion);
    
private:
    DatabaseManager();
    ~DatabaseManager() override;
    
    [[nodiscard]] DatabaseResult initializeTables();
    [[nodiscard]] DatabaseResult configureSQLite();
    [[nodiscard]] DatabaseResult checkAndMigrate();
    [[nodiscard]] int getCurrentSchemaVersion();
    void logSqlError(const QString& context, const QSqlError& error);
    
    QSqlDatabase m_database;
    DatabaseConfig m_config;
    bool m_connected = false;
    mutable QMutex m_mutex;
};

} // namespace Vento
