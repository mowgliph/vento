//==============================================================================
// ProductModel.cpp
// Vento - Implementación del modelo de productos para QML
//==============================================================================
#include "ProductModel.hpp"
#include <algorithm>

namespace Vento::Inventory {

//==============================================================================
// ProductModel
//==============================================================================

ProductModel::ProductModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_repository(new ProductRepository())
{
}

ProductModel::~ProductModel() {
    delete m_repository;
}

int ProductModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_products.size());
}

QVariant ProductModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(m_products.size())) {
        return QVariant();
    }

    const Product& product = m_products[index.row()];

    switch (role) {
        case Roles::IdRole: return product.id;
        case Roles::NameRole: return product.name;
        case Roles::DescriptionRole: return product.description;
        case Roles::SkuRole: return product.sku;
        case Roles::BarcodeRole: return product.barcode;
        case Roles::CategoryRole: return product.category;
        case Roles::CostUsdRole: return product.costUsd;
        case Roles::CostLocalRole: return product.costLocal;
        case Roles::MarginPercentRole: return product.marginPercent;
        case Roles::SalePriceRole: return product.salePrice;
        case Roles::StockQuantityRole: return product.stockQuantity;
        case Roles::MinStockAlertRole: return product.minStockAlert;
        case Roles::IsActiveRole: return product.isActive;
        case Roles::CreatedAtRole: return product.createdAt;
        case Roles::UpdatedAtRole: return product.updatedAt;
        case Roles::ProfitAmountRole: return product.profitAmount();
        case Roles::ProfitPercentRole: return product.profitPercent();
        case Roles::IsLowStockRole: return product.isLowStock();
        case Roles::FormattedCostUsdRole: return QString("$%1").arg(product.costUsd, 0, 'f', 2);
        case Roles::FormattedCostLocalRole: return QString("$%1").arg(product.costLocal, 0, 'f', 2);
        case Roles::FormattedSalePriceRole: return QString("$%1").arg(product.salePrice, 0, 'f', 2);
        case Roles::StockStatusRole: return product.stockStatus();
        default: return QVariant();
    }
}

QHash<int, QByteArray> ProductModel::roleNames() const {
    static QHash<int, QByteArray> roles = {
        {Roles::IdRole, "id"},
        {Roles::NameRole, "name"},
        {Roles::DescriptionRole, "description"},
        {Roles::SkuRole, "sku"},
        {Roles::BarcodeRole, "barcode"},
        {Roles::CategoryRole, "category"},
        {Roles::CostUsdRole, "costUsd"},
        {Roles::CostLocalRole, "costLocal"},
        {Roles::MarginPercentRole, "marginPercent"},
        {Roles::SalePriceRole, "salePrice"},
        {Roles::StockQuantityRole, "stockQuantity"},
        {Roles::MinStockAlertRole, "minStockAlert"},
        {Roles::IsActiveRole, "isActive"},
        {Roles::CreatedAtRole, "createdAt"},
        {Roles::UpdatedAtRole, "updatedAt"},
        {Roles::ProfitAmountRole, "profitAmount"},
        {Roles::ProfitPercentRole, "profitPercent"},
        {Roles::IsLowStockRole, "isLowStock"},
        {Roles::FormattedCostUsdRole, "formattedCostUsd"},
        {Roles::FormattedCostLocalRole, "formattedCostLocal"},
        {Roles::FormattedSalePriceRole, "formattedSalePrice"},
        {Roles::StockStatusRole, "stockStatus"}
    };
    return roles;
}

QVariant ProductModel::get(int row) const {
    if (row < 0 || row >= static_cast<int>(m_products.size())) {
        return QVariant();
    }
    return m_products[row].toVariantMap();
}

int ProductModel::indexOfId(qint64 id) const {
    for (int i = 0; i < static_cast<int>(m_products.size()); ++i) {
        if (m_products[i].id == id) {
            return i;
        }
    }
    return -1;
}

void ProductModel::refresh() {
    beginResetModel();
    applyFiltersAndSort();
    endResetModel();
    emit countChanged();
    emit activeCountChanged();
    emit lowStockCountChanged();
}

void ProductModel::setFilter(ProductFilter filter) {
    if (m_filter != filter) {
        m_filter = filter;
        refresh();
    }
}

void ProductModel::setCategoryFilter(const QString& category) {
    if (m_categoryFilter != category) {
        m_categoryFilter = category;
        refresh();
    }
}

void ProductModel::setSearchQuery(const QString& query) {
    if (m_searchQuery != query) {
        m_searchQuery = query;
        refresh();
    }
}

void ProductModel::setSortField(ProductSortField field) {
    if (m_sortField != field) {
        m_sortField = field;
        refresh();
    }
}

