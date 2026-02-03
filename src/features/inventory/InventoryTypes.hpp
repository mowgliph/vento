//==============================================================================
// InventoryTypes.hpp
// Vento - Tipos y constantes para el módulo Inventory
//==============================================================================
#pragma once

#include <QString>
#include <QObject>

namespace Vento::Inventory {

//==============================================================================
// Constantes del módulo
//==============================================================================
namespace Defaults {
    constexpr double DEFAULT_MARGIN_PERCENT = 30.0;
    constexpr double MIN_MARGIN_PERCENT = 0.0;
    constexpr double MAX_MARGIN_PERCENT = 500.0;
    constexpr int DEFAULT_MIN_STOCK_ALERT = 5;
    constexpr int DEFAULT_STOCK_QUANTITY = 0;
}

//==============================================================================
// Roles para QAbstractListModel
//==============================================================================
namespace Roles {
    enum ProductRoles {
        IdRole = Qt::UserRole + 1,
        NameRole,
        DescriptionRole,
        SkuRole,
        BarcodeRole,
        CategoryRole,
        CostUsdRole,
        CostLocalRole,
        MarginPercentRole,
        SalePriceRole,
        StockQuantityRole,
        MinStockAlertRole,
        IsActiveRole,
        CreatedAtRole,
        UpdatedAtRole,
        // Roles calculados
        ProfitAmountRole,
        ProfitPercentRole,
        IsLowStockRole,
        FormattedCostUsdRole,
        FormattedCostLocalRole,
        FormattedSalePriceRole,
        StockStatusRole
    };
}

//==============================================================================
// Filtros de productos
//==============================================================================
enum class ProductFilter {
    All,
    Active,
    Inactive,
    LowStock,
    OutOfStock,
    Category
};

//==============================================================================
// Ordenamiento
//==============================================================================
enum class ProductSortField {
    Name,
    Category,
    CostUsd,
    SalePrice,
    Stock,
    CreatedAt,
    UpdatedAt
};

enum class SortOrder {
    Ascending,
    Descending
};

//==============================================================================
// Resultado de operación
//==============================================================================
struct InventoryResult {
    bool success = false;
    QString errorMessage;
    qint64 affectedId = -1;
    int affectedCount = 0;
    
    explicit operator bool() const { return success; }
    
    static InventoryResult ok(qint64 id = -1, int count = 1) {
        return {true, {}, id, count};
    }
    
    static InventoryResult error(const QString& message) {
        return {false, message, -1, 0};
    }
};

} // namespace Vento::Inventory

Q_DECLARE_METATYPE(Vento::Inventory::ProductFilter)
Q_DECLARE_METATYPE(Vento::Inventory::ProductSortField)
