//==============================================================================
// CurrencyTypes.hpp
// Vento - Tipos y constantes para el módulo Currency
//==============================================================================
#pragma once

#include <QString>
#include <QObject>

namespace Vento::Currency {

//==============================================================================
// Constantes del módulo
//==============================================================================
namespace Defaults {
    constexpr double EXCHANGE_RATE_USD = 36.50;
    constexpr double MIN_EXCHANGE_RATE = 0.01;
    constexpr double MAX_EXCHANGE_RATE = 1000000.0;
}

//==============================================================================
// Claves de configuración en BD
//==============================================================================
namespace SettingsKeys {
    constexpr auto EXCHANGE_RATE_USD = "exchange_rate_usd";
    constexpr auto CURRENCY_SYMBOL_LOCAL = "currency_symbol_local";
    constexpr auto CURRENCY_SYMBOL_USD = "currency_symbol_usd";
    constexpr auto LAST_RATE_UPDATE = "last_rate_update";
    constexpr auto RATE_UPDATE_SOURCE = "rate_update_source";
}

//==============================================================================
// Códigos de moneda
//==============================================================================
enum class CurrencyCode {
    USD,    // Dólar estadounidense
    VES,    // Bolívar venezolano
    EUR,    // Euro
    COP,    // Peso colombiano
    BRL     // Real brasileño
};

inline QString currencyCodeToString(CurrencyCode code) {
    switch (code) {
        case CurrencyCode::USD: return "USD";
        case CurrencyCode::VES: return "VES";
        case CurrencyCode::EUR: return "EUR";
        case CurrencyCode::COP: return "COP";
        case CurrencyCode::BRL: return "BRL";
        default: return "USD";
    }
}

inline QString currencySymbol(CurrencyCode code) {
    switch (code) {
        case CurrencyCode::USD: return "$";
        case CurrencyCode::VES: return "Bs.";
        case CurrencyCode::EUR: return "€";
        case CurrencyCode::COP: return "$";
        case CurrencyCode::BRL: return "R$";
        default: return "$";
    }
}

//==============================================================================
// Fuente de actualización de tasa
//==============================================================================
enum class RateUpdateSource {
    Manual,         // Ingresado manualmente
    BCV,            // Banco Central de Venezuela
    DolarToday,     // DolarToday
    Binance,        // Binance P2P
    Custom          // API personalizada
};

inline QString rateSourceToString(RateUpdateSource source) {
    switch (source) {
        case RateUpdateSource::Manual:     return "manual";
        case RateUpdateSource::BCV:        return "bcv";
        case RateUpdateSource::DolarToday: return "dolartoday";
        case RateUpdateSource::Binance:    return "binance";
        case RateUpdateSource::Custom:     return "custom";
        default: return "manual";
    }
}

} // namespace Vento::Currency
