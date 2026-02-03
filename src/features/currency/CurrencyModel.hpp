//==============================================================================
// CurrencyModel.hpp
// Vento - Modelo de datos para información de moneda
//==============================================================================
#pragma once

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QJsonObject>

#include "CurrencyTypes.hpp"

namespace Vento {

//==============================================================================
// ExchangeRateInfo - Información completa de tasa de cambio
//==============================================================================
struct ExchangeRateInfo {
    double rate = Currency::Defaults::EXCHANGE_RATE_USD;
    QDateTime lastUpdate;
    Currency::RateUpdateSource source = Currency::RateUpdateSource::Manual;
    QString sourceDescription;
    
    // Serialización
    QJsonObject toJson() const {
        return {
            {"rate", rate},
            {"lastUpdate", lastUpdate.toString(Qt::ISODate)},
            {"source", Currency::rateSourceToString(source)},
            {"sourceDescription", sourceDescription}
        };
    }
    
    static ExchangeRateInfo fromJson(const QJsonObject& json) {
        ExchangeRateInfo info;
        info.rate = json["rate"].toDouble(Currency::Defaults::EXCHANGE_RATE_USD);
        info.lastUpdate = QDateTime::fromString(json["lastUpdate"].toString(), Qt::ISODate);
        info.sourceDescription = json["sourceDescription"].toString();
        return info;
    }
    
    // Validación
    bool isValid() const {
        return rate >= Currency::Defaults::MIN_EXCHANGE_RATE && 
               rate <= Currency::Defaults::MAX_EXCHANGE_RATE;
    }
};

//==============================================================================
// CurrencyInfo - Información de una moneda
//==============================================================================
struct CurrencyInfo {
    Currency::CurrencyCode code;
    QString symbol;
    QString name;
    QString country;
    int decimals = 2;
    
    static CurrencyInfo USD() {
        return {Currency::CurrencyCode::USD, "$", "Dólar Estadounidense", "USA", 2};
    }
    
    static CurrencyInfo VES() {
        return {Currency::CurrencyCode::VES, "Bs.", "Bolívar", "Venezuela", 2};
    }
};

//==============================================================================
// RateHistoryEntry - Entrada del historial de tasas
//==============================================================================
struct RateHistoryEntry {
    qint64 id = 0;
    double rate = 0.0;
    QDateTime timestamp;
    Currency::RateUpdateSource source = Currency::RateUpdateSource::Manual;
    double previousRate = 0.0;
    double changePercent = 0.0;
    
    // Calcular porcentaje de cambio
    void calculateChange() {
        if (previousRate > 0) {
            changePercent = ((rate - previousRate) / previousRate) * 100.0;
        }
    }
};

} // namespace Vento

// Registrar tipos para uso en Qt meta-system
Q_DECLARE_METATYPE(Vento::ExchangeRateInfo)
Q_DECLARE_METATYPE(Vento::CurrencyInfo)
Q_DECLARE_METATYPE(Vento::RateHistoryEntry)
