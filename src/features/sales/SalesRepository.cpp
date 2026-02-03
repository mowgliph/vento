//==============================================================================
// SalesRepository.cpp
// Vento - Implementación de la capa de persistencia para ventas
//==============================================================================
#include "SalesRepository.hpp"

#include <algorithm>
#include <QDate>

namespace Vento::Sales {

//==============================================================================
// Operaciones CRUD
//==============================================================================

SaleResult SalesRepository::save(Sale& sale) {
    if (sale.items.empty()) {
        return SaleResult::error("La venta debe tener al menos un item");
    }
    
    sale.calculateTotals();
    
    if (sale.total < Defaults::MIN_SALE_AMOUNT) {
        return SaleResult::error("El total de la venta es inválido");
    }
    
    if (sale.id == 0) {
        sale.id = m_nextId++;
        sale.createdAt = QDateTime::currentDateTime();
        if (sale.receiptNumber.isEmpty()) {
            sale.receiptNumber = generateReceiptNumber();
        }
    }
    
    updateTimestamps(sale);
    
    // Asignar saleId a los items
    for (auto& item : sale.items) {
        item.saleId = sale.id;
        if (item.id == 0) {
            item.id = m_nextId++;
        }
        if (item.createdAt.isNull()) {
            item.createdAt = QDateTime::currentDateTime();
        }
    }
    
    auto it = std::find_if(m_sales.begin(), m_sales.end(),
        [&sale](const Sale& existing) { return existing.id == sale.id; });
    
    if (it != m_sales.end()) {
        *it = sale;
    } else {
        m_sales.push_back(sale);
    }
    
    return SaleResult::ok(sale.id, sale.total);
}

std::optional<Sale> SalesRepository::findById(qint64 id) const {
    auto it = std::find_if(m_sales.begin(), m_sales.end(),
        [id](const Sale& s) { return s.id == id; });
    
    if (it != m_sales.end()) {
        return *it;
    }
    return std::nullopt;
}

std::vector<Sale> SalesRepository::findAll() const {
    return m_sales;
}

std::vector<Sale> SalesRepository::findByDateRange(const QDateTime& from, const QDateTime& to) const {
    std::vector<Sale> result;
    for (const auto& sale : m_sales) {
        if (sale.saleDate >= from && sale.saleDate <= to) {
            result.push_back(sale);
        }
    }
    return result;
}

std::vector<Sale> SalesRepository::findToday() const {
    QDate today = QDate::currentDate();
    QDateTime from(today, QTime(0, 0, 0));
    QDateTime to(today, QTime(23, 59, 59));
    return findByDateRange(from, to);
}

std::vector<Sale> SalesRepository::findThisWeek() const {
    QDate today = QDate::currentDate();
    QDate startOfWeek = today.addDays(-today.dayOfWeek() + 1);
    QDateTime from(startOfWeek, QTime(0, 0, 0));
    QDateTime to(today, QTime(23, 59, 59));
    return findByDateRange(from, to);
}

std::vector<Sale> SalesRepository::findThisMonth() const {
    QDate today = QDate::currentDate();
    QDate startOfMonth(today.year(), today.month(), 1);
    QDateTime from(startOfMonth, QTime(0, 0, 0));
    QDateTime to(today, QTime(23, 59, 59));
    return findByDateRange(from, to);
}

std::vector<Sale> SalesRepository::findByStatus(SaleStatus status) const {
    std::vector<Sale> result;
    for (const auto& sale : m_sales) {
        if (sale.status == status) {
            result.push_back(sale);
        }
    }
    return result;
}

std::vector<Sale> SalesRepository::findByPaymentMethod(PaymentMethod method) const {
    std::vector<Sale> result;
    for (const auto& sale : m_sales) {
        if (sale.paymentMethod == method) {
            result.push_back(sale);
        }
    }
    return result;
}

std::vector<Sale> SalesRepository::searchByCustomer(const QString& query) const {
    std::vector<Sale> result;
    QString lowerQuery = query.toLower();
    
    for (const auto& sale : m_sales) {
        if (sale.customerName.toLower().contains(lowerQuery) ||
            sale.customerPhone.contains(lowerQuery) ||
            sale.customerId.contains(lowerQuery)) {
            result.push_back(sale);
        }
    }
    return result;
}

SaleResult SalesRepository::cancel(qint64 id, const QString& reason) {
    if (updateSaleStatus(id, SaleStatus::Cancelled, reason)) {
        return SaleResult::ok(id, 0.0);
    }
    return SaleResult::error("No se pudo cancelar la venta");
}

SaleResult SalesRepository::refund(qint64 id, const QString& reason) {
    if (updateSaleStatus(id, SaleStatus::Refunded, reason)) {
        return SaleResult::ok(id, 0.0);
    }
    return SaleResult::error("No se pudo reembolsar la venta");
}

SaleResult SalesRepository::remove(qint64 id) {
    auto it = std::find_if(m_sales.begin(), m_sales.end(),
        [id](const Sale& s) { return s.id == id; });
    
    if (it != m_sales.end()) {
        m_sales.erase(it);
        return SaleResult::ok(id, 0.0);
    }
    
    return SaleResult::error("Venta no encontrada");
}

//==============================================================================
// Estadísticas
//==============================================================================

int SalesRepository::count() const {
    return static_cast<int>(m_sales.size());
}

int SalesRepository::countByStatus(SaleStatus status) const {
    return static_cast<int>(std::count_if(m_sales.begin(), m_sales.end(),
        [status](const Sale& s) { return s.status == status; }));
}

double SalesRepository::getTotalSales(const QDateTime& from, const QDateTime& to) const {
    double total = 0.0;
    for (const auto& sale : m_sales) {
        if (sale.saleDate >= from && sale.saleDate <= to && 
            sale.status == SaleStatus::Completed) {
            total += sale.total;
        }
    }
    return total;
}

double SalesRepository::getTodayTotal() const {
    auto todaySales = findToday();
    double total = 0.0;
    for (const auto& sale : todaySales) {
        if (sale.status == SaleStatus::Completed) {
            total += sale.total;
        }
    }
    return total;
}

double SalesRepository::getAverageSale() const {
    int completedCount = countByStatus(SaleStatus::Completed);
    if (completedCount == 0) return 0.0;
    
    double total = 0.0;
    for (const auto& sale : m_sales) {
        if (sale.status == SaleStatus::Completed) {
            total += sale.total;
        }
    }
    return total / completedCount;
}

//==============================================================================
// Utilidades
//==============================================================================

QString SalesRepository::generateReceiptNumber() {
    QString date = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString number = QString::number(m_receiptCounter++).rightJustified(4, '0');
    return QString("REC-%1-%2").arg(date, number);
}

void SalesRepository::clear() {
    m_sales.clear();
    m_nextId = 1;
    m_receiptCounter = 1;
}

void SalesRepository::updateTimestamps(Sale& sale) {
    sale.updatedAt = QDateTime::currentDateTime();
    if (sale.saleDate.isNull()) {
        sale.saleDate = QDateTime::currentDateTime();
    }
}

bool SalesRepository::updateSaleStatus(qint64 id, SaleStatus newStatus, const QString& reason) {
    auto it = std::find_if(m_sales.begin(), m_sales.end(),
        [id](const Sale& s) { return s.id == id; });
    
    if (it != m_sales.end()) {
        it->status = newStatus;
        it->updatedAt = QDateTime::currentDateTime();
        if (!reason.isEmpty()) {
            it->notes = reason;
        }
        return true;
    }
    return false;
}

} // namespace Vento::Sales
