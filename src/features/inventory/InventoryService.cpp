//==============================================================================
// InventoryService.cpp
// Vento - Implementación del servicio principal de inventario
//==============================================================================
#include "InventoryService.hpp"
#include <algorithm>

namespace Vento::Inventory {

void InventoryService::initialize() {
    m_repository = new ProductRepository();
}

QVariantMap InventoryService::createProduct(const QVariantMap& productData) {
    Product product = Product::fromVariantMap(productData);

    if (!product.isValid()) {
        return {{"success", false}, {"error", product.validationError()}};
    }

    if (!product.sku.isEmpty() && m_repository->existsBySku(product.sku)) {
        return {{"success", false}, {"error", "El SKU ya existe"}};
    }

    if (!product.barcode.isEmpty() && m_repository->existsByBarcode(product.barcode)) {
        return {{"success", false}, {"error", "El código de barras ya existe"}};
    }

    InventoryResult result = m_repository->save(product);
    if (!result) {
        return {{"success", false}, {"error", result.errorMessage}};
    }

    emit productAdded(product);
    emit countChanged();
    emit activeCountChanged();

    return {{"success", true}, {"id", product.id}, {"product", product.toVariantMap()}};
}

QVariantMap InventoryService::updateProduct(qint64 id, const QVariantMap& updates) {
    std::optional<Product> existing = m_repository->findById(id);
    if (!existing) {
        return {{"success", false}, {"error", "Producto no encontrado"}};
    }

    Product product = *existing;

    if (updates.contains("name")) product.name = updates["name"].toString();
    if (updates.contains("description")) product.description = updates["description"].toString();
    if (updates.contains("sku")) product.sku = updates["sku"].toString();
    if (updates.contains("barcode")) product.barcode = updates["barcode"].toString();
    if (updates.contains("category")) product.category = updates["category"].toString();
    if (updates.contains("costUsd")) product.costUsd = updates["costUsd"].toDouble();
    if (updates.contains("marginPercent")) product.marginPercent = updates["marginPercent"].toDouble();
    if (updates.contains("minStockAlert")) product.minStockAlert = updates["minStockAlert"].toInt();

    if (!product.isValid()) {
        return {{"success", false}, {"error", product.validationError()}};
    }

    InventoryResult result = m_repository->save(product);
    if (!result) {
        return {{"success", false}, {"error", result.errorMessage}};
    }

    emit productUpdated(product);
    return {{"success", true}, {"product", product.toVariantMap()}};
}

QVariantMap InventoryService::getProduct(qint64 id) {
    std::optional<Product> product = m_repository->findById(id);
    if (!product) {
        return {{"success", false}, {"error", "Producto no encontrado"}};
    }
    return {{"success", true}, {"product", product->toVariantMap()}};
}

QVariantMap InventoryService::getProductBySku(const QString& sku) {
    std::optional<Product> product = m_repository->findBySku(sku);
    if (!product) {
        return {{"success", false}, {"error", "Producto no encontrado"}};
    }
    return {{"success", true}, {"product", product->toVariantMap()}};
}

QVariantMap InventoryService::getProductByBarcode(const QString& barcode) {
    std::optional<Product> product = m_repository->findByBarcode(barcode);
    if (!product) {
        return {{"success", false}, {"error", "Producto no encontrado"}};
    }
    return {{"success", true}, {"product", product->toVariantMap()}};
}

QVariantList InventoryService::getAllProducts() {
    QVariantList result;
    for (const auto& product : m_repository->findAll()) {
        result.append(product.toVariantMap());
    }
    return result;
}

QVariantList InventoryService::getActiveProducts() {
    QVariantList result;
    for (const auto& product : m_repository->findActive()) {
        result.append(product.toVariantMap());
    }
    return result;
}

QVariantList InventoryService::getLowStockProducts() {
    QVariantList result;
    for (const auto& product : m_repository->findLowStock()) {
        result.append(product.toVariantMap());
    }
    return result;
}

QVariantList InventoryService::getOutOfStockProducts() {
    QVariantList result;
    for (const auto& product : m_repository->findOutOfStock()) {
        result.append(product.toVariantMap());
    }
    return result;
}

QVariantList InventoryService::getProductsByCategory(const QString& category) {
    QVariantList result;
    for (const auto& product : m_repository->findByCategory(category)) {
        result.append(product.toVariantMap());
    }
    return result;
}

QVariantList InventoryService::searchProducts(const QString& query) {
    QVariantList result;
    for (const auto& product : m_repository->searchByName(query)) {
        result.append(product.toVariantMap());
    }
    return result;
}

bool InventoryService::deleteProduct(qint64 id) {
    InventoryResult result = m_repository->remove(id);
    if (result) {
        emit productRemoved(id);
        emit countChanged();
        emit activeCountChanged();
    }
    return result;
}

bool InventoryService::deactivateProduct(qint64 id) {
    InventoryResult result = m_repository->deactivate(id);
    if (result) {
        std::optional<Product> product = m_repository->findById(id);
        if (product) {
            emit productUpdated(*product);
        }
        emit activeCountChanged();
    }
    return result;
}

bool InventoryService::activateProduct(qint64 id) {
    InventoryResult result = m_repository->activate(id);
    if (result) {
        std::optional<Product> product = m_repository->findById(id);
        if (product) {
            emit productUpdated(*product);
        }
        emit activeCountChanged();
    }
    return result;
}

bool InventoryService::updateStock(qint64 id, int quantity) {
    InventoryResult result = m_repository->updateStock(id, quantity);
    if (result) {
        std::optional<Product> product = m_repository->findById(id);
        if (product) {
            emit productUpdated(*product);
            emit lowStockCountChanged();
        }
    }
    return result;
}

bool InventoryService::adjustStock(qint64 id, int delta) {
    std::optional<Product> existing = m_repository->findById(id);
    if (!existing) {
        return false;
    }

    int newQuantity = existing->stockQuantity + delta;
    if (newQuantity < 0) {
        newQuantity = 0;
    }

    return updateStock(id, newQuantity);
}

int InventoryService::count() const {
    return m_repository->count();
}

int InventoryService::activeCount() const {
    return m_repository->countActive();
}

int InventoryService::lowStockCount() const {
    return m_repository->countLowStock();
}

QVariantList InventoryService::getAllCategories() {
    QVariantList result;
    for (const auto& category : m_repository->getAllCategories()) {
        result.append(category);
    }
    return result;
}

void InventoryService::updatePricesWithExchangeRate(double exchangeRate) {
    for (auto& product : m_repository->findAll()) {
        product.calculateSalePrice(exchangeRate);
        m_repository->save(product);
        emit productUpdated(product);
    }
}

void InventoryService::updateProductPrice(qint64 id, double costUsd, double marginPercent, double exchangeRate) {
    std::optional<Product> existing = m_repository->findById(id);
    if (!existing) {
        return;
    }

    Product product = *existing;
    product.costUsd = costUsd;
    product.marginPercent = marginPercent;
    product.calculateSalePrice(exchangeRate);

    m_repository->save(product);
    emit productUpdated(product);
}

} // namespace Vento::Inventory
