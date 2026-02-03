//==============================================================================
// SalesService.cpp
// Vento - Implementación del servicio principal de ventas
//==============================================================================
#include "SalesService.hpp"

#include <QDebug>
#include "../inventory/ProductRepository.hpp"
#include "../inventory/Product.hpp"

namespace Vento::Sales {

//==============================================================================
// Singleton Instance
//==============================================================================

SalesService& SalesService::instance() {
    static SalesService inst;
    return inst;
}

//==============================================================================
// Initialization
//==============================================================================

void SalesService::initialize(Inventory::ProductRepository* productRepo) {
    m_productRepo = productRepo;
    m_repository = std::make_unique<SalesRepository>();
    m_cartModel = std::make_unique<CartModel>();
    m_salesModel = std::make_unique<SalesModel>();
    
    qDebug() << "SalesService initialized";
}

//==============================================================================
// Cart Operations
//==============================================================================

bool SalesService::addToCart(qint64 productId, int quantity) {
    if (!m_productRepo) {
        setError(QObject::tr("Servicio de inventario no disponible"));
        return false;
    }
    
    if (quantity <= 0) quantity = 1;
    
    auto productOpt = m_productRepo->findById(productId);
    if (!productOpt) {
        setError(QObject::tr("Producto no encontrado"));
        return false;
    }
    
    const auto& product = *productOpt;
    
    if (!product.isActive) {
        setError(QObject::tr("Producto no está activo"));
        return false;
    }
    
    if (product.stockQuantity <= 0) {
        setError(QObject::tr("Producto sin stock"));
        emit stockInsufficient(productId, product.name, 0, quantity);
        return false;
    }
    
    // Check current quantity in cart
    int currentInCart = m_cartModel->getQuantity(productId);
    int totalRequested = currentInCart + quantity;
    
    if (totalRequested > product.stockQuantity) {
        emit stockInsufficient(productId, product.name, product.stockQuantity, totalRequested);
        return false;
    }
    
    bool success = m_cartModel->addProduct(
        productId,
        product.name,
        product.salePrice,
        product.stockQuantity,
        quantity
    );
    
    if (success) {
        emit cartChanged();
    }
    
    return success;
}

bool SalesService::addToCartByBarcode(const QString& barcode, int quantity) {
    if (!m_productRepo) {
        setError(QObject::tr("Servicio de inventario no disponible"));
        return false;
    }
    
    auto productOpt = m_productRepo->findByBarcode(barcode);
    if (!productOpt) {
        emit productNotFound(barcode);
        setError(QObject::tr("Producto no encontrado: %1").arg(barcode));
        return false;
    }
    
    return addToCart(productOpt->id, quantity);
}

bool SalesService::removeFromCart(int index) {
    bool success = m_cartModel->removeItem(index);
    if (success) {
        emit cartChanged();
    }
    return success;
}

bool SalesService::updateCartQuantity(int index, int quantity) {
    if (!m_productRepo || index < 0 || index >= m_cartModel->count()) {
        return false;
    }
    
    auto item = m_cartModel->get(index);
    qint64 productId = item["productId"].toLongLong();
    
    auto productOpt = m_productRepo->findById(productId);
    if (!productOpt) {
        return false;
    }
    
    if (quantity > productOpt->stockQuantity) {
        emit stockInsufficient(productId, productOpt->name, productOpt->stockQuantity, quantity);
        return false;
    }
    
    bool success = m_cartModel->updateQuantity(index, quantity);
    if (success) {
        emit cartChanged();
    }
    return success;
}

void SalesService::clearCart() {
    m_cartModel->clear();
    emit cartChanged();
}

//==============================================================================
// Sale Operations
//==============================================================================

QVariantMap SalesService::processSale(const QString& paymentMethod, 
                                       const QString& customerName,
                                       const QString& notes) {
    setProcessing(true);
    setError(QString());
    
    QVariantMap result;
    
    // Validate cart
    if (m_cartModel->isEmpty()) {
        setError(QObject::tr("El carrito está vacío"));
        setProcessing(false);
        result["success"] = false;
        result["error"] = m_lastError;
        return result;
    }
    
    // Validate stock one more time
    auto items = m_cartModel->toSaleItems();
    for (const auto& item : items) {
        int availableStock = 0;
        if (!validateStock(item.productId, item.quantity, availableStock)) {
            setError(QObject::tr("Stock insuficiente para: %1").arg(item.productName));
            setProcessing(false);
            result["success"] = false;
            result["error"] = m_lastError;
            return result;
        }
    }
    
    // Create sale
    Sale sale = createSaleFromCart(paymentMethod, customerName, notes);
    
    // Deduct stock
    for (const auto& item : items) {
        if (!deductStock(item.productId, item.quantity)) {
            // Rollback previous deductions
            for (const auto& prevItem : items) {
                if (prevItem.productId == item.productId) break;
                restoreStock(prevItem.productId, prevItem.quantity);
            }
            setError(QObject::tr("Error al actualizar stock"));
            setProcessing(false);
            result["success"] = false;
            result["error"] = m_lastError;
            return result;
        }
    }
    
    // Save sale
    SaleResult saveResult = m_repository->save(sale);
    
    if (!saveResult) {
        // Restore stock on failure
        for (const auto& item : items) {
            restoreStock(item.productId, item.quantity);
        }
        setError(saveResult.errorMessage);
        setProcessing(false);
        result["success"] = false;
        result["error"] = m_lastError;
        return result;
    }
    
    // Clear cart on success
    clearCart();
    
    // Refresh sales model
    m_salesModel->refresh();
    
    setProcessing(false);
    
    emit saleCompleted(sale.id, sale.total, sale.receiptNumber);
    
    result["success"] = true;
    result["saleId"] = sale.id;
    result["total"] = sale.total;
    result["receiptNumber"] = sale.receiptNumber;
    return result;
}

bool SalesService::cancelSale(qint64 saleId, const QString& reason) {
    setProcessing(true);
    
    auto saleOpt = m_repository->findById(saleId);
    if (!saleOpt) {
        setError(QObject::tr("Venta no encontrada"));
        setProcessing(false);
        return false;
    }
    
    if (!saleOpt->canBeCancelled()) {
        setError(QObject::tr("Esta venta no puede ser cancelada"));
        setProcessing(false);
        return false;
    }
    
    // Restore stock
    for (const auto& item : saleOpt->items) {
        restoreStock(item.productId, item.quantity);
    }
    
    SaleResult result = m_repository->cancel(saleId, reason);
    
    if (result) {
        m_salesModel->refresh();
        emit saleCancelled(saleId);
    } else {
        setError(result.errorMessage);
    }
    
    setProcessing(false);
    return result.success;
}

bool SalesService::refundSale(qint64 saleId, const QString& reason) {
    setProcessing(true);
    
    auto saleOpt = m_repository->findById(saleId);
    if (!saleOpt) {
        setError(QObject::tr("Venta no encontrada"));
        setProcessing(false);
        return false;
    }
    
    if (!saleOpt->canBeRefunded()) {
        setError(QObject::tr("Esta venta no puede ser reembolsada"));
        setProcessing(false);
        return false;
    }
    
    // Restore stock
    for (const auto& item : saleOpt->items) {
        restoreStock(item.productId, item.quantity);
    }
    
    SaleResult result = m_repository->refund(saleId, reason);
    
    if (result) {
        m_salesModel->refresh();
        emit saleRefunded(saleId);
    } else {
        setError(result.errorMessage);
    }
    
    setProcessing(false);
    return result.success;
}

QVariantMap SalesService::getSaleDetails(qint64 saleId) {
    auto saleOpt = m_repository->findById(saleId);
    if (!saleOpt) {
        return {};
    }
    return saleOpt->toVariantMap();
}

//==============================================================================
// Configuration
//==============================================================================

void SalesService::setTaxPercent(double percent) {
    if (m_cartModel) {
        m_cartModel->setTaxPercent(percent);
    }
}

void SalesService::setDiscountAmount(double amount) {
    if (m_cartModel) {
        m_cartModel->setDiscountAmount(amount);
    }
}

void SalesService::setExchangeRate(double rate) {
    if (rate > 0) {
        m_exchangeRate = rate;
    }
}

//==============================================================================
// Private Methods
//==============================================================================

void SalesService::setProcessing(bool processing) {
    if (m_processing != processing) {
        m_processing = processing;
        emit processingChanged();
    }
}

void SalesService::setError(const QString& error) {
    m_lastError = error;
    if (!error.isEmpty()) {
        emit errorOccurred(error);
    }
}

bool SalesService::validateStock(qint64 productId, int requestedQuantity, int& availableStock) {
    if (!m_productRepo) return false;
    
    auto productOpt = m_productRepo->findById(productId);
    if (!productOpt) return false;
    
    availableStock = productOpt->stockQuantity;
    return requestedQuantity <= availableStock;
}

bool SalesService::deductStock(qint64 productId, int quantity) {
    if (!m_productRepo) return false;
    
    auto productOpt = m_productRepo->findById(productId);
    if (!productOpt) return false;
    
    int newStock = productOpt->stockQuantity - quantity;
    if (newStock < 0) return false;
    
    auto result = m_productRepo->updateStock(productId, newStock);
    return result.success;
}

bool SalesService::restoreStock(qint64 productId, int quantity) {
    if (!m_productRepo) return false;
    
    auto productOpt = m_productRepo->findById(productId);
    if (!productOpt) return false;
    
    int newStock = productOpt->stockQuantity + quantity;
    auto result = m_productRepo->updateStock(productId, newStock);
    return result.success;
}

Sale SalesService::createSaleFromCart(const QString& paymentMethod, 
                                       const QString& customerName, 
                                       const QString& notes) {
    Sale sale;
    sale.saleDate = QDateTime::currentDateTime();
    sale.paymentMethod = stringToPayment(paymentMethod);
    sale.customerName = customerName;
    sale.notes = notes;
    sale.status = SaleStatus::Completed;
    sale.exchangeRate = m_exchangeRate;
    sale.items = m_cartModel->toSaleItems();
    sale.taxPercent = m_cartModel->taxPercent();
    sale.discountAmount = m_cartModel->discountAmount();
    sale.calculateTotals();
    
    return sale;
}

} // namespace Vento::Sales
