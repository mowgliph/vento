//==============================================================================
// CartModel.cpp
// Vento - Implementación del modelo del carrito de compras
//==============================================================================
#include "CartModel.hpp"

#include <algorithm>

namespace Vento::Sales {

//==============================================================================
// Constructor/Destructor
//==============================================================================

CartModel::CartModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

CartModel::~CartModel() = default;

//==============================================================================
// QAbstractListModel Interface
//==============================================================================

int CartModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) return 0;
    return static_cast<int>(m_items.size());
}

QVariant CartModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(m_items.size())) {
        return {};
    }
    
    const CartItem& item = m_items[index.row()];
    
    switch (role) {
        case Roles::ProductIdRole:
            return QVariant::fromValue(item.productId);
        case Roles::ProductNameRole:
            return item.productName;
        case Roles::UnitPriceRole:
            return item.unitPrice;
        case Roles::QuantityRole:
            return item.quantity;
        case Roles::SubtotalRole:
            return item.subtotal;
        case Roles::FormattedUnitPriceRole:
            return formatCurrency(item.unitPrice);
        case Roles::FormattedSubtotalRole:
            return formatCurrency(item.subtotal);
        case Roles::MaxStockRole:
            return item.maxStock;
        default:
            return {};
    }
}

QHash<int, QByteArray> CartModel::roleNames() const {
    return {
        {Roles::ProductIdRole, "productId"},
        {Roles::ProductNameRole, "productName"},
        {Roles::UnitPriceRole, "unitPrice"},
        {Roles::QuantityRole, "quantity"},
        {Roles::SubtotalRole, "subtotal"},
        {Roles::FormattedUnitPriceRole, "formattedUnitPrice"},
        {Roles::FormattedSubtotalRole, "formattedSubtotal"},
        {Roles::MaxStockRole, "maxStock"}
    };
}

