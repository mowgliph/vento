//==============================================================================
// DatabaseSchema.hpp
// Vento - Definición del esquema de base de datos
// 
// Principio: Single Responsibility
// Esta clase SOLO define el esquema, no ejecuta nada.
//==============================================================================
#pragma once

#include <QString>
#include <QStringList>
#include <array>

namespace Vento::Database {

//==============================================================================
// Versión del esquema (para futuras migraciones)
//==============================================================================
constexpr int SCHEMA_VERSION = 1;

//==============================================================================
// Nombres de tablas (constantes para evitar errores de tipeo)
//==============================================================================
namespace Tables {
    constexpr auto SETTINGS = "settings";
    constexpr auto PRODUCTS = "products";
    constexpr auto SALES = "sales";
    constexpr auto SALE_ITEMS = "sale_items";  // Para ventas con múltiples productos
    constexpr auto SCHEMA_INFO = "schema_info"; // Metadatos del esquema
}

//==============================================================================
// SQL de creación de tablas
//==============================================================================
namespace Schema {

// Tabla de metadatos del esquema (para control de versiones/migraciones)
constexpr auto CREATE_SCHEMA_INFO = R"SQL(
    CREATE TABLE IF NOT EXISTS schema_info (
        id INTEGER PRIMARY KEY CHECK (id = 1),
        version INTEGER NOT NULL DEFAULT 1,
        created_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
        updated_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime'))
    )
)SQL";

// Tabla de configuraciones (clave-valor)
// Aquí se guarda el precio del dólar y otras configuraciones
constexpr auto CREATE_SETTINGS = R"SQL(
    CREATE TABLE IF NOT EXISTS settings (
        key TEXT PRIMARY KEY NOT NULL,
        value TEXT NOT NULL,
        description TEXT,
        updated_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime'))
    )
)SQL";

// Tabla de productos
constexpr auto CREATE_PRODUCTS = R"SQL(
    CREATE TABLE IF NOT EXISTS products (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        
        -- Información básica
        name TEXT NOT NULL,
        description TEXT,
        sku TEXT UNIQUE,
        barcode TEXT UNIQUE,
        category TEXT,
        
        -- Precios y costos
        cost_usd REAL NOT NULL DEFAULT 0.0,
        cost_local REAL NOT NULL DEFAULT 0.0,
        margin_percent REAL NOT NULL DEFAULT 0.0,
        sale_price REAL NOT NULL DEFAULT 0.0,
        
        -- Inventario
        stock_quantity INTEGER NOT NULL DEFAULT 0,
        min_stock_alert INTEGER DEFAULT 5,
        
        -- Control
        is_active INTEGER NOT NULL DEFAULT 1,
        created_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
        updated_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
        
        -- Constraints
        CHECK (cost_usd >= 0),
        CHECK (cost_local >= 0),
        CHECK (margin_percent >= 0),
        CHECK (sale_price >= 0),
        CHECK (stock_quantity >= 0)
    )
)SQL";

// Tabla de ventas (encabezado)
constexpr auto CREATE_SALES = R"SQL(
    CREATE TABLE IF NOT EXISTS sales (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        
        -- Información de la venta
        sale_date TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
        subtotal REAL NOT NULL DEFAULT 0.0,
        tax_percent REAL NOT NULL DEFAULT 0.0,
        tax_amount REAL NOT NULL DEFAULT 0.0,
        discount_amount REAL NOT NULL DEFAULT 0.0,
        total REAL NOT NULL DEFAULT 0.0,
        
        -- Tasa de cambio al momento de la venta (histórico)
        exchange_rate REAL NOT NULL DEFAULT 1.0,
        
        -- Método de pago
        payment_method TEXT DEFAULT 'cash',
        
        -- Cliente (opcional para futuras expansiones)
        customer_name TEXT,
        customer_id TEXT,
        
        -- Estado
        status TEXT NOT NULL DEFAULT 'completed',
        notes TEXT,
        
        -- Control
        created_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
        
        CHECK (subtotal >= 0),
        CHECK (total >= 0),
        CHECK (status IN ('pending', 'completed', 'cancelled', 'refunded'))
    )
)SQL";

// Tabla de items de venta (detalle)
constexpr auto CREATE_SALE_ITEMS = R"SQL(
    CREATE TABLE IF NOT EXISTS sale_items (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        sale_id INTEGER NOT NULL,
        product_id INTEGER NOT NULL,
        
        -- Snapshot del producto al momento de la venta
        product_name TEXT NOT NULL,
        unit_price REAL NOT NULL,
        quantity INTEGER NOT NULL DEFAULT 1,
        subtotal REAL NOT NULL,
        
        -- Control
        created_at TEXT NOT NULL DEFAULT (datetime('now', 'localtime')),
        
        -- Foreign Keys
        FOREIGN KEY (sale_id) REFERENCES sales(id) ON DELETE CASCADE,
        FOREIGN KEY (product_id) REFERENCES products(id) ON DELETE RESTRICT,
        
        CHECK (quantity > 0),
        CHECK (unit_price >= 0),
        CHECK (subtotal >= 0)
    )
)SQL";

// Índices para optimización
constexpr auto CREATE_INDEXES = R"SQL(
    CREATE INDEX IF NOT EXISTS idx_products_name ON products(name);
    CREATE INDEX IF NOT EXISTS idx_products_sku ON products(sku);
    CREATE INDEX IF NOT EXISTS idx_products_category ON products(category);
    CREATE INDEX IF NOT EXISTS idx_products_active ON products(is_active);
    CREATE INDEX IF NOT EXISTS idx_sales_date ON sales(sale_date);
    CREATE INDEX IF NOT EXISTS idx_sales_status ON sales(status);
    CREATE INDEX IF NOT EXISTS idx_sale_items_sale ON sale_items(sale_id);
    CREATE INDEX IF NOT EXISTS idx_sale_items_product ON sale_items(product_id);
)SQL";

// Datos iniciales
constexpr auto INSERT_DEFAULT_SETTINGS = R"SQL(
    INSERT OR IGNORE INTO settings (key, value, description) VALUES 
        ('exchange_rate_usd', '36.50', 'Tasa de cambio USD a moneda local'),
        ('currency_symbol_local', 'Bs.', 'Símbolo de moneda local'),
        ('currency_symbol_usd', '$', 'Símbolo de USD'),
        ('tax_percent', '16.0', 'Porcentaje de impuesto por defecto'),
        ('store_name', 'Mi Tienda', 'Nombre del negocio'),
        ('app_theme', 'light', 'Tema de la aplicación (light/dark)')
)SQL";

constexpr auto INSERT_SCHEMA_VERSION = R"SQL(
    INSERT OR IGNORE INTO schema_info (id, version) VALUES (1, %1)
)SQL";

} // namespace Schema

//==============================================================================
// Lista ordenada de statements para inicialización
//==============================================================================
inline QStringList getInitializationStatements() {
    return {
        Schema::CREATE_SCHEMA_INFO,
        Schema::CREATE_SETTINGS,
        Schema::CREATE_PRODUCTS,
        Schema::CREATE_SALES,
        Schema::CREATE_SALE_ITEMS
    };
}

inline QStringList getIndexStatements() {
    // Separar cada CREATE INDEX en su propia línea
    QString indexSql = Schema::CREATE_INDEXES;
    return indexSql.split(';', Qt::SkipEmptyParts);
}

inline QStringList getDefaultDataStatements() {
    return {
        Schema::INSERT_DEFAULT_SETTINGS,
        QString(Schema::INSERT_SCHEMA_VERSION).arg(SCHEMA_VERSION)
    };
}

} // namespace Vento::Database
