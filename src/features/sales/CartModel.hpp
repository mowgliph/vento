//==============================================================================
// CartModel.hpp
// Vento - Modelo del carrito de compras para QML
//==============================================================================
#pragma once

#include <QAbstractListModel>
#include <vector>
#include <optional>

#include "SaleItem.hpp"
#include "SalesTypes.hpp"

namespace Vento::Sales {

//==============================================================================
// CartItem - Item del carrito con stock disponible
//==============================================================================
struct CartItem : public SaleItem {
    int maxStock = 0;  // Stock disponible para validación
    
    bool isValidQuantity() const {
        return quantity > 0 && quantity <= maxStock && quantity <= Defaults::MAX_QUANTITY_PER_ITEM;
    }
    
    bool canIncrease(int amount = 1) const {
        return (quantity + amount) <= maxStock && 
               (quantity + amount) <= Defaults::MAX_QUANTITY_PER_ITEM;
    }
};

//==============================================================================
// CartModel - Modelo QML para el carrito de compras
//==============================================================================
class CartModel : public QAbstractListModel {
    Q_OBJECT
    
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(double subtotal READ subtotal NOTIFY totalsChanged)
    Q_PROPERTY(double taxPercent READ taxPercent WRITE setTaxPercent NOTIFY taxPercentChanged)
    Q_PROPERTY(double taxAmount READ taxAmount NOTIFY totalsChanged)
    Q_PROPERTY(double discountAmount READ discountAmount WRITE setDiscountAmount NOTIFY discountAmountChanged)
    Q_PROPERTY(double total READ total NOTIFY totalsChanged)
    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY countChanged)
    Q_PROPERTY(bool hasItems READ hasItems NOTIFY countChanged)

public:
    explicit CartModel(QObject* parent = nullptr);
    ~CartModel() override;
    
    // QAbstractListModel interface
    [[nodiscard]] int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    
    // Propiedades
    [[nodiscard]] int count() const;
    [[nodiscard]] double subtotal() const;
    [[nodiscard]] double taxPercent() const { return m_taxPercent; }
    void setTaxPercent(double percent);
    [[nodiscard]] double taxAmount() const;
    [[nodiscard]] double discountAmount() const { return m_discountAmount; }
    void setDiscountAmount(double amount);
    [[nodiscard]] double total() const;
    [[nodiscard]] bool isEmpty() const { return m_items.empty(); }
    [[nodiscard]] bool hasItems() const { return !m_items.empty(); }
    
    // Métodos QML invocables
    Q_INVOKABLE bool addProduct(qint64 productId, const QString& name, 
                                 double price, int stock, int quantity = 1);
    Q_INVOKABLE bool removeItem(int index);
    Q_INVOKABLE bool updateQuantity(int index, int quantity);
    Q_INVOKABLE bool increaseQuantity(int index, int amount = 1);
    Q_INVOKABLE bool decreaseQuantity(int index, int amount = 1);
    Q_INVOKABLE void clear();
    Q_INVOKABLE QVariantMap get(int index) const;
    Q_INVOKABLE bool contains(qint64 productId) const;
    Q_INVOKABLE int getQuantity(qint64 productId) const;
    Q_INVOKABLE int getItemIndex(qint64 productId) const;
    
    // Para el servicio de ventas
    std::vector<SaleItem> toSaleItems() const;
    
signals:
    void countChanged();
    void totalsChanged();
    void taxPercentChanged();
    void discountAmountChanged();
    void itemAdded(int index);
    void itemRemoved(int index);
    void itemUpdated(int index);
    void stockError(const QString& message);

private:
    std::vector<CartItem> m_items;
    double m_taxPercent = Defaults::DEFAULT_TAX_PERCENT;
    double m_discountAmount = 0.0;
    
    void calculateSubtotal(CartItem& item);
    void recalculateAll();
    QString formatCurrency(double amount) const;
};

} // namespace Vento::Sales
