//==============================================================================
// ReportingRepository.hpp
// Vento - Repositorio para consultas de reportes
//==============================================================================
#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QVariantList>
#include <QVariantMap>

#include "ReportingTypes.hpp"

namespace Vento::Reporting {

//==============================================================================
// ReportingRepository - Acceso a datos para reportes
//==============================================================================
class ReportingRepository {
public:
    explicit ReportingRepository(QSqlDatabase& database);
    ~ReportingRepository() = default;

    // Reportes de ventas por día y producto
    QList<SalesReportData> getSalesByDayAndProduct(const ReportFilters& filters);
    
    // Reportes agregados
    QVariantMap getSalesSummary(const ReportFilters& filters);
    QList<QVariantMap> getTopSellingProducts(const ReportFilters& filters, int limit = 10);
    QList<QVariantMap> getDailySalesTotals(const ReportFilters& filters);
    
    // Estadísticas generales
    QVariantMap getGeneralStatistics();
    
    // Métodos de utilidad
    bool isValidDateRange(const QDate& startDate, const QDate& endDate);
    QStringList getProductCategories();
    QVariantMap getProductInfo(qint64 productId);

private:
    QSqlDatabase& m_database;
    
    // Métodos helper
    SalesReportData mapRowToSalesReportData(const QVariantMap& row);
    QString buildDateFilter(const ReportFilters& filters);
    QString buildProductFilter(const ReportFilters& filters);
};

} // namespace Vento::Reporting
