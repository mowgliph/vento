//==============================================================================
// ReportingRepository.cpp
// Vento - Implementación del repositorio de reportes
//==============================================================================

#include "ReportingRepository.hpp"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

namespace Vento::Reporting {

//==============================================================================
// Constructor
//==============================================================================
ReportingRepository::ReportingRepository(QSqlDatabase& database) 
    : m_database(database) {
}

//==============================================================================
// Obtener ventas agrupadas por día y producto
//==============================================================================
QList<SalesReportData> ReportingRepository::getSalesByDayAndProduct(const ReportFilters& filters) {
    QList<SalesReportData> results;
    
    if (!isValidDateRange(filters.startDate, filters.endDate)) {
        qWarning() << "Rango de fechas inválido";
        return results;
    }
    
    QSqlQuery query(m_database);
    QString sql = R"(
        SELECT 
            p.id as product_id,
            p.name as product_name,
            p.code as product_code,
            DATE(s.created_at) as sale_date,
            SUM(si.quantity) as total_quantity,
            SUM(si.total_price) as total_sales,
            AVG(si.unit_price) as avg_price
        FROM sales s
        INNER JOIN sale_items si ON s.id = si.sale_id
        INNER JOIN products p ON si.product_id = p.id
        WHERE 1=1
    )";
    
    sql += buildDateFilter(filters);
    sql += buildProductFilter(filters);
    
    if (!filters.includeCancelled) {
        sql += " AND s.status != 'cancelled'";
    }
    
    sql += R"(
        GROUP BY p.id, p.name, p.code, DATE(s.created_at)
        ORDER BY sale_date DESC, total_sales DESC
    )";
    
    if (!query.prepare(sql)) {
        qWarning() << "Error preparando consulta:" << query.lastError().text();
        return results;
    }
    
    // Bind parameters
    query.bindValue(":startDate", filters.startDate.toString("yyyy-MM-dd"));
    query.bindValue(":endDate", filters.endDate.toString("yyyy-MM-dd"));
    
    if (filters.productId > 0) {
        query.bindValue(":productId", filters.productId);
    }
    
    if (!query.exec()) {
        qWarning() << "Error ejecutando consulta:" << query.lastError().text();
        return results;
    }
    
    while (query.next()) {
        SalesReportData data;
        data.productId = query.value("product_id").toLongLong();
        data.productName = query.value("product_name").toString();
        data.productCode = query.value("product_code").toString();
        data.date = QDate::fromString(query.value("sale_date").toString(), "yyyy-MM-dd");
        data.quantitySold = query.value("total_quantity").toInt();
        data.totalSales = query.value("total_sales").toDouble();
        data.averagePrice = query.value("avg_price").toDouble();
        
        results.append(data);
    }
    
    return results;
}

//==============================================================================
// Obtener resumen de ventas
//==============================================================================
QVariantMap ReportingRepository::getSalesSummary(const ReportFilters& filters) {
    QVariantMap summary;
    
    if (!isValidDateRange(filters.startDate, filters.endDate)) {
        return summary;
    }
    
    QSqlQuery query(m_database);
    QString sql = R"(
        SELECT 
            COUNT(DISTINCT s.id) as total_sales,
            SUM(si.total_price) as total_revenue,
            SUM(si.quantity) as total_items,
            AVG(si.total_price) as avg_sale_amount,
            MIN(s.created_at) as first_sale,
            MAX(s.created_at) as last_sale
        FROM sales s
        INNER JOIN sale_items si ON s.id = si.sale_id
        WHERE 1=1
    )";
    
    sql += buildDateFilter(filters);
    sql += buildProductFilter(filters);
    
    if (!filters.includeCancelled) {
        sql += " AND s.status != 'cancelled'";
    }
    
    if (!query.prepare(sql)) {
        qWarning() << "Error preparando resumen:" << query.lastError().text();
        return summary;
    }
    
    query.bindValue(":startDate", filters.startDate.toString("yyyy-MM-dd"));
    query.bindValue(":endDate", filters.endDate.toString("yyyy-MM-dd"));
    
    if (filters.productId > 0) {
        query.bindValue(":productId", filters.productId);
    }
    
    if (!query.exec()) {
        qWarning() << "Error ejecutando resumen:" << query.lastError().text();
        return summary;
    }
    
    if (query.next()) {
        summary["totalSales"] = query.value("total_sales").toInt();
        summary["totalRevenue"] = query.value("total_revenue").toDouble();
        summary["totalItems"] = query.value("total_items").toInt();
        summary["avgSaleAmount"] = query.value("avg_sale_amount").toDouble();
        summary["firstSale"] = query.value("first_sale").toString();
        summary["lastSale"] = query.value("last_sale").toString();
    }
    
    return summary;
}

