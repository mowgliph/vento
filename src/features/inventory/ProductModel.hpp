//==============================================================================
// ProductModel.hpp
// Vento - QAbstractListModel para exponer productos a QML
//==============================================================================
#pragma once

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <QString>
#include <QVariant>

#include "Product.hpp"
#include "ProductRepository.hpp"
#include "InventoryTypes.hpp"

namespace Vento::Inventory {

//==============================================================================
// ProductModel - Modelo de productos para QML
//==============================================================================
class ProductModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int activeCount READ activeCount NOTIFY activeCountChanged)
    Q_PROPERTY(int lowStockCount READ lowStockCount NOTIFY lowStockCountChanged)

public:
    explicit ProductModel(QObject* parent = nullptr);
    ~ProductModel() override;

    //==========================================================================
    // QAbstractListModel interface
    //==========================================================================
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;

    //==========================================================================
    // Métodos QML
    //==========================================================================
    Q_INVOKABLE QVariant get(int row) const;
    Q_INVOKABLE int indexOfId(qint64 id) const;
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void setFilter(ProductFilter filter);
    Q_INVOKABLE void setCategoryFilter(const QString& category);
    Q_INVOKABLE void setSearchQuery(const QString& query);
    Q_INVOKABLE void setSortField(ProductSortField field);
    Q_INVOKABLE void setSortOrder(SortOrder order);
    Q_INVOKABLE void clearFilters();

    //==========================================================================
    // Getters
    //==========================================================================
    [[nodiscard]] int count() const;
    [[nodiscard]] int activeCount() const;
    [[nodiscard]] int lowStockCount() const;

signals:
    void countChanged();
    void activeCountChanged();
    void lowStockCountChanged();
    void modelChanged();

public slots:
    void onProductAdded(const Product& product);
    void onProductUpdated(const Product& product);
    void onProductRemoved(qint64 id);

private:
    ProductRepository* m_repository;
    std::vector<Product> m_products;
    ProductFilter m_filter = ProductFilter::All;
    QString m_categoryFilter;
    QString m_searchQuery;
    ProductSortField m_sortField = ProductSortField::Name;
    SortOrder m_sortOrder = SortOrder::Ascending;

    void applyFiltersAndSort();
    bool matchesFilter(const Product& product) const;
    bool compareProducts(const Product& a, const Product& b) const;
};

//==============================================================================
// ProductSortFilterProxyModel - Proxy para filtrado y ordenamiento
//==============================================================================
class ProductSortFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
    Q_PROPERTY(ProductFilter filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(QString categoryFilter READ categoryFilter WRITE setCategoryFilter NOTIFY categoryFilterChanged)
    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)
    Q_PROPERTY(ProductSortField sortField READ sortField WRITE setSortField NOTIFY sortFieldChanged)
    Q_PROPERTY(SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)

public:
    explicit ProductSortFilterProxyModel(QObject* parent = nullptr);
    ~ProductSortFilterProxyModel() override;

    ProductFilter filter() const;
    void setFilter(ProductFilter filter);

    QString categoryFilter() const;
    void setCategoryFilter(const QString& category);

    QString searchQuery() const;
    void setSearchQuery(const QString& query);

    ProductSortField sortField() const;
    void setSortField(ProductSortField field);

    SortOrder sortOrder() const;
    void setSortOrder(SortOrder order);

signals:
    void filterChanged();
    void categoryFilterChanged();
    void searchQueryChanged();
    void sortFieldChanged();
    void sortOrderChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    bool lessThan(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const override;

private:
    ProductFilter m_filter = ProductFilter::All;
    QString m_categoryFilter;
    QString m_searchQuery;
    ProductSortField m_sortField = ProductSortField::Name;
    SortOrder m_sortOrder = SortOrder::Ascending;
};

} // namespace Vento::Inventory
