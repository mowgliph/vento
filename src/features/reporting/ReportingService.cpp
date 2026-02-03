//==============================================================================
// ReportingService.cpp
// Vento - Implementación del servicio de reportes
//==============================================================================

#include "ReportingService.hpp"
#include "core/database/DatabaseManager.hpp"
#include "core/logger/Logger.hpp"

#include <QDir>
#include <QStandardPaths>
#include <QTextStream>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextDocument>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>

namespace Vento::Reporting {

//==============================================================================
// Inicialización del servicio
//==============================================================================
void ReportingService::initialize() {
    auto& db = Vento::DatabaseManager::instance();
    m_repository = std::make_unique<ReportingRepository>(db.database());
    
    Vento::Logger::instance().info("✓ ReportingService inicializado correctamente");
}

//==============================================================================
// Obtener instancia singleton
//==============================================================================
ReportingService& ReportingService::instance() {
    static ReportingService instance;
    return instance;
}

//==============================================================================
// Generar reporte de ventas
//==============================================================================
QVariantList ReportingService::generateSalesReport(const QDate& startDate, const QDate& endDate, qint64 productId) {
    setGenerating(true);
    setProgress(0);
    
    try {
        auto filters = buildFilters(startDate, endDate, productId);
        if (!filters.isValid()) {
            setError("Rango de fechas inválido");
            setGenerating(false);
            return {};
        }
        
        setProgress(25);
        
        auto data = m_repository->getSalesByDayAndProduct(filters);
        setProgress(75);
        
        auto result = convertSalesReportData(data);
        
        setProgress(100);
        emit reportGenerated("sales", result.size());
        
        setGenerating(false);
        return result;
        
    } catch (const std::exception& e) {
        setError(QString("Error generando reporte: %1").arg(e.what()));
        setGenerating(false);
        return {};
    }
}

//==============================================================================
// Obtener resumen de ventas
//==============================================================================
QVariantMap ReportingService::getSalesSummary(const QDate& startDate, const QDate& endDate, qint64 productId) {
    try {
        auto filters = buildFilters(startDate, endDate, productId);
        if (!filters.isValid()) {
            setError("Rango de fechas inválido");
            return {};
        }
        
        return m_repository->getSalesSummary(filters);
        
    } catch (const std::exception& e) {
        setError(QString("Error obteniendo resumen: %1").arg(e.what()));
        return {};
    }
}

//==============================================================================
// Obtener productos más vendidos
//==============================================================================
QVariantList ReportingService::getTopSellingProducts(const QDate& startDate, const QDate& endDate, int limit) {
    try {
        auto filters = buildFilters(startDate, endDate);
        if (!filters.isValid()) {
            setError("Rango de fechas inválido");
            return {};
        }
        
        auto data = m_repository->getTopSellingProducts(filters, limit);
        return QVariantList::fromList(data);
        
    } catch (const std::exception& e) {
        setError(QString("Error obteniendo productos: %1").arg(e.what()));
        return {};
    }
}

//==============================================================================
// Obtener totales diarios de ventas
//==============================================================================
QVariantList ReportingService::getDailySalesTotals(const QDate& startDate, const QDate& endDate) {
    try {
        auto filters = buildFilters(startDate, endDate);
        if (!filters.isValid()) {
            setError("Rango de fechas inválido");
            return {};
        }
        
        auto data = m_repository->getDailySalesTotals(filters);
        return QVariantList::fromList(data);
        
    } catch (const std::exception& e) {
        setError(QString("Error obteniendo totales diarios: %1").arg(e.what()));
        return {};
    }
}

//==============================================================================
// Obtener estadísticas generales
//==============================================================================
QVariantMap ReportingService::getGeneralStatistics() {
    try {
        return m_repository->getGeneralStatistics();
    } catch (const std::exception& e) {
        setError(QString("Error obteniendo estadísticas: %1").arg(e.what()));
        return {};
    }
}

//==============================================================================
// Exportar a Excel (CSV como alternativa simple)
//==============================================================================
bool ReportingService::exportToExcel(const QVariantList& data, const QString& fileName) {
    try {
        QString finalFileName = fileName;
        if (finalFileName.isEmpty()) {
            finalFileName = getDefaultReportFileName("sales", "csv");
        }
        
        if (!finalFileName.endsWith(".csv", Qt::CaseInsensitive)) {
            finalFileName += ".csv";
        }
        
        bool success = writeCSVFile(finalFileName, data);
        
        if (success) {
            emit exportCompleted(finalFileName, "Excel (CSV)");
        } else {
            emit exportFailed(finalFileName, "Error escribiendo archivo CSV");
        }
        
        return success;
        
    } catch (const std::exception& e) {
        setError(QString("Error exportando a Excel: %1").arg(e.what()));
        emit exportFailed(fileName, e.what());
        return false;
    }
}

//==============================================================================
// Exportar a PDF
//==============================================================================
bool ReportingService::exportToPDF(const QVariantList& data, const QString& fileName) {
    try {
        QString finalFileName = fileName;
        if (finalFileName.isEmpty()) {
            finalFileName = getDefaultReportFileName("sales", "pdf");
        }
        
        if (!finalFileName.endsWith(".pdf", Qt::CaseInsensitive)) {
            finalFileName += ".pdf";
        }
        
        bool success = writePDFFile(finalFileName, data);
        
        if (success) {
            emit exportCompleted(finalFileName, "PDF");
        } else {
            emit exportFailed(finalFileName, "Error escribiendo archivo PDF");
        }
        
        return success;
        
    } catch (const std::exception& e) {
        setError(QString("Error exportando a PDF: %1").arg(e.what()));
        emit exportFailed(fileName, e.what());
        return false;
    }
}

//==============================================================================
// Exportar a CSV
//==============================================================================
bool ReportingService::exportToCSV(const QVariantList& data, const QString& fileName) {
    try {
        QString finalFileName = fileName;
        if (finalFileName.isEmpty()) {
            finalFileName = getDefaultReportFileName("sales", "csv");
        }
        
        if (!finalFileName.endsWith(".csv", Qt::CaseInsensitive)) {
            finalFileName += ".csv";
        }
        
        bool success = writeCSVFile(finalFileName, data);
        
        if (success) {
            emit exportCompleted(finalFileName, "CSV");
        } else {
            emit exportFailed(finalFileName, "Error escribiendo archivo CSV");
        }
        
        return success;
        
    } catch (const std::exception& e) {
        setError(QString("Error exportando a CSV: %1").arg(e.what()));
        emit exportFailed(fileName, e.what());
        return false;
    }
}

//==============================================================================
// Imprimir reporte
//==============================================================================
bool ReportingService::printReport(const QVariantList& data) {
    try {
        printData(data);
        return true;
    } catch (const std::exception& e) {
        setError(QString("Error imprimiendo reporte: %1").arg(e.what()));
        return false;
    }
}

//==============================================================================
// Obtener categorías de productos
//==============================================================================
QStringList ReportingService::getProductCategories() {
    try {
        return m_repository->getProductCategories();
    } catch (const std::exception& e) {
        setError(QString("Error obteniendo categorías: %1").arg(e.what()));
        return {};
    }
}

//==============================================================================
// Obtener información de producto
//==============================================================================
QVariantMap ReportingService::getProductInfo(qint64 productId) {
    try {
        return m_repository->getProductInfo(productId);
    } catch (const std::exception& e) {
        setError(QString("Error obteniendo info del producto: %1").arg(e.what()));
        return {};
    }
}

//==============================================================================
// Obtener nombre de archivo por defecto
//==============================================================================
QString ReportingService::getDefaultReportFileName(const QString& reportType, const QString& format) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString reportsDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Vento/Reportes";
    
