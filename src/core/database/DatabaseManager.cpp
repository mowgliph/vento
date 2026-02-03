//==============================================================================
// DatabaseManager.cpp
// Vento - Implementación del gestor de base de datos
//==============================================================================

#include "DatabaseManager.hpp"
#include "DatabaseSchema.hpp"
#include "../logger/Logger.hpp"

#include <QSqlRecord>
#include <QSqlDriver>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>
#include <QThread>
#include <QRegularExpression>

namespace Vento {

//==============================================================================
// Singleton Instance
//==============================================================================
DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

//==============================================================================
// Constructor / Destructor
//==============================================================================
DatabaseManager::DatabaseManager() : QObject(nullptr) {
    Logger::instance().debug("DatabaseManager creado");
}

DatabaseManager::~DatabaseManager() {
    close();
    Logger::instance().debug("DatabaseManager destruido");
}

//==============================================================================
// Inicialización
//==============================================================================
DatabaseResult DatabaseManager::initialize(const QString& dbPath) {
    DatabaseConfig config;
    config.path = dbPath;
    return initialize(config);
}

DatabaseResult DatabaseManager::initialize(const DatabaseConfig& config) {
    QMutexLocker locker(&m_mutex);
    
    if (m_connected) {
        Logger::instance().warning("Base de datos ya conectada. Reconectando...");
        close();
    }
    
    m_config = config;
    
    Logger::instance().info(QString("Inicializando base de datos: %1").arg(config.path));
    
    QFileInfo fileInfo(config.path);
    QDir dir = fileInfo.absoluteDir();
    
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            auto error = QString("No se pudo crear el directorio: %1").arg(dir.absolutePath());
            Logger::instance().error(error);
            return DatabaseResult::error(error);
        }
        Logger::instance().info(QString("Directorio creado: %1").arg(dir.absolutePath()));
    }
    
    if (QSqlDatabase::contains(config.connectionName)) {
        m_database = QSqlDatabase::database(config.connectionName);
    } else {
        m_database = QSqlDatabase::addDatabase("QSQLITE", config.connectionName);
    }
    
    m_database.setDatabaseName(config.path);
    
    if (!m_database.open()) {
        logSqlError("Abrir base de datos", m_database.lastError());
        return DatabaseResult::error(
            QString("Error al abrir base de datos: %1").arg(m_database.lastError().text())
        );
    }
    
    m_connected = true;
    Logger::instance().info("✓ Conexión a base de datos establecida");
    
    auto configResult = configureSQLite();
    if (!configResult) {
        return configResult;
    }
    
    auto tablesResult = initializeTables();
    if (!tablesResult) {
        return tablesResult;
    }
    
    auto migrateResult = checkAndMigrate();
    if (!migrateResult) {
        return migrateResult;
    }
    
    emit connectionChanged(true);
    emit tablesInitialized();
    
    Logger::instance().info(QString("✓ Base de datos inicializada correctamente (v%1)")
        .arg(Database::SCHEMA_VERSION));
    
    return DatabaseResult::ok();
}

//==============================================================================
// Configuración de SQLite
//==============================================================================
DatabaseResult DatabaseManager::configureSQLite() {
    QSqlQuery query(m_database);
    
    QStringList pragmas;
    
    if (m_config.enableForeignKeys) {
        pragmas << "PRAGMA foreign_keys = ON";
    }
    
    if (m_config.enableWAL) {
        pragmas << "PRAGMA journal_mode = WAL";
    }
    
    pragmas << QString("PRAGMA busy_timeout = %1").arg(m_config.busyTimeout);
    pragmas << "PRAGMA synchronous = NORMAL";
    pragmas << "PRAGMA cache_size = -2000";
    pragmas << "PRAGMA temp_store = MEMORY";
    
    for (const QString& pragma : pragmas) {
        if (!query.exec(pragma)) {
            logSqlError(pragma, query.lastError());
            Logger::instance().warning(QString("PRAGMA falló: %1").arg(pragma));
        }
    }
    
    Logger::instance().debug("✓ SQLite configurado con PRAGMAs de optimización");
    return DatabaseResult::ok();
}

