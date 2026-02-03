//==============================================================================
// EventBus.hpp
// Implementación del patrón Observer para comunicación entre features
//==============================================================================
#pragma once

#include <QObject>
#include <QVariant>
#include <functional>
#include <unordered_map>
#include <vector>
#include <string>

namespace Vento {

class EventBus : public QObject {
    Q_OBJECT
    
public:
    using EventHandler = std::function<void(const QVariant&)>;
    using SubscriptionId = size_t;
    
    static EventBus& instance() {
        static EventBus instance;
        return instance;
    }
    
    // Suscribirse a un evento
    SubscriptionId subscribe(const std::string& eventName, EventHandler handler) {
        auto id = m_nextId++;
        m_subscribers[eventName].push_back({id, std::move(handler)});
        return id;
    }
    
    // Publicar un evento
    void publish(const std::string& eventName, const QVariant& data = {}) {
        if (auto it = m_subscribers.find(eventName); it != m_subscribers.end()) {
            for (const auto& [id, handler] : it->second) {
                handler(data);
            }
        }
    }
    
    // Cancelar suscripción
    void unsubscribe(const std::string& eventName, SubscriptionId id) {
        if (auto it = m_subscribers.find(eventName); it != m_subscribers.end()) {
            auto& handlers = it->second;
            handlers.erase(
                std::remove_if(handlers.begin(), handlers.end(),
                    [id](const auto& pair) { return pair.first == id; }),
                handlers.end()
            );
        }
    }
    
signals:
    void currencyRateChanged(double newRate);
    void inventoryUpdated();
    void saleCompleted(int saleId);
    
private:
    EventBus() = default;
    
    std::unordered_map<std::string, 
        std::vector<std::pair<SubscriptionId, EventHandler>>> m_subscribers;
    SubscriptionId m_nextId = 0;
};

// Eventos predefinidos
namespace Events {
    constexpr auto CURRENCY_RATE_CHANGED = "currency.rate.changed";
    constexpr auto INVENTORY_UPDATED = "inventory.updated";
    constexpr auto SALE_COMPLETED = "sale.completed";
}

} // namespace Vento
