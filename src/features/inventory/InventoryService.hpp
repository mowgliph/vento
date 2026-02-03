//==============================================================================
// InventoryService.hpp
// Vento - Servicio principal del módulo Inventory
//==============================================================================
#pragma once

#include <QObject>
#include <QVariantMap>

#include "Product.hpp"
#include "ProductRepository.hpp"
#include "InventoryTypes.hpp"

namespace Vento::Inventory {

//==============================================================================
// InventoryService - Servicio principal de inventario
//==============================================================================
class InventoryService : public QObject {
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int activeCount READ activeCount NOTIFY activeCountChanged)
    Q_PROPERTY(int lowStockCount READ lowStockCount NOTIFY lowStockCountChanged)

public:
    static InventoryService& instance() {
        static InventoryService inst;
        return inst;
    }

    void initialize();

    //==========================================================================
    // Operaciones de productos
    //==========================================================================
    Q_INVOKABLE QVariantMap createProduct(const QVariantMap& productData);
    Q_INVOKABLE QVariantMap updateProduct(qint64 id, const QVariantMap& updates);
    Q_INVOKABLE QVariantMap getProduct(qint64 id);
    Q_INVOKABLE QVariantMap getProductBySku(const QString& sku);
    Q_INVOKABLE QVariantMap getProductByBarcode(const QString& barcode);
    Q_INVOKABLE QVariantList getAllProducts();
    Q_INVOKABLE QVariantList getActiveProducts();
    Q_INVOKABLE QVariantList getLowStockProducts();
    Q_INVOKABLE QVariantList getOutOfStockProducts();
    Q_INVOKABLE QVariantList getProductsByCategory(const QString& category);
    Q_INVOKABLE QVariantList searchProducts(const QString& query);
    Q_INVOKABLE bool deleteProduct(qint64 id);
    Q_INVOKABLE bool deactivateProduct(qint64 id);
    Q_INVOKABLE bool activateProduct(qint64 id);
    Q_INVOKABLE bool updateStock(qint64 id, int quantity);
    Q_INVOKABLE bool adjustStock(qint64 id, int delta);

    //==========================================================================
    // Consultas y estadísticas
    //==========================================================================
    Q_INVOKABLE int count() const;
    Q_INVOKABLE int activeCount() const;
    Q_INVOKABLE int lowStockCount() const;
    Q_INVOKABLE QVariantList getAllCategories();

    //==========================================================================
    // Conexión con CurrencyService
    //==========================================================================
    Q_INVOKABLE void updatePricesWithExchangeRate(double exchangeRate);
    Q_INVOKABLE void updateProductPrice(qint64 id, double costUsd, double marginPercent, double exchangeRate);

signals:
    void productAdded(const Product& product);
    void productUpdated(const Product& product);
    void productRemoved(qint64 id);
    void countChanged();
    void activeCountChanged();
    void lowStockCountChanged();

private:
    InventoryService() = default;
    ~InventoryService() = default;

    InventoryService(const InventoryService&) = delete;
    InventoryService& operator=(const InventoryService&) = delete;

    ProductRepository* m_repository;
};

} // namespace Vento::Inventory