//==============================================================================
// Inicialización de tablas
//==============================================================================
DatabaseResult DatabaseManager::initializeTables() {
    Logger::instance().info("Inicializando tablas del esquema...");
    
    if (!m_database.transaction()) {
        return DatabaseResult::error("No se pudo iniciar transacción");
    }
    
    QSqlQuery query(m_database);
    
    try {
        for (const QString& sql : Database::getInitializationStatements()) {
            if (!query.exec(sql)) {
                logSqlError("Crear tabla", query.lastError());
                m_database.rollback();
                return DatabaseResult::error(
                    QString("Error creando tabla: %1").arg(query.lastError().text())
                );
            }
        }
        Logger::instance().debug("  ✓ Tablas principales creadas");
        
        for (const QString& sql : Database::getIndexStatements()) {
            QString trimmedSql = sql.trimmed();
            if (trimmedSql.isEmpty()) continue;
            
            if (!query.exec(trimmedSql)) {
                logSqlError("Crear índice", query.lastError());
                Logger::instance().warning(QString("Índice no creado: %1").arg(trimmedSql.left(50)));
            }
        }
        Logger::instance().debug("  ✓ Índices creados");
        
        for (const QString& sql : Database::getDefaultDataStatements()) {
            if (!query.exec(sql)) {
                logSqlError("Insertar datos default", query.lastError());
            }
        }
        Logger::instance().debug("  ✓ Datos por defecto insertados");
        
        if (!m_database.commit()) {
            m_database.rollback();
            return DatabaseResult::error("Error en commit de transacción");
        }
        
        Logger::instance().info("✓ Todas las tablas inicializadas correctamente");
        return DatabaseResult::ok();
        
    } catch (const std::exception& e) {
        m_database.rollback();
        return DatabaseResult::error(QString("Excepción: %1").arg(e.what()));
    }
}

//==============================================================================
// Migraciones
//==============================================================================
int DatabaseManager::getCurrentSchemaVersion() {
    QSqlQuery query(m_database);
    query.prepare("SELECT version FROM schema_info WHERE id = 1");
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    return 0;
}

DatabaseResult DatabaseManager::checkAndMigrate() {
    int currentVersion = getCurrentSchemaVersion();
    int targetVersion = Database::SCHEMA_VERSION;
    
    if (currentVersion == targetVersion) {
        Logger::instance().debug(QString("Esquema actualizado (v%1)").arg(currentVersion));
        return DatabaseResult::ok();
    }
    
    if (currentVersion > targetVersion) {
        Logger::instance().warning(
            QString("Versión de BD (%1) mayor que la aplicación (%2)")
            .arg(currentVersion).arg(targetVersion)
        );
        return DatabaseResult::ok();
    }
    
    Logger::instance().info(
        QString("Migración requerida: v%1 -> v%2")
        .arg(currentVersion).arg(targetVersion)
    );
    
    QSqlQuery query(m_database);
    query.prepare("UPDATE schema_info SET version = ?, updated_at = datetime('now', 'localtime') WHERE id = 1");
    query.addBindValue(targetVersion);
    
    if (!query.exec()) {
        logSqlError("Actualizar versión esquema", query.lastError());
        return DatabaseResult::error("Error actualizando versión del esquema");
    }
    
    emit migrationCompleted(currentVersion, targetVersion);
    Logger::instance().info(QString("✓ Migración completada a v%1").arg(targetVersion));
    
    return DatabaseResult::ok();
}

//==============================================================================
// Operaciones de base de datos
//==============================================================================
DatabaseResult DatabaseManager::execute(const QString& sql, const QVariantList& params) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_connected) {
        return DatabaseResult::error("Base de datos no conectada");
    }
    
    QSqlQuery query(m_database);
    query.prepare(sql);
    
    for (int i = 0; i < params.size(); ++i) {
        query.addBindValue(params[i]);
    }
    
    if (!query.exec()) {
        logSqlError("Execute", query.lastError());
        return DatabaseResult::error(query.lastError().text());
    }
    
    QVariant lastId = query.lastInsertId();
    return DatabaseResult::ok(lastId.isValid() ? lastId : QVariant(query.numRowsAffected()));
}

DatabaseResult DatabaseManager::executeScalar(const QString& sql, const QVariantList& params) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_connected) {
        return DatabaseResult::error("Base de datos no conectada");
    }
    
    QSqlQuery query(m_database);
    query.prepare(sql);
    
    for (int i = 0; i < params.size(); ++i) {
        query.addBindValue(params[i]);
    }
    
    if (!query.exec()) {
        logSqlError("ExecuteScalar", query.lastError());
        return DatabaseResult::error(query.lastError().text());
    }
    
    if (query.next()) {
        return DatabaseResult::ok(query.value(0));
    }
    
    return DatabaseResult::ok();
}

QList<QVariantMap> DatabaseManager::executeQuery(const QString& sql, const QVariantList& params) {
    QMutexLocker locker(&m_mutex);
    
    QList<QVariantMap> results;
    
    if (!m_connected) {
        Logger::instance().error("ExecuteQuery: Base de datos no conectada");
        return results;
    }
    
    QSqlQuery query(m_database);
    query.prepare(sql);
    
    for (int i = 0; i < params.size(); ++i) {
        query.addBindValue(params[i]);
    }
    
    if (!query.exec()) {
        logSqlError("ExecuteQuery", query.lastError());
        return results;
    }
    
    QSqlRecord record = query.record();
    int columnCount = record.count();
    
    while (query.next()) {
        QVariantMap row;
        for (int i = 0; i < columnCount; ++i) {
            row[record.fieldName(i)] = query.value(i);
        }
        results.append(row);
    }
    
    return results;
}

