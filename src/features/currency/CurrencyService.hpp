//==============================================================================
// CurrencyService.hpp
// Vento - Servicio de gestión de moneda expuesto a QML
//
// Este servicio actúa como fachada (Facade Pattern) para el módulo Currency
// y usa el patrón Observer a través de Qt Signals/Slots
//==============================================================================
#pragma once

#include <QObject>
#include <QString>
#include <QQmlEngine>
#include <QVariantList>
#include <QVariantMap>
#include <memory>

#include "CurrencyModel.hpp"
#include "CurrencyTypes.hpp"
#include "CurrencyRepository.hpp"

namespace Vento {

class CurrencyService : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    
    //--------------------------------------------------------------------------
    // Propiedades QML
    //--------------------------------------------------------------------------
    
    /// Tasa de cambio actual
    Q_PROPERTY(double exchangeRate 
               READ exchangeRate 
               WRITE setExchangeRate 
               NOTIFY exchangeRateChanged)
    
    /// Tasa formateada para mostrar
    Q_PROPERTY(QString exchangeRateFormatted 
               READ exchangeRateFormatted 
               NOTIFY exchangeRateChanged)
    
    /// Símbolo de moneda local
    Q_PROPERTY(QString localCurrencySymbol 
               READ localCurrencySymbol 
               WRITE setLocalCurrencySymbol 
               NOTIFY localCurrencySymbolChanged)
    
    /// Símbolo de USD
    Q_PROPERTY(QString usdSymbol 
               READ usdSymbol 
               CONSTANT)
    
    /// Última actualización formateada
    Q_PROPERTY(QString lastUpdateFormatted 
               READ lastUpdateFormatted 
               NOTIFY exchangeRateChanged)
    
    /// Porcentaje de cambio respecto a la tasa anterior
    Q_PROPERTY(double changePercent 
               READ changePercent 
               NOTIFY exchangeRateChanged)
    
    /// Indica si la tasa subió, bajó o se mantuvo
    Q_PROPERTY(QString trend 
               READ trend 
               NOTIFY exchangeRateChanged)
    
public:
    //--------------------------------------------------------------------------
    // Singleton
    //--------------------------------------------------------------------------
    static CurrencyService& instance();
    
    /// Para uso con QML singleton
    static CurrencyService* create(QQmlEngine* engine, QJSEngine* scriptEngine);
    
    // Prevent copying
    CurrencyService(const CurrencyService&) = delete;
    CurrencyService& operator=(const CurrencyService&) = delete;
    
    //--------------------------------------------------------------------------
    // Inicialización
    //--------------------------------------------------------------------------
    
    /// Inicializa el servicio (cargar datos de BD)
    void initialize();
    
    //--------------------------------------------------------------------------
    // Propiedades - Getters
    //--------------------------------------------------------------------------
    
    /// Obtiene la tasa de cambio actual
    [[nodiscard]] double exchangeRate() const;
    
    /// Obtiene la tasa formateada (ej: "36.50")
    [[nodiscard]] QString exchangeRateFormatted() const;
    
    /// Obtiene el símbolo de moneda local
    [[nodiscard]] QString localCurrencySymbol() const;
    
    /// Obtiene el símbolo de USD
    [[nodiscard]] QString usdSymbol() const;
    
    /// Obtiene la última actualización formateada
    [[nodiscard]] QString lastUpdateFormatted() const;
    
    /// Obtiene el porcentaje de cambio
    [[nodiscard]] double changePercent() const;
    
    /// Obtiene la tendencia ("up", "down", "stable")
    [[nodiscard]] QString trend() const;
    
    //--------------------------------------------------------------------------
    // Propiedades - Setters
    //--------------------------------------------------------------------------
    
    /// Establece la tasa de cambio
    void setExchangeRate(double rate);
    
    /// Establece el símbolo de moneda local
    void setLocalCurrencySymbol(const QString& symbol);
    
    //--------------------------------------------------------------------------
    // Métodos Q_INVOKABLE (accesibles desde QML)
    //--------------------------------------------------------------------------
    
    /// Actualiza la tasa de cambio manualmente
    Q_INVOKABLE bool updateRate(double newRate);
    
    /// Actualiza la tasa desde una fuente específica
    Q_INVOKABLE bool updateRateFromSource(double newRate, const QString& source);
    
    /// Convierte de USD a moneda local
    Q_INVOKABLE double convertToLocal(double amountUsd) const;
    
    /// Convierte de moneda local a USD
    Q_INVOKABLE double convertToUsd(double amountLocal) const;
    
    /// Formatea un monto en moneda local
    Q_INVOKABLE QString formatLocal(double amount) const;
    
    /// Formatea un monto en USD
    Q_INVOKABLE QString formatUsd(double amount) const;
    
    /// Formatea un monto con símbolo personalizado
    Q_INVOKABLE QString formatAmount(double amount, const QString& symbol, int decimals = 2) const;
    
    /// Calcula el precio de venta basado en costo USD y margen
    Q_INVOKABLE double calculateSalePrice(double costUsd, double marginPercent) const;
    
    /// Obtiene el historial de tasas para gráficos
    Q_INVOKABLE QVariantList getRateHistory(int limit = 30) const;
    
    /// Obtiene información completa de la tasa actual
    Q_INVOKABLE QVariantMap getRateInfo() const;
    
    /// Limpia el historial antiguo
    Q_INVOKABLE bool cleanHistory(int daysToKeep = 90);
    
    /// Refresca los datos desde la BD
    Q_INVOKABLE void refresh();
    
signals:
    //--------------------------------------------------------------------------
    // Señales (Observer Pattern)
    //--------------------------------------------------------------------------
    
    /// Emitida cuando cambia la tasa de cambio
    /// Esta es la señal principal que otros módulos escucharán
    void exchangeRateChanged(double newRate);
    
    /// Emitida con información detallada del cambio
    void rateUpdated(double newRate, double previousRate, double changePercent);
    
    /// Emitida cuando cambia el símbolo de moneda local
    void localCurrencySymbolChanged(const QString& symbol);
    
    /// Emitida cuando hay un error
    void errorOccurred(const QString& error);
    
private:
    //--------------------------------------------------------------------------
    // Constructor privado (Singleton)
    //--------------------------------------------------------------------------
    explicit CurrencyService(QObject* parent = nullptr);
    ~CurrencyService() override = default;
    
    //--------------------------------------------------------------------------
    // Métodos privados
    //--------------------------------------------------------------------------
    
    /// Carga los datos iniciales desde la BD
    void loadFromDatabase();
    
    /// Actualiza la tendencia basada en el cambio
    void updateTrend();
    
    /// Valida una tasa de cambio
    [[nodiscard]] bool isValidRate(double rate) const;
    
    //--------------------------------------------------------------------------
    // Miembros privados
    //--------------------------------------------------------------------------
    std::unique_ptr<CurrencyRepository> m_repository;
    
    // Cache de datos
    double m_exchangeRate = Currency::Defaults::EXCHANGE_RATE_USD;
    QString m_localCurrencySymbol = "Bs.";
    QDateTime m_lastUpdate;
    double m_previousRate = 0.0;
    double m_changePercent = 0.0;
    QString m_trend = "stable";
    
    bool m_initialized = false;
};

} // namespace Vento
