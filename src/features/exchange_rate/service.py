from src.features.exchange_rate.models import ExchangeRate
from src.features.exchange_rate.repository import ExchangeRateRepository
from src.core.validators import InputValidator, ValidationError

class ExchangeRateService:
    """Service for Exchange Rate business logic"""
    
    def __init__(self, repository: ExchangeRateRepository = None):
        self._repository = repository or ExchangeRateRepository()
    
    def get_current_rate(self) -> ExchangeRate:
        return self._repository.get_current()
    
    def update_rate(self, usd_to_cup: float) -> ExchangeRate:
        try:
            validated_rate = InputValidator.validate_price(usd_to_cup, "tasa de cambio")
            if float(validated_rate) <= 0:
                raise ValidationError("La tasa de cambio debe ser mayor que cero")
        except ValidationError as e:
            raise ValueError(str(e))
        
        rate = ExchangeRate(usd_to_cup=float(validated_rate))
        return self._repository.update(rate)
    
    def convert(self, amount: float, from_currency: str, to_currency: str) -> float:
        """Convert amount between currencies"""
        if from_currency == to_currency:
            return amount
        
        rate = self.get_current_rate()
        
        if from_currency == "USD" and to_currency == "CUP":
            return amount * rate.usd_to_cup
        elif from_currency == "CUP" and to_currency == "USD":
            return amount / rate.usd_to_cup
        
        raise ValueError(f"Conversión no soportada: {from_currency} a {to_currency}")
