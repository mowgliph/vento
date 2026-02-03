//==============================================================================
// SalesService.hpp
// Vento - Servicio principal de ventas (Controller)
//==============================================================================
#pragma once

#include <QObject>
#include <QString>
#include <memory>

#include "SalesTypes.hpp"
#include "Sale.hpp"
#include "SalesRepository.hpp"
#include "CartModel.hpp"
#include "SalesModel.hpp"

namespace Vento::Inventory {
class ProductRepository;
}

namespace Vento::Sales {

//==============================================================================
// SalesService - Controller principal del módulo de ventas
//==============================================================================
class SalesService : public QObject {
    Q_OBJECT
    
    Q_PROPERTY(CartModel* cartModel READ cartModel CONSTANT)
    Q_PROPERTY(SalesModel* salesModel READ salesModel CONSTANT)
    Q_PROPERTY(bool isProcessing READ isProcessing NOTIFY processingChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY errorOccurred)

public:
    static SalesService& instance();
    
    void initialize(Inventory::ProductRepository* productRepo = nullptr);
    
    // Modelos QML
    [[nodiscard]] CartModel* cartModel() const { return m_cartModel.get(); }
    [[nodiscard]] SalesModel* salesModel() const { return m_salesModel.get(); }
    
    // Estado
    [[nodiscard]] bool isProcessing() const { return m_processing; }
    [[nodiscard]] QString lastError() const { return m_lastError; }
    
    // Métodos QML invocables - Carrito
    Q_INVOKABLE bool addToCart(qint64 productId, int quantity = 1);
    Q_INVOKABLE bool addToCartByBarcode(const QString& barcode, int quantity = 1);
    Q_INVOKABLE bool removeFromCart(int index);
    Q_INVOKABLE bool updateCartQuantity(int index, int quantity);
    Q_INVOKABLE void clearCart();
    
    // Métodos QML invocables - Ventas
    Q_INVOKABLE QVariantMap processSale(const QString& paymentMethod, 
                                         const QString& customerName = QString(),
                                         const QString& notes = QString());
    Q_INVOKABLE bool cancelSale(qint64 saleId, const QString& reason);
    Q_INVOKABLE bool refundSale(qint64 saleId, const QString& reason);
    Q_INVOKABLE QVariantMap getSaleDetails(qint64 saleId);
    
    // Configuración
    Q_INVOKABLE void setTaxPercent(double percent);
    Q_INVOKABLE void setDiscountAmount(double amount);
    Q_INVOKABLE void setExchangeRate(double rate);
    
signals:
    void processingChanged();
    void errorOccurred(const QString& error);
    void saleCompleted(qint64 saleId, double total, QString receiptNumber);
    void saleCancelled(qint64 saleId);
    void saleRefunded(qint64 saleId);
    void cartChanged();
    void productNotFound(const QString& barcode);
    void stockInsufficient(qint64 productId, const QString& productName, int available, int requested);

private:
    SalesService() = default;
    ~SalesService() override = default;
    
    SalesService(const SalesService&) = delete;
    SalesService& operator=(const SalesService&) = delete;
    
    std::unique_ptr<SalesRepository> m_repository;
    std::unique_ptr<CartModel> m_cartModel;
    std::unique_ptr<SalesModel> m_salesModel;
    Inventory::ProductRepository* m_productRepo = nullptr;
    
    bool m_processing = false;
    QString m_lastError;
    double m_exchangeRate = 1.0;
    
    void setProcessing(bool processing);
    void setError(const QString& error);
    bool validateStock(qint64 productId, int requestedQuantity, int& availableStock);
    bool deductStock(qint64 productId, int quantity);
    bool restoreStock(qint64 productId, int quantity);
    Sale createSaleFromCart(const QString& paymentMethod, const QString& customerName, const QString& notes);
};

} // namespace Vento::Sales
