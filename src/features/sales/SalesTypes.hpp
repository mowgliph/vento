//==============================================================================
// SalesTypes.hpp
// Vento - Tipos y constantes para el módulo Sales
//==============================================================================
#pragma once

#include <QString>
#include <QObject>
#include <QDateTime>

namespace Vento::Sales {

//==============================================================================
// Constantes del módulo
//==============================================================================
namespace Defaults {
    constexpr double DEFAULT_TAX_PERCENT = 16.0;
    constexpr double MIN_SALE_AMOUNT = 0.01;
    constexpr int MAX_QUANTITY_PER_ITEM = 9999;
}

//==============================================================================
// Estados de venta
//==============================================================================
enum class SaleStatus {
    Pending,        // En proceso
    Completed,      // Completada
    Cancelled,      // Cancelada
    Refunded,       // Reembolsada
    PartialRefund   // Reembolso parcial
};

inline QString statusToString(SaleStatus status) {
    switch (status) {
        case SaleStatus::Pending:       return "pending";
        case SaleStatus::Completed:     return "completed";
        case SaleStatus::Cancelled:     return "cancelled";
        case SaleStatus::Refunded:      return "refunded";
        case SaleStatus::PartialRefund: return "partial_refund";
        default: return "completed";
    }
}

inline SaleStatus stringToStatus(const QString& str) {
    if (str == "pending") return SaleStatus::Pending;
    if (str == "cancelled") return SaleStatus::Cancelled;
    if (str == "refunded") return SaleStatus::Refunded;
    if (str == "partial_refund") return SaleStatus::PartialRefund;
    return SaleStatus::Completed;
}

inline QString statusDisplayName(SaleStatus status) {
    switch (status) {
        case SaleStatus::Pending:       return QObject::tr("Pendiente");
        case SaleStatus::Completed:     return QObject::tr("Completada");
        case SaleStatus::Cancelled:     return QObject::tr("Cancelada");
        case SaleStatus::Refunded:      return QObject::tr("Reembolsada");
        case SaleStatus::PartialRefund: return QObject::tr("Reembolso Parcial");
        default: return QObject::tr("Desconocido");
    }
}

//==============================================================================
// Métodos de pago
//==============================================================================
enum class PaymentMethod {
    Cash,           // Efectivo
    Card,           // Tarjeta
    Transfer,       // Transferencia
    Mobile,         // Pago móvil
    Mixed,          // Mixto
    Credit          // Crédito/Fiado
};

inline QString paymentToString(PaymentMethod method) {
    switch (method) {
        case PaymentMethod::Cash:     return "cash";
        case PaymentMethod::Card:     return "card";
        case PaymentMethod::Transfer: return "transfer";
        case PaymentMethod::Mobile:   return "mobile";
        case PaymentMethod::Mixed:    return "mixed";
        case PaymentMethod::Credit:   return "credit";
        default: return "cash";
    }
}

inline PaymentMethod stringToPayment(const QString& str) {
    if (str == "card") return PaymentMethod::Card;
    if (str == "transfer") return PaymentMethod::Transfer;
    if (str == "mobile") return PaymentMethod::Mobile;
    if (str == "mixed") return PaymentMethod::Mixed;
    if (str == "credit") return PaymentMethod::Credit;
    return PaymentMethod::Cash;
}

inline QString paymentDisplayName(PaymentMethod method) {
    switch (method) {
        case PaymentMethod::Cash:     return QObject::tr("Efectivo");
        case PaymentMethod::Card:     return QObject::tr("Tarjeta");
        case PaymentMethod::Transfer: return QObject::tr("Transferencia");
        case PaymentMethod::Mobile:   return QObject::tr("Pago Móvil");
        case PaymentMethod::Mixed:    return QObject::tr("Mixto");
        case PaymentMethod::Credit:   return QObject::tr("Crédito");
        default: return QObject::tr("Efectivo");
    }
}

//==============================================================================
// Roles para QAbstractListModel
//==============================================================================
namespace Roles {
    enum SaleRoles {
        IdRole = Qt::UserRole + 1,
        SaleDateRole,
        SubtotalRole,
        TaxPercentRole,
        TaxAmountRole,
        DiscountAmountRole,
        TotalRole,
        ExchangeRateRole,
        PaymentMethodRole,
        PaymentMethodDisplayRole,
        CustomerNameRole,
        StatusRole,
        StatusDisplayRole,
        NotesRole,
        ItemCountRole,
        FormattedTotalRole,
        FormattedDateRole,
        FormattedTimeRole
    };
    
    enum CartItemRoles {
        ProductIdRole = Qt::UserRole + 1,
        ProductNameRole,
        UnitPriceRole,
        QuantityRole,
        SubtotalRole,
        FormattedUnitPriceRole,
        FormattedSubtotalRole,
        MaxStockRole
    };
}

//==============================================================================
// Resultado de operación de venta
//==============================================================================
struct SaleResult {
    bool success = false;
    QString errorMessage;
    qint64 saleId = -1;
    double total = 0.0;
    
    explicit operator bool() const { return success; }
    
    static SaleResult ok(qint64 id, double total) {
        return {true, {}, id, total};
    }
    
    static SaleResult error(const QString& message) {
        return {false, message, -1, 0.0};
    }
};

//==============================================================================
// Filtros de ventas
//==============================================================================
enum class SaleFilter {
    All,
    Today,
    ThisWeek,
    ThisMonth,
    Completed,
    Cancelled,
    DateRange
};

} // namespace Vento::Sales

Q_DECLARE_METATYPE(Vento::Sales::SaleStatus)
Q_DECLARE_METATYPE(Vento::Sales::PaymentMethod)
Q_DECLARE_METATYPE(Vento::Sales::SaleFilter)