//==============================================================================
// Obtener productos más vendidos
//==============================================================================
QList<QVariantMap> ReportingRepository::getTopSellingProducts(const ReportFilters& filters, int limit) {
    QList<QVariantMap> results;
    
    if (!isValidDateRange(filters.startDate, filters.endDate)) {
        return results;
    }
    
    QSqlQuery query(m_database);
    QString sql = R"(
        SELECT 
            p.id as product_id,
            p.name as product_name,
            p.code as product_code,
            SUM(si.quantity) as total_quantity,
            SUM(si.total_price) as total_revenue,
            COUNT(DISTINCT s.id) as sales_count
        FROM sales s
        INNER JOIN sale_items si ON s.id = si.sale_id
        INNER JOIN products p ON si.product_id = p.id
        WHERE 1=1
    )";
    
    sql += buildDateFilter(filters);
    sql += buildProductFilter(filters);
    
    if (!filters.includeCancelled) {
        sql += " AND s.status != 'cancelled'";
    }
    
    sql += R"(
        GROUP BY p.id, p.name, p.code
        ORDER BY total_quantity DESC
        LIMIT :limit
    )";
    
    if (!query.prepare(sql)) {
        qWarning() << "Error preparando top productos:" << query.lastError().text();
        return results;
    }
    
    query.bindValue(":startDate", filters.startDate.toString("yyyy-MM-dd"));
    query.bindValue(":endDate", filters.endDate.toString("yyyy-MM-dd"));
    query.bindValue(":limit", limit);
    
    if (filters.productId > 0) {
        query.bindValue(":productId", filters.productId);
    }
    
    if (!query.exec()) {
        qWarning() << "Error ejecutando top productos:" << query.lastError().text();
        return results;
    }
    
    while (query.next()) {
        QVariantMap product;
        product["productId"] = query.value("product_id").toLongLong();
        product["productName"] = query.value("product_name").toString();
        product["productCode"] = query.value("product_code").toString();
        product["totalQuantity"] = query.value("total_quantity").toInt();
        product["totalRevenue"] = query.value("total_revenue").toDouble();
        product["salesCount"] = query.value("sales_count").toInt();
        
        results.append(product);
    }
    
    return results;
}

//==============================================================================
// Obtener totales diarios de ventas
//==============================================================================
QList<QVariantMap> ReportingRepository::getDailySalesTotals(const ReportFilters& filters) {
    QList<QVariantMap> results;
    
    if (!isValidDateRange(filters.startDate, filters.endDate)) {
        return results;
    }
    
    QSqlQuery query(m_database);
    QString sql = R"(
        SELECT 
            DATE(s.created_at) as sale_date,
            COUNT(DISTINCT s.id) as sales_count,
            SUM(si.total_price) as total_revenue,
            SUM(si.quantity) as total_items,
            AVG(si.total_price) as avg_sale_amount
        FROM sales s
        INNER JOIN sale_items si ON s.id = si.sale_id
        WHERE 1=1
    )";
    
    sql += buildDateFilter(filters);
    sql += buildProductFilter(filters);
    
    if (!filters.includeCancelled) {
        sql += " AND s.status != 'cancelled'";
    }
    
    sql += R"(
        GROUP BY DATE(s.created_at)
        ORDER BY sale_date DESC
    )";
    
    if (!query.prepare(sql)) {
        qWarning() << "Error preparando totales diarios:" << query.lastError().text();
        return results;
    }
    
    query.bindValue(":startDate", filters.startDate.toString("yyyy-MM-dd"));
    query.bindValue(":endDate", filters.endDate.toString("yyyy-MM-dd"));
    
    if (filters.productId > 0) {
        query.bindValue(":productId", filters.productId);
    }
    
    if (!query.exec()) {
        qWarning() << "Error ejecutando totales diarios:" << query.lastError().text();
        return results;
    }
    
    while (query.next()) {
        QVariantMap day;
        day["date"] = query.value("sale_date").toString();
        day["salesCount"] = query.value("sales_count").toInt();
        day["totalRevenue"] = query.value("total_revenue").toDouble();
        day["totalItems"] = query.value("total_items").toInt();
        day["avgSaleAmount"] = query.value("avg_sale_amount").toDouble();
        
        results.append(day);
    }
    
    return results;
}

