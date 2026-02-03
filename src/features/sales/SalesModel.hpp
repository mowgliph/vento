//==============================================================================
// SalesModel.hpp
// Vento - Modelo de datos para historial de ventas (QAbstractListModel)
//==============================================================================
#pragma once

#include <QAbstractListModel>
#include <vector>
#include <memory>

#include "Sale.hpp"
#include "SalesTypes.hpp"

namespace Vento::Sales {

//==============================================================================
// SalesModel - Modelo QML para lista de ventas
//==============================================================================
class SalesModel : public QAbstractListModel {
    Q_OBJECT
    
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(double todayTotal READ todayTotal NOTIFY statisticsChanged)
    Q_PROPERTY(int todayCount READ todayCount NOTIFY statisticsChanged)
    Q_PROPERTY(double averageSale READ averageSale NOTIFY statisticsChanged)

public:
    explicit SalesModel(QObject* parent = nullptr);
    ~SalesModel() override;
    
    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    
    // Propiedades QML
    [[nodiscard]] int count() const;
    [[nodiscard]] double todayTotal() const;
    [[nodiscard]] int todayCount() const;
    [[nodiscard]] double averageSale() const;
    
    // Métodos QML invocables
    Q_INVOKABLE QVariantMap get(int index) const;
    Q_INVOKABLE QVariantMap getById(qint64 id) const;
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void filterByDate(const QDateTime& from, const QDateTime& to);
    Q_INVOKABLE void filterByStatus(const QString& status);
    Q_INVOKABLE void filterToday();
    Q_INVOKABLE void filterThisWeek();
    Q_INVOKABLE void filterThisMonth();
    Q_INVOKABLE void clearFilter();
    Q_INVOKABLE void search(const QString& query);
    
signals:
    void countChanged();
    void statisticsChanged();
    void saleSelected(qint64 saleId);

private:
    std::vector<Sale> m_sales;
    std::vector<Sale> m_filteredSales;
    bool m_hasFilter = false;
    
    void loadSales();
    void applyFilter();
    QString formatCurrency(double amount) const;
};

} // namespace Vento::Sales
