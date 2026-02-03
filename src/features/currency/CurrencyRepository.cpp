//==============================================================================
// CurrencyRepository.cpp
// Vento - Implementación del repositorio de moneda
//==============================================================================

#include "CurrencyRepository.hpp"
#include "core/database/DatabaseManager.hpp"
#include "core/logger/Logger.hpp"

#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>

namespace Vento {

//==============================================================================
// Constructor
//==============================================================================
CurrencyRepository::CurrencyRepository(QObject* parent) 
    : QObject(parent) 
{
    ensureHistoryTableExists();
    Logger::instance().debug("CurrencyRepository inicializado", "Currency");
}

//==============================================================================
// Tasa de cambio
//==============================================================================
bool CurrencyRepository::saveExchangeRate(double rate, Currency::RateUpdateSource source) {
    auto& db = DatabaseManager::instance();
    
    // Validar rango
    if (rate < Currency::Defaults::MIN_EXCHANGE_RATE || 
        rate > Currency::Defaults::MAX_EXCHANGE_RATE) {
        Logger::instance().warning(
            QString("Tasa de cambio fuera de rango: %1").arg(rate), 
            "Currency"
        );
        return false;
    }
    
    // Obtener tasa anterior para el historial
    double previousRate = getExchangeRate();
    
    // Actualizar en settings
    auto result = db.execute(
        R"(
            INSERT INTO settings (key, value, description, updated_at)
            VALUES (?, ?, ?, datetime('now', 'localtime'))
            ON CONFLICT(key) DO UPDATE SET 
                value = excluded.value,
                updated_at = datetime('now', 'localtime')
        )",
        {
            Currency::SettingsKeys::EXCHANGE_RATE_USD,
            QString::number(rate, 'f', 4),
            "Tasa de cambio USD a moneda local"
        }
    );
    
    if (!result) {
        Logger::instance().error(
            QString("Error guardando tasa de cambio: %1").arg(result.errorMessage),
            "Currency"
        );
        return false;
    }
    
    // Guardar fuente de actualización
    db.execute(
        R"(
            INSERT INTO settings (key, value, updated_at)
            VALUES (?, ?, datetime('now', 'localtime'))
            ON CONFLICT(key) DO UPDATE SET 
                value = excluded.value,
                updated_at = datetime('now', 'localtime')
        )",
        {
            Currency::SettingsKeys::RATE_UPDATE_SOURCE,
            Currency::rateSourceToString(source)
        }
    );
    
    // Guardar en historial
    RateHistoryEntry historyEntry;
    historyEntry.rate = rate;
    historyEntry.timestamp = QDateTime::currentDateTime();
    historyEntry.source = source;
    historyEntry.previousRate = previousRate;
    historyEntry.calculateChange();
    
    saveRateHistory(historyEntry);
    
    Logger::instance().info(
        QString("Tasa de cambio actualizada: %1 (fuente: %2, cambio: %3%)")
            .arg(rate, 0, 'f', 4)
            .arg(Currency::rateSourceToString(source))
            .arg(historyEntry.changePercent, 0, 'f', 2),
        "Currency"
    );
    
    return true;
}

double CurrencyRepository::getExchangeRate() const {
    auto result = DatabaseManager::instance().executeScalar(
        "SELECT value FROM settings WHERE key = ?",
        {Currency::SettingsKeys::EXCHANGE_RATE_USD}
    );
    
    if (result && result.data.isValid()) {
        bool ok;
        double rate = result.data.toString().toDouble(&ok);
        if (ok && rate > 0) {
            return rate;
        }
    }
    
    return Currency::Defaults::EXCHANGE_RATE_USD;
}

ExchangeRateInfo CurrencyRepository::getExchangeRateInfo() const {
    auto& db = DatabaseManager::instance();
    ExchangeRateInfo info;
    
    // Obtener tasa
    info.rate = getExchangeRate();
    
    // Obtener última actualización
    auto updateResult = db.executeScalar(
        "SELECT updated_at FROM settings WHERE key = ?",
        {Currency::SettingsKeys::EXCHANGE_RATE_USD}
    );
    
    if (updateResult && updateResult.data.isValid()) {
        info.lastUpdate = QDateTime::fromString(
            updateResult.data.toString(), 
            "yyyy-MM-dd hh:mm:ss"
        );
    }
    
    // Obtener fuente
    auto sourceResult = db.executeScalar(
        "SELECT value FROM settings WHERE key = ?",
        {Currency::SettingsKeys::RATE_UPDATE_SOURCE}
    );
    
    if (sourceResult && sourceResult.data.isValid()) {
        QString sourceStr = sourceResult.data.toString();
        if (sourceStr == "bcv") info.source = Currency::RateUpdateSource::BCV;
        else if (sourceStr == "dolartoday") info.source = Currency::RateUpdateSource::DolarToday;
        else if (sourceStr == "binance") info.source = Currency::RateUpdateSource::Binance;
        else info.source = Currency::RateUpdateSource::Manual;
    }
    
    return info;
}

