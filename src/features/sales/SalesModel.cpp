//==============================================================================
// SalesModel.cpp
// Vento - Implementación del modelo de historial de ventas
//==============================================================================
#include "SalesModel.hpp"

#include <QDateTime>
#include <QDate>
#include <algorithm>

namespace Vento::Sales {

//==============================================================================
// Constructor/Destructor
//==============================================================================

SalesModel::SalesModel(QObject* parent)
    : QAbstractListModel(parent)
{
    loadSales();
}

SalesModel::~SalesModel() = default;

//==============================================================================
// QAbstractListModel Interface
//==============================================================================

int SalesModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_filteredSales.size());
}

QVariant SalesModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(m_filteredSales.size())) {
        return {};
    }
    
    const Sale& sale = m_filteredSales[index.row()];
    
    switch (role) {
        case Roles::IdRole:
            return QVariant::fromValue(sale.id);
        case Roles::SaleDateRole:
            return sale.saleDate;
        case Roles::SubtotalRole:
            return sale.subtotal;
        case Roles::TaxPercentRole:
            return sale.taxPercent;
        case Roles::TaxAmountRole:
            return sale.taxAmount;
        case Roles::DiscountAmountRole:
            return sale.discountAmount;
        case Roles::TotalRole:
            return sale.total;
        case Roles::ExchangeRateRole:
            return sale.exchangeRate;
        case Roles::PaymentMethodRole:
            return paymentToString(sale.paymentMethod);
        case Roles::PaymentMethodDisplayRole:
            return paymentDisplayName(sale.paymentMethod);
        case Roles::CustomerNameRole:
            return sale.customerName;
        case Roles::StatusRole:
            return statusToString(sale.status);
        case Roles::StatusDisplayRole:
            return statusDisplayName(sale.status);
        case Roles::NotesRole:
            return sale.notes;
        case Roles::ItemCountRole:
            return sale.itemCount();
        case Roles::FormattedTotalRole:
            return formatCurrency(sale.total);
        case Roles::FormattedDateRole:
            return sale.formattedDate();
        case Roles::FormattedTimeRole:
            return sale.formattedTime();
        default:
            return {};
    }
}

QHash<int, QByteArray> SalesModel::roleNames() const {
    return {
        {Roles::IdRole, "id"},
        {Roles::SaleDateRole, "saleDate"},
        {Roles::SubtotalRole, "subtotal"},
        {Roles::TaxPercentRole, "taxPercent"},
        {Roles::TaxAmountRole, "taxAmount"},
        {Roles::DiscountAmountRole, "discountAmount"},
        {Roles::TotalRole, "total"},
        {Roles::ExchangeRateRole, "exchangeRate"},
        {Roles::PaymentMethodRole, "paymentMethod"},
        {Roles::PaymentMethodDisplayRole, "paymentMethodDisplay"},
        {Roles::CustomerNameRole, "customerName"},
        {Roles::StatusRole, "status"},
        {Roles::StatusDisplayRole, "statusDisplay"},
        {Roles::NotesRole, "notes"},
        {Roles::ItemCountRole, "itemCount"},
        {Roles::FormattedTotalRole, "formattedTotal"},
        {Roles::FormattedDateRole, "formattedDate"},
        {Roles::FormattedTimeRole, "formattedTime"}
    };
}

//==============================================================================
// Propiedades
//==============================================================================

int SalesModel::count() const {
    return static_cast<int>(m_filteredSales.size());
}

double SalesModel::todayTotal() const {
    QDate today = QDate::currentDate();
    double total = 0.0;
    for (const auto& sale : m_sales) {
        if (sale.saleDate.date() == today && sale.status == SaleStatus::Completed) {
            total += sale.total;
        }
    }
    return total;
}

int SalesModel::todayCount() const {
    QDate today = QDate::currentDate();
    int count = 0;
    for (const auto& sale : m_sales) {
        if (sale.saleDate.date() == today && sale.status == SaleStatus::Completed) {
            count++;
        }
    }
    return count;
}