void ProductModel::setSortOrder(SortOrder order) {
    if (m_sortOrder != order) {
        m_sortOrder = order;
        refresh();
    }
}

void ProductModel::clearFilters() {
    m_filter = ProductFilter::All;
    m_categoryFilter.clear();
    m_searchQuery.clear();
    refresh();
}

int ProductModel::count() const {
    return m_repository->count();
}

int ProductModel::activeCount() const {
    return m_repository->countActive();
}

int ProductModel::lowStockCount() const {
    return m_repository->countLowStock();
}

void ProductModel::onProductAdded(const Product& product) {
    int index = static_cast<int>(m_products.size());
    beginInsertRows(QModelIndex(), index, index);
    m_products.push_back(product);
    endInsertRows();
    emit countChanged();
}

void ProductModel::onProductUpdated(const Product& product) {
    int row = indexOfId(product.id);
    if (row >= 0) {
        QModelIndex idx = index(row, 0);
        m_products[row] = product;
        emit dataChanged(idx, idx);
    }
}

void ProductModel::onProductRemoved(qint64 id) {
    int row = indexOfId(id);
    if (row >= 0) {
        beginRemoveRows(QModelIndex(), row, row);
        m_products.erase(m_products.begin() + row);
        endRemoveRows();
        emit countChanged();
    }
}

void ProductModel::applyFiltersAndSort() {
    m_products.clear();

    std::vector<Product> allProducts = m_repository->findAll();

    for (const auto& product : allProducts) {
        if (matchesFilter(product)) {
            m_products.push_back(product);
        }
    }

    std::sort(m_products.begin(), m_products.end(),
        [this](const Product& a, const Product& b) {
            return compareProducts(a, b);
        });
}

bool ProductModel::matchesFilter(const Product& product) const {
    if (!product.isActive) {
        return false;
    }

    switch (m_filter) {
        case ProductFilter::Active:
            return product.isActive;
        case ProductFilter::Inactive:
            return !product.isActive;
        case ProductFilter::LowStock:
            return product.isLowStock();
        case ProductFilter::OutOfStock:
            return product.isOutOfStock();
        case ProductFilter::Category:
            return !m_categoryFilter.isEmpty() && product.category == m_categoryFilter;
        case ProductFilter::All:
        default:
            break;
    }

    if (!m_categoryFilter.isEmpty() && product.category != m_categoryFilter) {
        return false;
    }

    if (!m_searchQuery.isEmpty()) {
        QString query = m_searchQuery.toLower();
        return product.name.toLower().contains(query) ||
               product.sku.toLower().contains(query) ||
               product.barcode.toLower().contains(query);
    }

    return true;
}

bool ProductModel::compareProducts(const Product& a, const Product& b) const {
    bool ascending = (m_sortOrder == SortOrder::Ascending);

    bool result = false;
    switch (m_sortField) {
        case ProductSortField::Name:
            result = a.name.compare(b.name, Qt::CaseInsensitive) < 0;
            break;
        case ProductSortField::Category:
            result = a.category.compare(b.category, Qt::CaseInsensitive) < 0;
            break;
        case ProductSortField::CostUsd:
            result = a.costUsd < b.costUsd;
            break;
        case ProductSortField::SalePrice:
            result = a.salePrice < b.salePrice;
            break;
        case ProductSortField::Stock:
            result = a.stockQuantity < b.stockQuantity;
            break;
        case ProductSortField::CreatedAt:
            result = a.createdAt < b.createdAt;
            break;
        case ProductSortField::UpdatedAt:
            result = a.updatedAt < b.updatedAt;
            break;
    }

    return ascending ? result : !result;
}

//==============================================================================
// ProductSortFilterProxyModel
//==============================================================================

ProductSortFilterProxyModel::ProductSortFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

ProductSortFilterProxyModel::~ProductSortFilterProxyModel() = default;

ProductFilter ProductSortFilterProxyModel::filter() const {
    return m_filter;
}

void ProductSortFilterProxyModel::setFilter(ProductFilter filter) {
    if (m_filter != filter) {
        m_filter = filter;
        emit filterChanged();
        invalidateFilter();
    }
}

QString ProductSortFilterProxyModel::categoryFilter() const {
    return m_categoryFilter;
}

void ProductSortFilterProxyModel::setCategoryFilter(const QString& category) {
    if (m_categoryFilter != category) {
        m_categoryFilter = category;
        emit categoryFilterChanged();
        invalidateFilter();
    }
}

QString ProductSortFilterProxyModel::searchQuery() const {
    return m_searchQuery;
}

void ProductSortFilterProxyModel::setSearchQuery(const QString& query) {
    if (m_searchQuery != query) {
        m_searchQuery = query;
        emit searchQueryChanged();
        invalidateFilter();
    }
}

ProductSortField ProductSortFilterProxyModel::sortField() const {
    return m_sortField;
}

