//==============================================================================
// SalesRepository.hpp
// Vento - Capa de persistencia para ventas
//==============================================================================
#pragma once

#include <vector>
#include <memory>
#include <optional>
#include <QDateTime>

#include "Sale.hpp"
#include "SalesTypes.hpp"

namespace Vento::Sales {

//==============================================================================
// SalesRepository - Almacén persistente de ventas
//==============================================================================
class SalesRepository {
public:
    SalesRepository() = default;
    ~SalesRepository() = default;
    
    SalesRepository(const SalesRepository&) = delete;
    SalesRepository& operator=(const SalesRepository&) = delete;
    SalesRepository(SalesRepository&&) = default;
    SalesRepository& operator=(SalesRepository&&) = default;
    
    //==========================================================================
    // Operaciones CRUD
    //==========================================================================
    
    /// Guarda una nueva venta o actualiza una existente
    SaleResult save(Sale& sale);
    
    /// Busca una venta por ID
    std::optional<Sale> findById(qint64 id) const;
    
    /// Obtiene todas las ventas
    std::vector<Sale> findAll() const;
    
    /// Obtiene ventas por rango de fechas
    std::vector<Sale> findByDateRange(const QDateTime& from, const QDateTime& to) const;
    
    /// Obtiene ventas de hoy
    std::vector<Sale> findToday() const;
    
    /// Obtiene ventas de esta semana
    std::vector<Sale> findThisWeek() const;
    
    /// Obtiene ventas de este mes
    std::vector<Sale> findThisMonth() const;
    
    /// Obtiene ventas por estado
    std::vector<Sale> findByStatus(SaleStatus status) const;
    
    /// Obtiene ventas por método de pago
    std::vector<Sale> findByPaymentMethod(PaymentMethod method) const;
    
    /// Busca ventas por nombre de cliente
    std::vector<Sale> searchByCustomer(const QString& query) const;
    
    /// Cancela una venta
    SaleResult cancel(qint64 id, const QString& reason);
    
    /// Marca una venta como reembolsada
    SaleResult refund(qint64 id, const QString& reason);
    
    /// Elimina una venta permanentemente
    SaleResult remove(qint64 id);
    
    //==========================================================================
    // Estadísticas
    //==========================================================================
    
    /// Obtiene el conteo total de ventas
    [[nodiscard]] int count() const;
    
    /// Obtiene el conteo de ventas por estado
    [[nodiscard]] int countByStatus(SaleStatus status) const;
    
    /// Obtiene el total de ventas en un rango de fechas
    [[nodiscard]] double getTotalSales(const QDateTime& from, const QDateTime& to) const;
    
    /// Obtiene el total de ventas de hoy
    [[nodiscard]] double getTodayTotal() const;
    
    /// Obtiene el promedio de ventas
    [[nodiscard]] double getAverageSale() const;
    
    //==========================================================================
    // Utilidades
    //==========================================================================
    
    /// Genera un número de recibo único
    [[nodiscard]] QString generateReceiptNumber();
    
    /// Limpia todas las ventas
    void clear();
    
private:
    std::vector<Sale> m_sales;
    qint64 m_nextId = 1;
    int m_receiptCounter = 1;
    
    void updateTimestamps(Sale& sale);
    bool updateSaleStatus(qint64 id, SaleStatus newStatus, const QString& reason);
};

} // namespace Vento::Sales
