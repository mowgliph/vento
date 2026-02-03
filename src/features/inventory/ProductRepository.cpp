//==============================================================================
// ProductRepository.cpp
// Vento - Implementación de la capa de persistencia para productos
//==============================================================================
#include "ProductRepository.hpp"

namespace Vento::Inventory {

//==============================================================================
// Operaciones CRUD
//==============================================================================

InventoryResult ProductRepository::save(const Product& product) {
    Product p = product;
    
    if (p.id == 0) {
        p.id = m_nextId++;
        p.createdAt = QDateTime::currentDateTime();
    }
    
    updateTimestamps(p);
    
    if (p.id == 0) {
        return InventoryResult::error("No se pudo generar ID para el producto");
    }
    
    auto it = std::find_if(m_products.begin(), m_products.end(),
        [&p](const Product& existing) { return existing.id == p.id; });
    
    if (it != m_products.end()) {
        *it = p;
    } else {
        m_products.push_back(p);
    }
    
    return InventoryResult::ok(p.id);
}

std::optional<Product> ProductRepository::findById(qint64 id) const {
    auto it = std::find_if(m_products.begin(), m_products.end(),
        [id](const Product& p) { return p.id == id; });
    
    if (it != m_products.end()) {
        return *it;
    }
    return std::nullopt;
}

std::optional<Product> ProductRepository::findBySku(const QString& sku) const {
    auto it = std::find_if(m_products.begin(), m_products.end(),
        [&sku](const Product& p) { return p.sku == sku; });
    
    if (it != m_products.end()) {
        return *it;
    }
    return std::nullopt;
}

std::optional<Product> ProductRepository::findByBarcode(const QString& barcode) const {
    auto it = std::find_if(m_products.begin(), m_products.end(),
        [&barcode](const Product& p) { return p.barcode == barcode; });
    
    if (it != m_products.end()) {
        return *it;
    }
    return std::nullopt;
}

std::vector<Product> ProductRepository::findAll() const {
    return m_products;
}

std::vector<Product> ProductRepository::findActive() const {
    std::vector<Product> result;
    for (const auto& p : m_products) {
        if (p.isActive) {
            result.push_back(p);
        }
    }
    return result;
}

std::vector<Product> ProductRepository::findLowStock() const {
    std::vector<Product> result;
    for (const auto& p : m_products) {
        if (p.isActive && p.isLowStock()) {
            result.push_back(p);
        }
    }
    return result;
}

std::vector<Product> ProductRepository::findOutOfStock() const {
    std::vector<Product> result;
    for (const auto& p : m_products) {
        if (p.isActive && p.isOutOfStock()) {
            result.push_back(p);
        }
    }
    return result;
}

std::vector<Product> ProductRepository::findByCategory(const QString& category) const {
    std::vector<Product> result;
    for (const auto& p : m_products) {
        if (p.isActive && p.category == category) {
            result.push_back(p);
        }
    }
    return result;
}

std::vector<Product> ProductRepository::searchByName(const QString& query) const {
    std::vector<Product> result;
    QString lowerQuery = query.toLower();
    
    for (const auto& p : m_products) {
        if (p.isActive && p.name.toLower().contains(lowerQuery)) {
            result.push_back(p);
        }
    }
    return result;
}

InventoryResult ProductRepository::remove(qint64 id) {
    auto it = std::find_if(m_products.begin(), m_products.end(),
        [id](const Product& p) { return p.id == id; });
    
    if (it != m_products.end()) {
        m_products.erase(it);
        return InventoryResult::ok(id);
    }
    
    return InventoryResult::error("Producto no encontrado");
}

InventoryResult ProductRepository::deactivate(qint64 id) {
    auto it = std::find_if(m_products.begin(), m_products.end(),
        [id](const Product& p) { return p.id == id; });
    
    if (it != m_products.end()) {
        it->isActive = false;
        it->updatedAt = QDateTime::currentDateTime();
        return InventoryResult::ok(id);
    }
    
    return InventoryResult::error("Producto no encontrado");
}

InventoryResult ProductRepository::activate(qint64 id) {
    auto it = std::find_if(m_products.begin(), m_products.end(),
        [id](const Product& p) { return p.id == id; });
    
    if (it != m_products.end()) {
        it->isActive = true;
        it->updatedAt = QDateTime::currentDateTime();
        return InventoryResult::ok(id);
    }
    
    return InventoryResult::error("Producto no encontrado");
}

InventoryResult ProductRepository::updateStock(qint64 id, int quantity) {
    auto it = std::find_if(m_products.begin(), m_products.end(),
        [id](const Product& p) { return p.id == id; });
    
    if (it != m_products.end()) {
        it->stockQuantity = quantity;
        it->updatedAt = QDateTime::currentDateTime();
        return InventoryResult::ok(id);
    }
    
    return InventoryResult::error("Producto no encontrado");
}

int ProductRepository::count() const {
    return static_cast<int>(m_products.size());
}

int ProductRepository::countActive() const {
    return static_cast<int>(std::count_if(m_products.begin(), m_products.end(),
        [](const Product& p) { return p.isActive; }));
}

int ProductRepository::countLowStock() const {
    return static_cast<int>(std::count_if(m_products.begin(), m_products.end(),
        [](const Product& p) { return p.isActive && p.isLowStock(); }));
}

void ProductRepository::clear() {
    m_products.clear();
    m_nextId = 1;
}

//==============================================================================
// Utilidades
//==============================================================================

bool ProductRepository::existsBySku(const QString& sku) const {
    return std::any_of(m_products.begin(), m_products.end(),
        [&sku](const Product& p) { return p.sku == sku; });
}

bool ProductRepository::existsByBarcode(const QString& barcode) const {
    return std::any_of(m_products.begin(), m_products.end(),
        [&barcode](const Product& p) { return p.barcode == barcode; });
}

std::vector<QString> ProductRepository::getAllCategories() const {
    std::set<QString> categories;
    for (const auto& p : m_products) {
        if (p.isActive && !p.category.isEmpty()) {
            categories.insert(p.category);
        }
    }
    return std::vector<QString>(categories.begin(), categories.end());
}

void ProductRepository::updateTimestamps(Product& product) {
    product.updatedAt = QDateTime::currentDateTime();
}

} // namespace Vento::Inventory
