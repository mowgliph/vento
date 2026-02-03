//==============================================================================
// CurrencyService.cpp
// Vento - Implementación del servicio de moneda
//==============================================================================

#include "CurrencyService.hpp"
#include "core/logger/Logger.hpp"
#include "core/utils/EventBus.hpp"

#include <QLocale>
#include <cmath>

namespace Vento {

//==============================================================================
// Singleton
//==============================================================================
CurrencyService& CurrencyService::instance() {
    static CurrencyService instance;
    return instance;
}

CurrencyService* CurrencyService::create(QQmlEngine* engine, QJSEngine* scriptEngine) {
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    
    // Retornar instancia existente (el engine no debe tomar ownership)
    auto* instance = &CurrencyService::instance();
    QJSEngine::setObjectOwnership(instance, QJSEngine::CppOwnership);
    return instance;
}

//==============================================================================
// Constructor
//==============================================================================
CurrencyService::CurrencyService(QObject* parent)
    : QObject(parent)
    , m_repository(std::make_unique<CurrencyRepository>(this))
{
    Logger::instance().debug("CurrencyService creado", "Currency");
}

//==============================================================================
// Inicialización
//==============================================================================
void CurrencyService::initialize() {
    if (m_initialized) {
        Logger::instance().warning("CurrencyService ya inicializado", "Currency");
        return;
    }
    
    loadFromDatabase();
    m_initialized = true;
    
    Logger::instance().info(
        QString("CurrencyService inicializado - Tasa: %1 %2")
            .arg(m_exchangeRate, 0, 'f', 2)
            .arg(m_localCurrencySymbol),
        "Currency"
    );
}

void CurrencyService::loadFromDatabase() {
    // Cargar tasa de cambio
    auto rateInfo = m_repository->getExchangeRateInfo();
    m_exchangeRate = rateInfo.rate;
    m_lastUpdate = rateInfo.lastUpdate;
    
    // Cargar símbolo de moneda
    m_localCurrencySymbol = m_repository->getLocalCurrencySymbol();
    
    // Cargar última entrada del historial para calcular cambio
    auto lastEntry = m_repository->getLastRateEntry();
    if (lastEntry) {
        m_previousRate = lastEntry->previousRate;
        m_changePercent = lastEntry->changePercent;
        updateTrend();
    }
}

//==============================================================================
// Getters
//==============================================================================
double CurrencyService::exchangeRate() const {
    return m_exchangeRate;
}

QString CurrencyService::exchangeRateFormatted() const {
    return QString::number(m_exchangeRate, 'f', 2);
}

QString CurrencyService::localCurrencySymbol() const {
    return m_localCurrencySymbol;
}

QString CurrencyService::usdSymbol() const {
    return m_repository->getUsdSymbol();
}

QString CurrencyService::lastUpdateFormatted() const {
    if (!m_lastUpdate.isValid()) {
        return tr("Sin actualizar");
    }
    
    QDateTime now = QDateTime::currentDateTime();
    qint64 secsAgo = m_lastUpdate.secsTo(now);
    
    if (secsAgo < 60) {
        return tr("Hace un momento");
    } else if (secsAgo < 3600) {
        int mins = static_cast<int>(secsAgo / 60);
        return tr("Hace %1 min").arg(mins);
    } else if (secsAgo < 86400) {
        int hours = static_cast<int>(secsAgo / 3600);
        return tr("Hace %1 h").arg(hours);
    } else {
        return m_lastUpdate.toString("dd/MM/yyyy hh:mm");
    }
}

double CurrencyService::changePercent() const {
    return m_changePercent;
}

QString CurrencyService::trend() const {
    return m_trend;
}

//==============================================================================
// Setters
//==============================================================================
void CurrencyService::setExchangeRate(double rate) {
    if (!isValidRate(rate)) {
        emit errorOccurred(tr("Tasa de cambio inválida: %1").arg(rate));
        return;
    }
    
    if (std::abs(m_exchangeRate - rate) < 0.0001) {
        return; // Sin cambio significativo
    }
    
    updateRate(rate);
}

void CurrencyService::setLocalCurrencySymbol(const QString& symbol) {
    if (symbol.isEmpty() || symbol == m_localCurrencySymbol) {
        return;
    }
    
    if (m_repository->setLocalCurrencySymbol(symbol)) {
        m_localCurrencySymbol = symbol;
        emit localCurrencySymbolChanged(symbol);
        
        Logger::instance().info(
            QString("Símbolo de moneda actualizado: %1").arg(symbol),
            "Currency"
        );
    }
}

//==============================================================================
// Métodos Q_INVOKABLE
//==============================================================================
bool CurrencyService::updateRate(double newRate) {
    return updateRateFromSource(newRate, "manual");
}

bool CurrencyService::updateRateFromSource(double newRate, const QString& source) {
    if (!isValidRate(newRate)) {
        emit errorOccurred(tr("Tasa de cambio fuera de rango permitido"));
        Logger::instance().warning(
            QString("Intento de establecer tasa inválida: %1").arg(newRate),
            "Currency"
        );
        return false;
    }
    
    // Determinar fuente
    Currency::RateUpdateSource rateSource = Currency::RateUpdateSource::Manual;
    if (source == "bcv") rateSource = Currency::RateUpdateSource::BCV;
    else if (source == "dolartoday") rateSource = Currency::RateUpdateSource::DolarToday;
    else if (source == "binance") rateSource = Currency::RateUpdateSource::Binance;
    
    // Guardar tasa anterior
    double previousRate = m_exchangeRate;
    
    // Persistir en BD
    if (!m_repository->saveExchangeRate(newRate, rateSource)) {
        emit errorOccurred(tr("Error al guardar la tasa de cambio"));
        return false;
    }
    
    // Actualizar cache
    m_previousRate = previousRate;
    m_exchangeRate = newRate;
    m_lastUpdate = QDateTime::currentDateTime();
    
    // Calcular cambio porcentual
    if (m_previousRate > 0) {
        m_changePercent = ((newRate - m_previousRate) / m_previousRate) * 100.0;
    } else {
        m_changePercent = 0.0;
    }
    
    updateTrend();
    
    // ═══════════════════════════════════════════════════════════════════════
    // PATRÓN OBSERVER: Emitir señales para notificar a otros módulos
    // ═══════════════════════════════════════════════════════════════════════
    
    // Señal simple para binding QML
    emit exchangeRateChanged(newRate);
    
    // Señal detallada para lógica de negocio
    emit rateUpdated(newRate, previousRate, m_changePercent);
    
    // También publicar en el EventBus para componentes C++ que no usen señales Qt
    EventBus::instance().publish(
        Events::CURRENCY_RATE_CHANGED,
        QVariant::fromValue(newRate)
    );
    
    Logger::instance().info(
        QString("💱 Tasa actualizada: %1 → %2 (%3%4%)")
            .arg(previousRate, 0, 'f', 2)
            .arg(newRate, 0, 'f', 2)
            .arg(m_changePercent >= 0 ? "+" : "")
            .arg(m_changePercent, 0, 'f', 2),
        "Currency"
    );
    
    return true;
}

double CurrencyService::convertToLocal(double amountUsd) const {
    return amountUsd * m_exchangeRate;
}

double CurrencyService::convertToUsd(double amountLocal) const {
    if (m_exchangeRate <= 0) {
        return 0.0;
    }
    return amountLocal / m_exchangeRate;
}

QString CurrencyService::formatLocal(double amount) const {
    return formatAmount(amount, m_localCurrencySymbol, 2);
}

QString CurrencyService::formatUsd(double amount) const {
    return formatAmount(amount, usdSymbol(), 2);
}

QString CurrencyService::formatAmount(double amount, const QString& symbol, int decimals) const {
    QLocale locale(QLocale::Spanish, QLocale::Venezuela);
    QString formatted = locale.toString(amount, 'f', decimals);
    return QString("%1 %2").arg(symbol, formatted);
}

double CurrencyService::calculateSalePrice(double costUsd, double marginPercent) const {
    // Precio de venta = (Costo USD × Tasa) × (1 + Margen/100)
    double costLocal = convertToLocal(costUsd);
    double marginMultiplier = 1.0 + (marginPercent / 100.0);
    return costLocal * marginMultiplier;
}

QVariantList CurrencyService::getRateHistory(int limit) const {
    auto history = m_repository->getRateHistory(limit);
    
    QVariantList result;
    result.reserve(history.size());
    
    for (const auto& entry : history) {
        QVariantMap item;
        item["id"] = entry.id;
        item["rate"] = entry.rate;
        item["timestamp"] = entry.timestamp.toString("dd/MM/yyyy hh:mm");
        item["timestampIso"] = entry.timestamp.toString(Qt::ISODate);
        item["previousRate"] = entry.previousRate;
        item["changePercent"] = entry.changePercent;
        item["source"] = Currency::rateSourceToString(entry.source);
        item["trend"] = entry.changePercent > 0 ? "up" : 
                       (entry.changePercent < 0 ? "down" : "stable");
        result.append(item);
    }
    
    return result;
}

QVariantMap CurrencyService::getRateInfo() const {
    QVariantMap info;
    info["rate"] = m_exchangeRate;
    info["rateFormatted"] = exchangeRateFormatted();
    info["localSymbol"] = m_localCurrencySymbol;
    info["usdSymbol"] = usdSymbol();
    info["lastUpdate"] = m_lastUpdate;
    info["lastUpdateFormatted"] = lastUpdateFormatted();
    info["previousRate"] = m_previousRate;
    info["changePercent"] = m_changePercent;
    info["trend"] = m_trend;
    return info;
}

bool CurrencyService::cleanHistory(int daysToKeep) {
    return m_repository->cleanOldHistory(daysToKeep);
}

void CurrencyService::refresh() {
    loadFromDatabase();
    emit exchangeRateChanged(m_exchangeRate);
    
    Logger::instance().debug("CurrencyService refrescado", "Currency");
}

//==============================================================================
// Métodos privados
//==============================================================================
void CurrencyService::updateTrend() {
    if (std::abs(m_changePercent) < 0.01) {
        m_trend = "stable";
    } else if (m_changePercent > 0) {
        m_trend = "up";
    } else {
        m_trend = "down";
    }
}

bool CurrencyService::isValidRate(double rate) const {
    return rate >= Currency::Defaults::MIN_EXCHANGE_RATE && 
           rate <= Currency::Defaults::MAX_EXCHANGE_RATE;
}

} // namespace Vento
