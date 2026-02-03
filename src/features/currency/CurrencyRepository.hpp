//==============================================================================
// CurrencyRepository.hpp
// Vento - Repositorio para persistencia de datos de moneda
//
// Principio: Single Responsibility
// Esta clase SOLO se encarga de operaciones de base de datos
//==============================================================================
#pragma once

#include <QObject>
#include <QString>
#include <QList>
#include <optional>

#include "CurrencyModel.hpp"
#include "CurrencyTypes.hpp"

namespace Vento {

class CurrencyRepository : public QObject {
    Q_OBJECT
    
public:
    explicit CurrencyRepository(QObject* parent = nullptr);
    ~CurrencyRepository() override = default;
    
    //--------------------------------------------------------------------------
    // Operaciones de tasa de cambio
    //--------------------------------------------------------------------------
    
    /// Guarda la tasa de cambio actual
    /// @param rate Nueva tasa de cambio
    /// @param source Fuente de la actualización
    /// @return true si se guardó correctamente
    [[nodiscard]] bool saveExchangeRate(
        double rate, 
        Currency::RateUpdateSource source = Currency::RateUpdateSource::Manual
    );
    
    /// Obtiene la tasa de cambio actual
    /// @return Tasa actual o valor por defecto si no existe
    [[nodiscard]] double getExchangeRate() const;
    
    /// Obtiene información completa de la tasa
    /// @return Estructura con toda la información
    [[nodiscard]] ExchangeRateInfo getExchangeRateInfo() const;
    
    //--------------------------------------------------------------------------
    // Configuración de moneda
    //--------------------------------------------------------------------------
    
    /// Obtiene el símbolo de moneda local
    [[nodiscard]] QString getLocalCurrencySymbol() const;
    
    /// Establece el símbolo de moneda local
    bool setLocalCurrencySymbol(const QString& symbol);
    
    /// Obtiene el símbolo de USD
    [[nodiscard]] QString getUsdSymbol() const;
    
    //--------------------------------------------------------------------------
    // Historial de tasas
    //--------------------------------------------------------------------------
    
    /// Guarda una entrada en el historial
    bool saveRateHistory(const RateHistoryEntry& entry);
    
    /// Obtiene el historial de tasas
    /// @param limit Número máximo de registros (0 = todos)
    /// @return Lista de entradas del historial
    [[nodiscard]] QList<RateHistoryEntry> getRateHistory(int limit = 30) const;
    
    /// Obtiene la última tasa registrada en el historial
    [[nodiscard]] std::optional<RateHistoryEntry> getLastRateEntry() const;
    
    /// Limpia el historial antiguo (más de N días)
    bool cleanOldHistory(int daysToKeep = 90);
    
    //--------------------------------------------------------------------------
    // Utilidades
    //--------------------------------------------------------------------------
    
    /// Obtiene un valor de configuración
    [[nodiscard]] QString getSetting(const QString& key, const QString& defaultValue = {}) const;
    
    /// Guarda un valor de configuración
    bool setSetting(const QString& key, const QString& value);
    
private:
    /// Asegura que existe la tabla de historial
    void ensureHistoryTableExists();
};

} // namespace Vento