//==============================================================================
// Obtener estadísticas generales
//==============================================================================
QVariantMap ReportingRepository::getGeneralStatistics() {
    QVariantMap stats;
    
    QSqlQuery query(m_database);
    
    // Total de productos
    query.exec("SELECT COUNT(*) as count FROM products WHERE active = 1");
    if (query.next()) {
        stats["totalProducts"] = query.value("count").toInt();
    }
    
    // Total de ventas
    query.exec("SELECT COUNT(*) as count FROM sales WHERE status != 'cancelled'");
    if (query.next()) {
        stats["totalSales"] = query.value("count").toInt();
    }
    
    // Ingresos totales
    query.prepare(R"(
        SELECT SUM(si.total_price) as total 
        FROM sales s 
        INNER JOIN sale_items si ON s.id = si.sale_id 
        WHERE s.status != 'cancelled'
    )");
    if (query.exec() && query.next()) {
        stats["totalRevenue"] = query.value("total").toDouble();
    }
    
    // Ventas hoy
    query.prepare(R"(
        SELECT COUNT(*) as count 
        FROM sales 
        WHERE DATE(created_at) = DATE('now') AND status != 'cancelled'
    )");
    if (query.exec() && query.next()) {
        stats["todaySales"] = query.value("count").toInt();
    }
    
    return stats;
}

//==============================================================================
// Validar rango de fechas
//==============================================================================
bool ReportingRepository::isValidDateRange(const QDate& startDate, const QDate& endDate) {
    return startDate.isValid() && endDate.isValid() && startDate <= endDate;
}

//==============================================================================
// Obtener categorías de productos
//==============================================================================
QStringList ReportingRepository::getProductCategories() {
    QStringList categories;
    
    QSqlQuery query("SELECT DISTINCT category FROM products WHERE category IS NOT NULL AND category != '' ORDER BY category");
    while (query.next()) {
        categories.append(query.value("category").toString());
    }
    
    return categories;
}

//==============================================================================
// Obtener información de producto
//==============================================================================
QVariantMap ReportingRepository::getProductInfo(qint64 productId) {
    QVariantMap info;
    
    if (productId <= 0) {
        return info;
    }
    
    QSqlQuery query(m_database);
    query.prepare("SELECT id, name, code, category, price FROM products WHERE id = :id");
    query.bindValue(":id", productId);
    
    if (query.exec() && query.next()) {
        info["id"] = query.value("id").toLongLong();
        info["name"] = query.value("name").toString();
        info["code"] = query.value("code").toString();
        info["category"] = query.value("category").toString();
        info["price"] = query.value("price").toDouble();
    }
    
    return info;
}

//==============================================================================
// Construir filtro de fecha
//==============================================================================
QString ReportingRepository::buildDateFilter(const ReportFilters& filters) {
    return " AND DATE(s.created_at) BETWEEN :startDate AND :endDate ";
}

//==============================================================================
// Construir filtro de producto
//==============================================================================
QString ReportingRepository::buildProductFilter(const ReportFilters& filters) {
    QString filter;
    
    if (filters.productId > 0) {
        filter += " AND si.product_id = :productId ";
    }
    
    if (!filters.category.isEmpty()) {
        filter += QString(" AND p.category = '%1' ").arg(filters.category);
    }
    
    return filter;
}

} // namespace Vento::Reporting
