//==============================================================================
// Product.hpp
// Vento - Estructura de datos del producto
//==============================================================================
#pragma once

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QVariantMap>
#include <cmath>

#include "InventoryTypes.hpp"

namespace Vento {

//==============================================================================
// Product - Estructura principal del producto
//==============================================================================
struct Product {
    // Identificador
    qint64 id = 0;
    
    // Información básica
    QString name;
    QString description;
    QString sku;
    QString barcode;
    QString category;
    
    // Precios y costos
    double costUsd = 0.0;
    double costLocal = 0.0;
    double marginPercent = Inventory::Defaults::DEFAULT_MARGIN_PERCENT;
    double salePrice = 0.0;
    
    // Inventario
    int stockQuantity = Inventory::Defaults::DEFAULT_STOCK_QUANTITY;
    int minStockAlert = Inventory::Defaults::DEFAULT_MIN_STOCK_ALERT;
    
    // Estado
    bool isActive = true;
    
    // Timestamps
    QDateTime createdAt;
    QDateTime updatedAt;
    
    //==========================================================================
    // Cálculos
    //==========================================================================
    
    /// Calcula el precio de venta basado en costo USD, tasa y margen
    void calculateSalePrice(double exchangeRate) {
        costLocal = costUsd * exchangeRate;
        salePrice = costLocal * (1.0 + marginPercent / 100.0);
    }
    
    /// Calcula el costo local basado en costo USD y tasa
    void calculateCostLocal(double exchangeRate) {
        costLocal = costUsd * exchangeRate;
    }
    
    /// Obtiene la ganancia en moneda local
    [[nodiscard]] double profitAmount() const {
        return salePrice - costLocal;
    }
    
    /// Obtiene el porcentaje de ganancia real
    [[nodiscard]] double profitPercent() const {
        if (costLocal <= 0) return 0.0;
        return ((salePrice - costLocal) / costLocal) * 100.0;
    }
    
    /// Verifica si el stock está bajo
    [[nodiscard]] bool isLowStock() const {
        return stockQuantity > 0 && stockQuantity <= minStockAlert;
    }
    
    /// Verifica si está agotado
    [[nodiscard]] bool isOutOfStock() const {
        return stockQuantity <= 0;
    }
    
    /// Obtiene el estado del stock como string
    [[nodiscard]] QString stockStatus() const {
        if (isOutOfStock()) return "out";
        if (isLowStock()) return "low";
        return "ok";
    }
    
    //==========================================================================
    // Validación
    //==========================================================================
    
    [[nodiscard]] bool isValid() const {
        return !name.isEmpty() && 
               costUsd >= 0 && 
               marginPercent >= 0 &&
               stockQuantity >= 0;
    }
    
    [[nodiscard]] QString validationError() const {
        if (name.isEmpty()) return "El nombre es requerido";
        if (costUsd < 0) return "El costo USD no puede ser negativo";
        if (marginPercent < 0) return "El margen no puede ser negativo";
        if (stockQuantity < 0) return "El stock no puede ser negativo";
        return {};
    }
    
    //==========================================================================
    // Serialización
    //==========================================================================
    
    [[nodiscard]] QJsonObject toJson() const {
        return {
            {"id", id},
            {"name", name},
            {"description", description},
            {"sku", sku},
            {"barcode", barcode},
            {"category", category},
            {"costUsd", costUsd},
            {"costLocal", costLocal},
            {"marginPercent", marginPercent},
            {"salePrice", salePrice},
            {"stockQuantity", stockQuantity},
            {"minStockAlert", minStockAlert},
            {"isActive", isActive},
            {"createdAt", createdAt.toString(Qt::ISODate)},
            {"updatedAt", updatedAt.toString(Qt::ISODate)}
        };
    }
    
    [[nodiscard]] QVariantMap toVariantMap() const {
        return {
            {"id", id},
            {"name", name},
            {"description", description},
            {"sku", sku},
            {"barcode", barcode},
            {"category", category},
            {"costUsd", costUsd},
            {"costLocal", costLocal},
            {"marginPercent", marginPercent},
            {"salePrice", salePrice},
            {"stockQuantity", stockQuantity},
            {"minStockAlert", minStockAlert},
            {"isActive", isActive},
            {"createdAt", createdAt},
            {"updatedAt", updatedAt}
        };
    }
    
    static Product fromJson(const QJsonObject& json) {
        Product p;
        p.id = json["id"].toVariant().toLongLong();
        p.name = json["name"].toString();
        p.description = json["description"].toString();
        p.sku = json["sku"].toString();
        p.barcode = json["barcode"].toString();
        p.category = json["category"].toString();
        p.costUsd = json["costUsd"].toDouble();
        p.costLocal = json["costLocal"].toDouble();
        p.marginPercent = json["marginPercent"].toDouble();
        p.salePrice = json["salePrice"].toDouble();
        p.stockQuantity = json["stockQuantity"].toInt();
        p.minStockAlert = json["minStockAlert"].toInt();
        p.isActive = json["isActive"].toBool(true);
        p.createdAt = QDateTime::fromString(json["createdAt"].toString(), Qt::ISODate);
        p.updatedAt = QDateTime::fromString(json["updatedAt"].toString(), Qt::ISODate);
        return p;
    }
    
    static Product fromVariantMap(const QVariantMap& map) {
        Product p;
        p.id = map["id"].toLongLong();
        p.name = map["name"].toString();
        p.description = map["description"].toString();
        p.sku = map["sku"].toString();
        p.barcode = map["barcode"].toString();
        p.category = map["category"].toString();
        p.costUsd = map["costUsd"].toDouble();
        p.costLocal = map["costLocal"].toDouble();
        p.marginPercent = map["marginPercent"].toDouble();
        p.salePrice = map["salePrice"].toDouble();
        p.stockQuantity = map["stockQuantity"].toInt();
        p.minStockAlert = map["minStockAlert"].toInt();
        p.isActive = map["isActive"].toBool();
        p.createdAt = map["createdAt"].toDateTime();
        p.updatedAt = map["updatedAt"].toDateTime();
        return p;
    }
};

} // namespace Vento