void ProductSortFilterProxyModel::setSortField(ProductSortField field) {
    if (m_sortField != field) {
        m_sortField = field;
        emit sortFieldChanged();
        invalidate();
    }
}

SortOrder ProductSortFilterProxyModel::sortOrder() const {
    return m_sortOrder;
}

void ProductSortFilterProxyModel::setSortOrder(SortOrder order) {
    if (m_sortOrder != order) {
        m_sortOrder = order;
        emit sortOrderChanged();
        invalidate();
    }
}

bool ProductSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    
    bool isActive = sourceModel()->data(index, Roles::IsActiveRole).toBool();
    if (!isActive) {
        return false;
    }

    switch (m_filter) {
        case ProductFilter::Active:
            return isActive;
        case ProductFilter::Inactive:
            return !isActive;
        case ProductFilter::LowStock:
            return sourceModel()->data(index, Roles::IsLowStockRole).toBool();
        case ProductFilter::OutOfStock:
            return sourceModel()->data(index, Roles::StockStatusRole).toString() == "out";
        case ProductFilter::Category:
            if (m_categoryFilter.isEmpty()) return true;
            return sourceModel()->data(index, Roles::CategoryRole).toString() == m_categoryFilter;
        case ProductFilter::All:
        default:
            break;
    }

    if (!m_categoryFilter.isEmpty()) {
        if (sourceModel()->data(index, Roles::CategoryRole).toString() != m_categoryFilter) {
            return false;
        }
    }

    if (!m_searchQuery.isEmpty()) {
        QString name = sourceModel()->data(index, Roles::NameRole).toString().toLower();
        QString sku = sourceModel()->data(index, Roles::SkuRole).toString().toLower();
        QString barcode = sourceModel()->data(index, Roles::BarcodeRole).toString().toLower();
        QString query = m_searchQuery.toLower();
        
        return name.contains(query) || sku.contains(query) || barcode.contains(query);
    }

    return true;
}

bool ProductSortFilterProxyModel::lessThan(const QModelIndex& sourceLeft, const QModelIndex& sourceRight) const {
    bool ascending = (m_sortOrder == SortOrder::Ascending);

    QVariant leftData, rightData;
    switch (m_sortField) {
        case ProductSortField::Name:
            leftData = sourceModel()->data(sourceLeft, Roles::NameRole);
            rightData = sourceModel()->data(sourceRight, Roles::NameRole);
            return ascending ? leftData.toString().compare(rightData.toString(), Qt::CaseInsensitive) < 0
                            : leftData.toString().compare(rightData.toString(), Qt::CaseInsensitive) > 0;
        case ProductSortField::Category:
            leftData = sourceModel()->data(sourceLeft, Roles::CategoryRole);
            rightData = sourceModel()->data(sourceRight, Roles::CategoryRole);
            return ascending ? leftData.toString().compare(rightData.toString(), Qt::CaseInsensitive) < 0
                            : leftData.toString().compare(rightData.toString(), Qt::CaseInsensitive) > 0;
        case ProductSortField::CostUsd:
            leftData = sourceModel()->data(sourceLeft, Roles::CostUsdRole);
            rightData = sourceModel()->data(sourceRight, Roles::CostUsdRole);
            return ascending ? leftData.toDouble() < rightData.toDouble()
                            : leftData.toDouble() > rightData.toDouble();
        case ProductSortField::SalePrice:
            leftData = sourceModel()->data(sourceLeft, Roles::SalePriceRole);
            rightData = sourceModel()->data(sourceRight, Roles::SalePriceRole);
            return ascending ? leftData.toDouble() < rightData.toDouble()
                            : leftData.toDouble() > rightData.toDouble();
        case ProductSortField::Stock:
            leftData = sourceModel()->data(sourceLeft, Roles::StockQuantityRole);
            rightData = sourceModel()->data(sourceRight, Roles::StockQuantityRole);
            return ascending ? leftData.toInt() < rightData.toInt()
                            : leftData.toInt() > rightData.toInt();
        case ProductSortField::CreatedAt:
            leftData = sourceModel()->data(sourceLeft, Roles::CreatedAtRole);
            rightData = sourceModel()->data(sourceRight, Roles::CreatedAtRole);
            return ascending ? leftData.toDateTime() < rightData.toDateTime()
                            : leftData.toDateTime() > rightData.toDateTime();
        case ProductSortField::UpdatedAt:
            leftData = sourceModel()->data(sourceLeft, Roles::UpdatedAtRole);
            rightData = sourceModel()->data(sourceRight, Roles::UpdatedAtRole);
            return ascending ? leftData.toDateTime() < rightData.toDateTime()
                            : leftData.toDateTime() > rightData.toDateTime();
    }

    return false;
}

} // namespace Vento::Inventory
