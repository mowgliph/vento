//==============================================================================
// ReportingService.hpp
// Vento - Servicio principal de reportes (Controller)
//==============================================================================
#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QDate>
#include <memory>

#include "ReportingTypes.hpp"
#include "ReportingRepository.hpp"

namespace Vento::Reporting {

//==============================================================================
// ReportingService - Controller principal del módulo de reportes
//==============================================================================
class ReportingService : public QObject {
    Q_OBJECT
    
    Q_PROPERTY(bool isGenerating READ isGenerating NOTIFY generatingChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY errorOccurred)
    Q_PROPERTY(int currentProgress READ currentProgress NOTIFY progressChanged)

public:
    static ReportingService& instance();
    
    void initialize();
    
    // Estado
    [[nodiscard]] bool isGenerating() const { return m_generating; }
    [[nodiscard]] QString lastError() const { return m_lastError; }
    [[nodiscard]] int currentProgress() const { return m_currentProgress; }
    
    // Métodos QML invocables - Generación de reportes
    Q_INVOKABLE QVariantList generateSalesReport(const QDate& startDate, const QDate& endDate, qint64 productId = 0);
    Q_INVOKABLE QVariantMap getSalesSummary(const QDate& startDate, const QDate& endDate, qint64 productId = 0);
    Q_INVOKABLE QVariantList getTopSellingProducts(const QDate& startDate, const QDate& endDate, int limit = 10);
    Q_INVOKABLE QVariantList getDailySalesTotals(const QDate& startDate, const QDate& endDate);
    Q_INVOKABLE QVariantMap getGeneralStatistics();
    
    // Métodos QML invocables - Exportación
    Q_INVOKABLE bool exportToExcel(const QVariantList& data, const QString& fileName);
    Q_INVOKABLE bool exportToPDF(const QVariantList& data, const QString& fileName);
    Q_INVOKABLE bool exportToCSV(const QVariantList& data, const QString& fileName);
    Q_INVOKABLE bool printReport(const QVariantList& data);
    
    // Métodos QML invocables - Utilidades
    Q_INVOKABLE QStringList getProductCategories();
    Q_INVOKABLE QVariantMap getProductInfo(qint64 productId);
    Q_INVOKABLE QString getDefaultReportFileName(const QString& reportType, const QString& format);
    
signals:
    void generatingChanged();
    void errorOccurred(const QString& error);
    void progressChanged();
    void reportGenerated(const QString& reportType, int recordCount);
    void exportCompleted(const QString& fileName, const QString& format);
    void exportFailed(const QString& fileName, const QString& error);

private:
    ReportingService() = default;
    ~ReportingService() override = default;
    
    ReportingService(const ReportingService&) = delete;
    ReportingService& operator=(const ReportingService&) = delete;
    
    std::unique_ptr<ReportingRepository> m_repository;
    
    bool m_generating = false;
    QString m_lastError;
    int m_currentProgress = 0;
    
    // Métodos privados
    void setGenerating(bool generating);
    void setError(const QString& error);
    void setProgress(int progress);
    
    ReportFilters buildFilters(const QDate& startDate, const QDate& endDate, qint64 productId = 0);
    QVariantList convertSalesReportData(const QList<SalesReportData>& data);
    
    // Métodos de exportación (implementaciones específicas)
    bool writeCSVFile(const QString& fileName, const QVariantList& data);
    bool writeExcelFile(const QString& fileName, const QVariantList& data);
    bool writePDFFile(const QString& fileName, const QVariantList& data);
    void printData(const QVariantList& data);
};

} // namespace Vento::Reporting