Qt::ItemFlags CartModel::flags(const QModelIndex& index) const {
    if (!index.isValid()) return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool CartModel::setData(const QModelIndex& index, const QVariant& value, int role) {
    if (!index.isValid() || index.row() >= static_cast<int>(m_items.size())) {
        return false;
    }
    
    if (role == Roles::QuantityRole) {
        int newQuantity = value.toInt();
        return updateQuantity(index.row(), newQuantity);
    }
    
    return false;
}

//==============================================================================
// Propiedades
//==============================================================================

int CartModel::count() const {
    return static_cast<int>(m_items.size());
}

double CartModel::subtotal() const {
    double sum = 0.0;
    for (const auto& item : m_items) {
        sum += item.subtotal;
    }
    return sum;
}

void CartModel::setTaxPercent(double percent) {
    if (m_taxPercent != percent) {
        m_taxPercent = percent;
        emit taxPercentChanged();
        emit totalsChanged();
    }
}

double CartModel::taxAmount() const {
    return subtotal() * (m_taxPercent / 100.0);
}

void CartModel::setDiscountAmount(double amount) {
    if (m_discountAmount != amount) {
        m_discountAmount = amount;
        emit discountAmountChanged();
        emit totalsChanged();
    }
}

double CartModel::total() const {
    double t = subtotal() + taxAmount() - m_discountAmount;
    return t < 0.0 ? 0.0 : t;
}

//==============================================================================
// Métodos QML Invocables
//==============================================================================

bool CartModel::addProduct(qint64 productId, const QString& name, 
                            double price, int stock, int quantity) {
    // Validar stock
    if (stock <= 0) {
        emit stockError(QObject::tr("Producto sin stock disponible"));
        return false;
    }
    
    if (quantity <= 0) quantity = 1;
    if (quantity > stock) quantity = stock;
    if (quantity > Defaults::MAX_QUANTITY_PER_ITEM) {
        quantity = Defaults::MAX_QUANTITY_PER_ITEM;
    }
    
    // Buscar si ya existe
    auto it = std::find_if(m_items.begin(), m_items.end(),
        [productId](const CartItem& item) { return item.productId == productId; });
    
    if (it != m_items.end()) {
        // Actualizar cantidad existente
        int index = static_cast<int>(it - m_items.begin());
        int newQuantity = it->quantity + quantity;
        
        if (newQuantity > it->maxStock) {
            emit stockError(QObject::tr("No hay suficiente stock disponible"));
            return false;
        }
        
        return updateQuantity(index, newQuantity);
    }
    
    // Agregar nuevo item
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    
    CartItem item;
    item.productId = productId;
    item.productName = name;
    item.unitPrice = price;
    item.quantity = quantity;
    item.maxStock = stock;
    calculateSubtotal(item);
    
    m_items.push_back(item);
    
    endInsertRows();
    
    emit countChanged();
    emit totalsChanged();
    emit itemAdded(m_items.size() - 1);
    
    return true;
}

bool CartModel::removeItem(int index) {
    if (index < 0 || index >= static_cast<int>(m_items.size())) {
        return false;
    }
    
    beginRemoveRows(QModelIndex(), index, index);
    m_items.erase(m_items.begin() + index);
    endRemoveRows();
    
    emit countChanged();
    emit totalsChanged();
    emit itemRemoved(index);
    
    return true;
}

bool CartModel::updateQuantity(int index, int quantity) {
    if (index < 0 || index >= static_cast<int>(m_items.size())) {
        return false;
    }
    
    CartItem& item = m_items[index];
    
    if (quantity <= 0) {
        return removeItem(index);
    }
    
    if (quantity > item.maxStock) {
        emit stockError(QObject::tr("Cantidad excede el stock disponible (%1)").arg(item.maxStock));
        return false;
    }
    
    if (quantity > Defaults::MAX_QUANTITY_PER_ITEM) {
        quantity = Defaults::MAX_QUANTITY_PER_ITEM;
    }
    
    item.quantity = quantity;
    calculateSubtotal(item);
    
    emit dataChanged(this->index(index), this->index(index), {Roles::QuantityRole, Roles::SubtotalRole, Roles::FormattedSubtotalRole});
    emit totalsChanged();
    emit itemUpdated(index);
    
    return true;
}

bool CartModel::increaseQuantity(int index, int amount) {
    if (index < 0 || index >= static_cast<int>(m_items.size())) {
        return false;
    }
    
    const CartItem& item = m_items[index];
    return updateQuantity(index, item.quantity + amount);
}

bool CartModel::decreaseQuantity(int index, int amount) {
    if (index < 0 || index >= static_cast<int>(m_items.size())) {
        return false;
    }
    
    const CartItem& item = m_items[index];
    return updateQuantity(index, item.quantity - amount);
}

void CartModel::clear() {
    if (m_items.empty()) return;
    
    beginResetModel();
    m_items.clear();
    endResetModel();
    
    emit countChanged();
    emit totalsChanged();
}

QVariantMap CartModel::get(int index) const {
    if (index < 0 || index >= static_cast<int>(m_items.size())) {
        return {};
    }
    
    const CartItem& item = m_items[index];
    return {
        {"productId", item.productId},
        {"productName", item.productName},
        {"unitPrice", item.unitPrice},
        {"quantity", item.quantity},
        {"subtotal", item.subtotal},
        {"maxStock", item.maxStock},
        {"formattedUnitPrice", formatCurrency(item.unitPrice)},
        {"formattedSubtotal", formatCurrency(item.subtotal)}
    };
}

bool CartModel::contains(qint64 productId) const {
    return std::any_of(m_items.begin(), m_items.end(),
        [productId](const CartItem& item) { return item.productId == productId; });
}

int CartModel::getQuantity(qint64 productId) const {
    auto it = std::find_if(m_items.begin(), m_items.end(),
        [productId](const CartItem& item) { return item.productId == productId; });
    
    if (it != m_items.end()) {
        return it->quantity;
    }
    return 0;
}

int CartModel::getItemIndex(qint64 productId) const {
    auto it = std::find_if(m_items.begin(), m_items.end(),
        [productId](const CartItem& item) { return item.productId == productId; });
    
    if (it != m_items.end()) {
        return static_cast<int>(it - m_items.begin());
    }
    return -1;
}

//==============================================================================
// Para el servicio de ventas
//==============================================================================

std::vector<SaleItem> CartModel::toSaleItems() const {
    std::vector<SaleItem> result;
    result.reserve(m_items.size());
    
    for (const auto& item : m_items) {
        SaleItem saleItem;
        saleItem.productId = item.productId;
        saleItem.productName = item.productName;
        saleItem.unitPrice = item.unitPrice;
        saleItem.quantity = item.quantity;
        saleItem.subtotal = item.subtotal;
        result.push_back(saleItem);
    }
    
    return result;
}

//==============================================================================
// Métodos Privados
//==============================================================================

void CartModel::calculateSubtotal(CartItem& item) {
    item.subtotal = item.unitPrice * item.quantity;
}

void CartModel::recalculateAll() {
    for (auto& item : m_items) {
        calculateSubtotal(item);
    }
}

QString CartModel::formatCurrency(double amount) const {
    return QString("$ %1").arg(QString::number(amount, 'f', 2));
}

} // namespace Vento::Sales