double SalesModel::averageSale() const {
    int completedCount = 0;
    double total = 0.0;
    for (const auto& sale : m_sales) {
        if (sale.status == SaleStatus::Completed) {
            total += sale.total;
            completedCount++;
        }
    }
    return completedCount > 0 ? total / completedCount : 0.0;
}

//==============================================================================
// Métodos QML Invocables
//==============================================================================

QVariantMap SalesModel::get(int index) const {
    if (index < 0 || index >= static_cast<int>(m_filteredSales.size())) {
        return {};
    }
    return m_filteredSales[index].toVariantMap();
}

QVariantMap SalesModel::getById(qint64 id) const {
    auto it = std::find_if(m_sales.begin(), m_sales.end(),
        [id](const Sale& s) { return s.id == id; });
    
    if (it != m_sales.end()) {
        return it->toVariantMap();
    }
    return {};
}

void SalesModel::refresh() {
    beginResetModel();
    loadSales();
    endResetModel();
    emit countChanged();
    emit statisticsChanged();
}

void SalesModel::filterByDate(const QDateTime& from, const QDateTime& to) {
    beginResetModel();
    m_filteredSales.clear();
    for (const auto& sale : m_sales) {
        if (sale.saleDate >= from && sale.saleDate <= to) {
            m_filteredSales.push_back(sale);
        }
    }
    m_hasFilter = true;
    endResetModel();
    emit countChanged();
}

void SalesModel::filterByStatus(const QString& status) {
    beginResetModel();
    m_filteredSales.clear();
    SaleStatus targetStatus = stringToStatus(status);
    for (const auto& sale : m_sales) {
        if (sale.status == targetStatus) {
            m_filteredSales.push_back(sale);
        }
    }
    m_hasFilter = true;
    endResetModel();
    emit countChanged();
}

void SalesModel::filterToday() {
    QDate today = QDate::currentDate();
    QDateTime from(today, QTime(0, 0, 0));
    QDateTime to(today, QTime(23, 59, 59));
    filterByDate(from, to);
}

void SalesModel::filterThisWeek() {
    QDate today = QDate::currentDate();
    QDate startOfWeek = today.addDays(-today.dayOfWeek() + 1);
    QDateTime from(startOfWeek, QTime(0, 0, 0));
    QDateTime to(today, QTime(23, 59, 59));
    filterByDate(from, to);
}

void SalesModel::filterThisMonth() {
    QDate today = QDate::currentDate();
    QDate startOfMonth(today.year(), today.month(), 1);
    QDateTime from(startOfMonth, QTime(0, 0, 0));
    QDateTime to(today, QTime(23, 59, 59));
    filterByDate(from, to);
}

void SalesModel::clearFilter() {
    beginResetModel();
    m_filteredSales = m_sales;
    m_hasFilter = false;
    endResetModel();
    emit countChanged();
}

void SalesModel::search(const QString& query) {
    if (query.isEmpty()) {
        clearFilter();
        return;
    }
    
    beginResetModel();
    m_filteredSales.clear();
    QString lowerQuery = query.toLower();
    
    for (const auto& sale : m_sales) {
        if (sale.customerName.toLower().contains(lowerQuery) ||
            sale.receiptNumber.toLower().contains(lowerQuery) ||
            QString::number(sale.id).contains(query)) {
            m_filteredSales.push_back(sale);
        }
    }
    m_hasFilter = true;
    endResetModel();
    emit countChanged();
}

//==============================================================================
// Métodos Privados
//==============================================================================

void SalesModel::loadSales() {
    // En una implementación real, esto cargaría desde SalesRepository
    // Por ahora, usamos los datos en memoria
    if (m_sales.empty()) {
        m_filteredSales.clear();
    } else {
        m_filteredSales = m_sales;
    }
}

void SalesModel::applyFilter() {
    if (!m_hasFilter) {
        m_filteredSales = m_sales;
    }
}

QString SalesModel::formatCurrency(double amount) const {
    return QString("$ %1").arg(QString::number(amount, 'f', 2));
}

} // namespace Vento::Sales
