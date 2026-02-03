//==============================================================================
// ReportingTypes.hpp
// Vento - Tipos y estructuras para el módulo de reportes
//==============================================================================
#pragma once

#include <QObject>
#include <QDate>
#include <QVariantMap>

namespace Vento::Reporting {

//==============================================================================
// Estructura para datos de reportes de ventas
//==============================================================================
struct SalesReportData {
    qint64 productId;
    QString productName;
    QString productCode;
    double totalSales;
    int quantitySold;
    QDate date;
    double averagePrice;
    
    QVariantMap toMap() const {
        QVariantMap map;
        map["productId"] = productId;
        map["productName"] = productName;
        map["productCode"] = productCode;
        map["totalSales"] = totalSales;
        map["quantitySold"] = quantitySold;
        map["date"] = date.toString("yyyy-MM-dd");
        map["averagePrice"] = averagePrice;
        return map;
    }
};

//==============================================================================
// Enumeración para tipos de reportes
//==============================================================================
enum class ReportType {
    DailySales,
    MonthlySales,
    ProductSales,
    Summary
};

//==============================================================================
// Enumeración para formatos de exportación
//==============================================================================
enum class ExportFormat {
    Excel,
    PDF,
    CSV,
    Print
};

//==============================================================================
// Estructura para filtros de reportes
//==============================================================================
struct ReportFilters {
    QDate startDate;
    QDate endDate;
    qint64 productId = 0;  // 0 = todos los productos
    QString category;
    bool includeCancelled = false;
    
    bool isValid() const {
        return startDate.isValid() && endDate.isValid() && startDate <= endDate;
    }
};

} // namespace Vento::Reporting