    QDir dir;
    if (!dir.exists(reportsDir)) {
        dir.mkpath(reportsDir);
    }
    
    return QString("%1/%2_%3.%4").arg(reportsDir, reportType, timestamp, format);
}

//==============================================================================
// Métodos privados - Setters
//==============================================================================
void ReportingService::setGenerating(bool generating) {
    if (m_generating != generating) {
        m_generating = generating;
        emit generatingChanged();
    }
}

void ReportingService::setError(const QString& error) {
    if (m_lastError != error) {
        m_lastError = error;
        emit errorOccurred(error);
        
        if (!error.isEmpty()) {
            Vento::Logger::instance().error(QString("ReportingService Error: %1").arg(error));
        }
    }
}

void ReportingService::setProgress(int progress) {
    if (m_currentProgress != progress) {
        m_currentProgress = progress;
        emit progressChanged();
    }
}

//==============================================================================
// Construir filtros
//==============================================================================
ReportFilters ReportingService::buildFilters(const QDate& startDate, const QDate& endDate, qint64 productId) {
    ReportFilters filters;
    filters.startDate = startDate;
    filters.endDate = endDate;
    filters.productId = productId;
    filters.includeCancelled = false;
    
    return filters;
}

//==============================================================================
// Convertir datos de reporte de ventas
//==============================================================================
QVariantList ReportingService::convertSalesReportData(const QList<SalesReportData>& data) {
    QVariantList result;
    
    for (const auto& item : data) {
        result.append(item.toMap());
    }
    
    return result;
}

