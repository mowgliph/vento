//==============================================================================
// ProductRepository.hpp
// Vento - Capa de persistencia para productos
//==============================================================================
#pragma once

#include <vector>
#include <memory>
#include <optional>

#include "Product.hpp"
#include "InventoryTypes.hpp"

namespace Vento::Inventory {

//==============================================================================
// ProductRepository - Almacén persistente de productos
//==============================================================================
class ProductRepository {
public:
    ProductRepository() = default;
    ~ProductRepository() = default;
    
    ProductRepository(const ProductRepository&) = delete;
    ProductRepository& operator=(const ProductRepository&) = delete;
    ProductRepository(ProductRepository&&) = default;
    ProductRepository& operator=(ProductRepository&&) = default;
    
    //==========================================================================
    // Operaciones CRUD
    //==========================================================================
    
    /// Guarda un nuevo producto o actualiza uno existente
    InventoryResult save(const Product& product);
    
    /// Busca un producto por ID
    std::optional<Product> findById(qint64 id) const;
    
    /// Busca un producto por SKU
    std::optional<Product> findBySku(const QString& sku) const;
    
    /// Busca un producto por código de barras
    std::optional<Product> findByBarcode(const QString& barcode) const;
    
    /// Obtiene todos los productos
    std::vector<Product> findAll() const;
    
    /// Obtiene productos activos
    std::vector<Product> findActive() const;
    
    /// Obtiene productos con stock bajo
    std::vector<Product> findLowStock() const;
    
    /// Obtiene productos agotados
    std::vector<Product> findOutOfStock() const;
    
    /// Obtiene productos por categoría
    std::vector<Product> findByCategory(const QString& category) const;
    
    /// Busca productos por nombre (búsqueda parcial)
    std::vector<Product> searchByName(const QString& query) const;
    
    /// Elimina un producto por ID
    InventoryResult remove(qint64 id);
    
    /// Marca un producto como inactivo
    InventoryResult deactivate(qint64 id);
    
    /// Reactiva un producto
    InventoryResult activate(qint64 id);
    
    /// Actualiza el stock de un producto
    InventoryResult updateStock(qint64 id, int quantity);
    
    /// Obtiene el conteo total de productos
    [[nodiscard]] int count() const;
    
    /// Obtiene el conteo de productos activos
    [[nodiscard]] int countActive() const;
    
    /// Obtiene el conteo de productos con stock bajo
    [[nodiscard]] int countLowStock() const;
    
    /// Limpia todos los productos
    void clear();
    
    //==========================================================================
    // Utilidades
    //==========================================================================
    
    /// Verifica si existe un producto con el SKU dado
    [[nodiscard]] bool existsBySku(const QString& sku) const;
    
    /// Verifica si existe un producto con el código de barras dado
    [[nodiscard]] bool existsByBarcode(const QString& barcode) const;
    
    /// Obtiene todas las categorías únicas
    [[nodiscard]] std::vector<QString> getAllCategories() const;
    
private:
    std::vector<Product> m_products;
    qint64 m_nextId = 1;
    
    void updateTimestamps(Product& product);
};

} // namespace Vento::Inventory
