//==============================================================================
// Types.hpp
// Vento - Tipos comunes del proyecto
//==============================================================================
#pragma once

#include <QString>
#include <QVariant>
#include <QDateTime>
#include <cstdint>

namespace Vento {

using Id = qint64;
using Money = double;
using Percentage = double;

namespace Business {
    constexpr Money DEFAULT_EXCHANGE_RATE = 36.50;
    constexpr Percentage DEFAULT_TAX_PERCENT = 16.0;
    constexpr Percentage DEFAULT_MARGIN_PERCENT = 30.0;
    constexpr int DEFAULT_MIN_STOCK_ALERT = 5;
}

namespace SettingsKeys {
    constexpr auto EXCHANGE_RATE = "exchange_rate_usd";
    constexpr auto CURRENCY_LOCAL = "currency_symbol_local";
    constexpr auto CURRENCY_USD = "currency_symbol_usd";
    constexpr auto TAX_PERCENT = "tax_percent";
    constexpr auto STORE_NAME = "store_name";
    constexpr auto APP_THEME = "app_theme";
}

enum class SaleStatus {
    Pending,
    Completed,
    Cancelled,
    Refunded
};

inline QString saleStatusToString(SaleStatus status) {
    switch (status) {
        case SaleStatus::Pending:   return "pending";
        case SaleStatus::Completed: return "completed";
        case SaleStatus::Cancelled: return "cancelled";
        case SaleStatus::Refunded:  return "refunded";
        default:                    return "unknown";
    }
}

inline SaleStatus stringToSaleStatus(const QString& str) {
    if (str == "pending")   return SaleStatus::Pending;
    if (str == "cancelled") return SaleStatus::Cancelled;
    if (str == "refunded")  return SaleStatus::Refunded;
    return SaleStatus::Completed;
}

enum class PaymentMethod {
    Cash,
    Card,
    Transfer,
    Mixed,
    Credit
};

inline QString paymentMethodToString(PaymentMethod method) {
    switch (method) {
        case PaymentMethod::Cash:     return "cash";
        case PaymentMethod::Card:     return "card";
        case PaymentMethod::Transfer: return "transfer";
        case PaymentMethod::Mixed:    return "mixed";
        case PaymentMethod::Credit:   return "credit";
        default:                      return "cash";
    }
}

} // namespace Vento