//==============================================================================
// Escribir archivo CSV
//==============================================================================
bool ReportingService::writeCSVFile(const QString& fileName, const QVariantList& data) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        setError(QString("No se puede abrir el archivo: %1").arg(fileName));
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    // Escribir encabezados
    out << "Producto ID,Nombre Producto,Código,Fecha,Cantidad,Venta Total,Precio Promedio\n";
    
    // Escribir datos
    for (const auto& item : data) {
        QVariantMap map = item.toMap();
        out << map["productId"].toString() << ","
            << "\"" << map["productName"].toString() << "\"" << ","
            << map["productCode"].toString() << ","
            << map["date"].toString() << ","
            << map["quantitySold"].toString() << ","
            << map["totalSales"].toString() << ","
            << map["averagePrice"].toString() << "\n";
    }
    
    file.close();
    return true;
}

//==============================================================================
// Escribir archivo PDF
//==============================================================================
bool ReportingService::writePDFFile(const QString& fileName, const QVariantList& data) {
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(20, 20, 20, 20), QPageLayout::Millimeter);
    
    QTextDocument document;
    QString html = R"(
        <html>
        <head>
            <style>
                body { font-family: Arial, sans-serif; margin: 20px; }
                h1 { color: #005A9E; text-align: center; }
                table { width: 100%; border-collapse: collapse; margin-top: 20px; }
                th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
                th { background-color: #f2f2f2; font-weight: bold; }
                .date { text-align: center; color: #666; margin-bottom: 20px; }
            </style>
        </head>
        <body>
            <h1>Reporte de Ventas</h1>
            <div class="date">Generado: %1</div>
            <table>
                <thead>
                    <tr>
                        <th>Producto</th>
                        <th>Código</th>
                        <th>Fecha</th>
                        <th>Cantidad</th>
                        <th>Total</th>
                        <th>Precio Promedio</th>
                    </tr>
                </thead>
                <tbody>
    )";
    
    html = html.arg(QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
    
    for (const auto& item : data) {
        QVariantMap map = item.toMap();
        html += QString(R"(
                    <tr>
                        <td>%1</td>
                        <td>%2</td>
                        <td>%3</td>
                        <td align='right'>%4</td>
                        <td align='right'>%5</td>
                        <td align='right'>%6</td>
                    </tr>
        )").arg(map["productName"].toString())
           .arg(map["productCode"].toString())
           .arg(map["date"].toString())
           .arg(map["quantitySold"].toString())
           .arg(QString::number(map["totalSales"].toDouble(), 'f', 2))
           .arg(QString::number(map["averagePrice"].toDouble(), 'f', 2));
    }
    
    html += R"(
                </tbody>
            </table>
        </body>
        </html>
    )";
    
    document.setHtml(html);
    document.print(&printer);
    
    return true;
}

//==============================================================================
// Imprimir datos
//==============================================================================
void ReportingService::printData(const QVariantList& data) {
    QPrinter printer;
    QPrintDialog dialog(&printer);
    
    if (dialog.exec() == QDialog::Accepted) {
        QTextDocument document;
        QString html = R"(
            <html>
            <head>
                <style>
                    body { font-family: Arial, sans-serif; margin: 20px; }
                    h1 { color: #005A9E; text-align: center; }
                    table { width: 100%; border-collapse: collapse; margin-top: 20px; }
                    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
                    th { background-color: #f2f2f2; font-weight: bold; }
                </style>
            </head>
            <body>
                <h1>Reporte de Ventas</h1>
                <table>
                    <thead>
                        <tr>
                            <th>Producto</th>
                            <th>Código</th>
                            <th>Fecha</th>
                            <th>Cantidad</th>
                            <th>Total</th>
                        </tr>
                    </thead>
                    <tbody>
        )";
        
        for (const auto& item : data) {
            QVariantMap map = item.toMap();
            html += QString(R"(
                        <tr>
                            <td>%1</td>
                            <td>%2</td>
                            <td>%3</td>
                            <td align='right'>%4</td>
                            <td align='right'>%5</td>
                        </tr>
            )").arg(map["productName"].toString())
               .arg(map["productCode"].toString())
               .arg(map["date"].toString())
               .arg(map["quantitySold"].toString())
               .arg(QString::number(map["totalSales"].toDouble(), 'f', 2));
        }
        
        html += R"(
                    </tbody>
                </table>
            </body>
            </html>
        )";
        
        document.setHtml(html);
        document.print(&printer);
    }
}

} // namespace Vento::Reporting
