//==============================================================================
// Sale.hpp
// Vento - Estructura de venta completa
//==============================================================================
#pragma once

#include <QString>
#include <QDateTime>
#include <QVariantMap>
#include <vector>

#include "SalesTypes.hpp"
#include "SaleItem.hpp"

namespace Vento {

//==============================================================================
// Sale - Venta completa con items
//==============================================================================
struct Sale {
    qint64 id = 0;
    QDateTime saleDate;
    
    // Totales
    double subtotal = 0.0;
    double taxPercent = Sales::Defaults::DEFAULT_TAX_PERCENT;
    double taxAmount = 0.0;
    double discountAmount = 0.0;
    double total = 0.0;
    
    // Moneda
    double exchangeRate = 1.0;
    QString currencyCode = "VES";
    
    // Pago
    Sales::PaymentMethod paymentMethod = Sales::PaymentMethod::Cash;
    
    // Cliente (opcional)
    QString customerName;
    QString customerPhone;
    QString customerId;
    
    // Estado
    Sales::SaleStatus status = Sales::SaleStatus::Completed;
    
    // Metadata
    QString notes;
    QString cashierName;
    QString receiptNumber;
    
    QDateTime createdAt;
    QDateTime updatedAt;
    
    // Items de la venta
    std::vector<SaleItem> items;
    
    //==========================================================================
    // Cálculos
    //==========================================================================
    
    void calculateTotals() {
        subtotal = 0.0;
        for (const auto& item : items) {
            subtotal += item.subtotal;
        }
        
        taxAmount = subtotal * (taxPercent / 100.0);
        total = subtotal + taxAmount - discountAmount;
        
        if (total < 0.0) total = 0.0;
    }
    
    double getTotalInCurrency(const QString& targetCurrency) const {
        if (targetCurrency == currencyCode) {
            return total;
        }
        return total / exchangeRate;
    }
    
    //==========================================================================
    // Utilidades
    //==========================================================================
    
    int itemCount() const {
        return static_cast<int>(items.size());
    }
    
    int totalQuantity() const {
        int qty = 0;
        for (const auto& item : items) {
            qty += item.quantity;
        }
        return qty;
    }
    
    bool isValid() const {
        return !items.empty() && total >= Sales::Defaults::MIN_SALE_AMOUNT;
    }
    
    bool canBeCancelled() const {
        return status == Sales::SaleStatus::Completed ||
               status == Sales::SaleStatus::Pending;
    }
    
    bool canBeRefunded() const {
        return status == Sales::SaleStatus::Completed;
    }
    
    //==========================================================================
    // Formateo
    //==========================================================================
    
    QString formattedTotal() const {
        return QString::number(total, 'f', 2);
    }
    
    QString formattedDate() const {
        return saleDate.toString("dd/MM/yyyy");
    }
    
    QString formattedTime() const {
        return saleDate.toString("hh:mm AP");
    }
    
    QString formattedDateTime() const {
        return saleDate.toString("dd/MM/yyyy hh:mm AP");
    }
    
    //==========================================================================
    // Conversiones
    //==========================================================================
    
    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["id"] = id;
        map["saleDate"] = saleDate;
        map["subtotal"] = subtotal;
        map["taxPercent"] = taxPercent;
        map["taxAmount"] = taxAmount;
        map["discountAmount"] = discountAmount;
        map["total"] = total;
        map["exchangeRate"] = exchangeRate;
        map["currencyCode"] = currencyCode;
        map["paymentMethod"] = Sales::paymentToString(paymentMethod);
        map["customerName"] = customerName;
        map["customerPhone"] = customerPhone;
        map["customerId"] = customerId;
        map["status"] = Sales::statusToString(status);
        map["notes"] = notes;
        map["cashierName"] = cashierName;
        map["receiptNumber"] = receiptNumber;
        map["createdAt"] = createdAt;
        map["updatedAt"] = updatedAt;
        map["itemCount"] = itemCount();
        map["totalQuantity"] = totalQuantity();
        
        QVariantList itemsList;
        for (const auto& item : items) {
            itemsList.append(item.toVariantMap());
        }
        map["items"] = itemsList;
        
        return map;
    }
    
    static Sale fromVariantMap(const QVariantMap& map) {
        Sale sale;
        sale.id = map.value("id", 0).toLongLong();
        sale.saleDate = map.value("saleDate").toDateTime();
        sale.subtotal = map.value("subtotal", 0.0).toDouble();
        sale.taxPercent = map.value("taxPercent", Sales::Defaults::DEFAULT_TAX_PERCENT).toDouble();
        sale.taxAmount = map.value("taxAmount", 0.0).toDouble();
        sale.discountAmount = map.value("discountAmount", 0.0).toDouble();
        sale.total = map.value("total", 0.0).toDouble();
        sale.exchangeRate = map.value("exchangeRate", 1.0).toDouble();
        sale.currencyCode = map.value("currencyCode", "VES").toString();
        sale.paymentMethod = Sales::stringToPayment(map.value("paymentMethod").toString());
        sale.customerName = map.value("customerName").toString();
        sale.customerPhone = map.value("customerPhone").toString();
        sale.customerId = map.value("customerId").toString();
        sale.status = Sales::stringToStatus(map.value("status").toString());
        sale.notes = map.value("notes").toString();
        sale.cashierName = map.value("cashierName").toString();
        sale.receiptNumber = map.value("receiptNumber").toString();
        sale.createdAt = map.value("createdAt").toDateTime();
        sale.updatedAt = map.value("updatedAt").toDateTime();
        
        QVariantList itemsList = map.value("items").toList();
        for (const auto& itemVar : itemsList) {
            sale.items.push_back(SaleItem::fromVariantMap(itemVar.toMap()));
        }
        
        return sale;
    }
};

} // namespace Vento

Q_DECLARE_METATYPE(Vento::Sale)
