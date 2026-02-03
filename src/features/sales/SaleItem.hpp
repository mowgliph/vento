//==============================================================================
// SaleItem.hpp
// Vento - Estructura de item de venta (detalle)
//==============================================================================
#pragma once

#include <QString>
#include <QVariantMap>
#include <QDateTime>

namespace Vento {

//==============================================================================
// SaleItem - Item individual de una venta
//==============================================================================
struct SaleItem {
    qint64 id = 0;
    qint64 saleId = 0;
    qint64 productId = 0;
    
    // Snapshot del producto al momento de la venta
    QString productName;
    double unitPrice = 0.0;
    int quantity = 1;
    double subtotal = 0.0;
    
    QDateTime createdAt;
    
    //==========================================================================
    // Cálculos
    //==========================================================================
    
    void calculateSubtotal() {
        subtotal = unitPrice * quantity;
    }
    
    //==========================================================================
    // Conversiones
    //==========================================================================
    
    QVariantMap toVariantMap() const {
        return {
            {"id", id},
            {"saleId", saleId},
            {"productId", productId},
            {"productName", productName},
            {"unitPrice", unitPrice},
            {"quantity", quantity},
            {"subtotal", subtotal},
            {"createdAt", createdAt}
        };
    }
    
    static SaleItem fromVariantMap(const QVariantMap& map) {
        SaleItem item;
        item.id = map.value("id", 0).toLongLong();
        item.saleId = map.value("saleId", 0).toLongLong();
        item.productId = map.value("productId", 0).toLongLong();
        item.productName = map.value("productName").toString();
        item.unitPrice = map.value("unitPrice", 0.0).toDouble();
        item.quantity = map.value("quantity", 1).toInt();
        item.subtotal = map.value("subtotal", 0.0).toDouble();
        item.createdAt = map.value("createdAt").toDateTime();
        return item;
    }
};

} // namespace Vento

Q_DECLARE_METATYPE(Vento::SaleItem)