DatabaseResult DatabaseManager::executeTransaction(const QStringList& statements) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_connected) {
        return DatabaseResult::error("Base de datos no conectada");
    }
    
    if (!m_database.transaction()) {
        return DatabaseResult::error("No se pudo iniciar transacción");
    }
    
    QSqlQuery query(m_database);
    
    for (const QString& sql : statements) {
        if (!query.exec(sql)) {
            logSqlError("Transaction statement", query.lastError());
            m_database.rollback();
            return DatabaseResult::error(query.lastError().text());
        }
    }
    
    if (!m_database.commit()) {
        m_database.rollback();
        return DatabaseResult::error("Error en commit");
    }
    
    return DatabaseResult::ok();
}

DatabaseResult DatabaseManager::executeInTransaction(std::function<bool(QSqlQuery&)> func) {
    QMutexLocker locker(&m_mutex);
    
    if (!m_connected) {
        return DatabaseResult::error("Base de datos no conectada");
    }
    
    if (!m_database.transaction()) {
        return DatabaseResult::error("No se pudo iniciar transacción");
    }
    
    QSqlQuery query(m_database);
    
    try {
        if (func(query)) {
            if (m_database.commit()) {
                return DatabaseResult::ok();
            }
            m_database.rollback();
            return DatabaseResult::error("Error en commit");
        } else {
            m_database.rollback();
            return DatabaseResult::error("Función de transacción retornó false");
        }
    } catch (const std::exception& e) {
        m_database.rollback();
        return DatabaseResult::error(QString("Excepción: %1").arg(e.what()));
    }
}

//==============================================================================
// Utilidades
//==============================================================================
bool DatabaseManager::tableExists(const QString& tableName) {
    QMutexLocker locker(&m_mutex);
    
    QSqlQuery query(m_database);
    query.prepare(
        "SELECT COUNT(*) FROM sqlite_master "
        "WHERE type = 'table' AND name = ?"
    );
    query.addBindValue(tableName);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    
    return false;
}

int DatabaseManager::rowCount(const QString& tableName) {
    QMutexLocker locker(&m_mutex);
    
    QString safeName = tableName;
    safeName.remove(QRegularExpression("[^a-zA-Z0-9_]"));
    
    QSqlQuery query(m_database);
    if (query.exec(QString("SELECT COUNT(*) FROM %1").arg(safeName)) && query.next()) {
        return query.value(0).toInt();
    }
    
    return -1;
}

qint64 DatabaseManager::lastInsertId() {
    QMutexLocker locker(&m_mutex);
    
    QSqlQuery query(m_database);
    if (query.exec("SELECT last_insert_rowid()") && query.next()) {
        return query.value(0).toLongLong();
    }
    
    return -1;
}

QSqlDatabase& DatabaseManager::database() {
    return m_database;
}

QSqlQuery DatabaseManager::createQuery() {
    return QSqlQuery(m_database);
}

DatabaseResult DatabaseManager::vacuum() {
    QMutexLocker locker(&m_mutex);
    
    QSqlQuery query(m_database);
    if (query.exec("VACUUM")) {
        Logger::instance().info("VACUUM ejecutado correctamente");
        return DatabaseResult::ok();
    }
    
    return DatabaseResult::error(query.lastError().text());
}

QVariantMap DatabaseManager::getStatistics() {
    QMutexLocker locker(&m_mutex);
    
    QVariantMap stats;
    stats["connected"] = m_connected;
    stats["path"] = m_config.path;
    stats["schemaVersion"] = Database::SCHEMA_VERSION;
    
    if (m_connected) {
        stats["productsCount"] = rowCount(Database::Tables::PRODUCTS);
        stats["salesCount"] = rowCount(Database::Tables::SALES);
        
        QFileInfo fileInfo(m_config.path);
        stats["fileSizeBytes"] = fileInfo.size();
        stats["fileSizeMB"] = QString::number(fileInfo.size() / 1024.0 / 1024.0, 'f', 2);
    }
    
    return stats;
}

//==============================================================================
// Close
//==============================================================================
void DatabaseManager::close() {
    QMutexLocker locker(&m_mutex);
    
    if (m_connected && m_database.isOpen()) {
        m_database.close();
        m_connected = false;
        
        Logger::instance().info("Base de datos cerrada");
        emit connectionChanged(false);
    }
}

//==============================================================================
// Getters
//==============================================================================
bool DatabaseManager::isConnected() const {
    return m_connected && m_database.isOpen();
}

QString DatabaseManager::databasePath() const {
    return m_config.path;
}

int DatabaseManager::schemaVersion() const {
    return Database::SCHEMA_VERSION;
}

//==============================================================================
// Logging
//==============================================================================
void DatabaseManager::logSqlError(const QString& context, const QSqlError& error) {
    if (error.isValid()) {
        Logger::instance().error(
            QString("[SQL Error] %1\n"
                    "  Type: %2\n"
                    "  Database: %3\n"
                    "  Driver: %4")
            .arg(context)
            .arg(error.type())
            .arg(error.databaseText())
            .arg(error.driverText())
        );
        
        emit errorOccurred(error.text());
    }
}

} // namespace Vento