//==============================================================================
// Símbolos de moneda
//==============================================================================
QString CurrencyRepository::getLocalCurrencySymbol() const {
    auto result = DatabaseManager::instance().executeScalar(
        "SELECT value FROM settings WHERE key = ?",
        {Currency::SettingsKeys::CURRENCY_SYMBOL_LOCAL}
    );
    
    if (result && result.data.isValid()) {
        return result.data.toString();
    }
    
    return "Bs.";
}

bool CurrencyRepository::setLocalCurrencySymbol(const QString& symbol) {
    auto result = DatabaseManager::instance().execute(
        R"(
            INSERT INTO settings (key, value, updated_at)
            VALUES (?, ?, datetime('now', 'localtime'))
            ON CONFLICT(key) DO UPDATE SET 
                value = excluded.value,
                updated_at = datetime('now', 'localtime')
        )",
        {Currency::SettingsKeys::CURRENCY_SYMBOL_LOCAL, symbol}
    );
    
    return result.success;
}

QString CurrencyRepository::getUsdSymbol() const {
    auto result = DatabaseManager::instance().executeScalar(
        "SELECT value FROM settings WHERE key = ?",
        {Currency::SettingsKeys::CURRENCY_SYMBOL_USD}
    );
    
    if (result && result.data.isValid()) {
        return result.data.toString();
    }
    
    return "$";
}

//==============================================================================
// Historial
//==============================================================================
void CurrencyRepository::ensureHistoryTableExists() {
    DatabaseManager::instance().execute(R"(
        CREATE TABLE IF NOT EXISTS rate_history (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            rate REAL NOT NULL,
            timestamp TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
            source TEXT DEFAULT 'manual',
            previous_rate REAL,
            change_percent REAL
        )
    )");
    
    DatabaseManager::instance().execute(
        "CREATE INDEX IF NOT EXISTS idx_rate_history_timestamp ON rate_history(timestamp)"
    );
}

bool CurrencyRepository::saveRateHistory(const RateHistoryEntry& entry) {
    auto result = DatabaseManager::instance().execute(
        R"(
            INSERT INTO rate_history (rate, timestamp, source, previous_rate, change_percent)
            VALUES (?, ?, ?, ?, ?)
        )",
        {
            entry.rate,
            entry.timestamp.toString("yyyy-MM-dd hh:mm:ss"),
            Currency::rateSourceToString(entry.source),
            entry.previousRate,
            entry.changePercent
        }
    );
    
    return result.success;
}

QList<RateHistoryEntry> CurrencyRepository::getRateHistory(int limit) const {
    QString sql = R"(
        SELECT id, rate, timestamp, source, previous_rate, change_percent
        FROM rate_history
        ORDER BY timestamp DESC
    )";
    
    if (limit > 0) {
        sql += QString(" LIMIT %1").arg(limit);
    }
    
    auto rows = DatabaseManager::instance().executeQuery(sql);
    
    QList<RateHistoryEntry> history;
    history.reserve(rows.size());
    
    for (const auto& row : rows) {
        RateHistoryEntry entry;
        entry.id = row["id"].toLongLong();
        entry.rate = row["rate"].toDouble();
        entry.timestamp = QDateTime::fromString(
            row["timestamp"].toString(), 
            "yyyy-MM-dd hh:mm:ss"
        );
        entry.previousRate = row["previous_rate"].toDouble();
        entry.changePercent = row["change_percent"].toDouble();
        
        QString sourceStr = row["source"].toString();
        if (sourceStr == "bcv") entry.source = Currency::RateUpdateSource::BCV;
        else if (sourceStr == "dolartoday") entry.source = Currency::RateUpdateSource::DolarToday;
        else entry.source = Currency::RateUpdateSource::Manual;
        
        history.append(entry);
    }
    
    return history;
}

std::optional<RateHistoryEntry> CurrencyRepository::getLastRateEntry() const {
    auto history = getRateHistory(1);
    if (!history.isEmpty()) {
        return history.first();
    }
    return std::nullopt;
}

bool CurrencyRepository::cleanOldHistory(int daysToKeep) {
    auto result = DatabaseManager::instance().execute(
        R"(
            DELETE FROM rate_history 
            WHERE timestamp < datetime('now', ? || ' days')
        )",
        {QString("-%1").arg(daysToKeep)}
    );
    
    if (result) {
        int deleted = result.data.toInt();
        if (deleted > 0) {
            Logger::instance().info(
                QString("Historial de tasas limpiado: %1 registros eliminados").arg(deleted),
                "Currency"
            );
        }
    }
    
    return result.success;
}

//==============================================================================
// Utilidades
//==============================================================================
QString CurrencyRepository::getSetting(const QString& key, const QString& defaultValue) const {
    auto result = DatabaseManager::instance().executeScalar(
        "SELECT value FROM settings WHERE key = ?",
        {key}
    );
    
    if (result && result.data.isValid()) {
        return result.data.toString();
    }
    
    return defaultValue;
}

bool CurrencyRepository::setSetting(const QString& key, const QString& value) {
    auto result = DatabaseManager::instance().execute(
        R"(
            INSERT INTO settings (key, value, updated_at)
            VALUES (?, ?, datetime('now', 'localtime'))
            ON CONFLICT(key) DO UPDATE SET 
                value = excluded.value,
                updated_at = datetime('now', 'localtime')
        )",
        {key, value}
    );
    
    return result.success;
}

} // namespace Vento
