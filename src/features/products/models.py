from dataclasses import dataclass
from datetime import datetime
from typing import Optional
from enum import Enum

class Currency(Enum):
    USD = "USD"
    CUP = "CUP"

@dataclass
class Product:
    """Product entity"""
    id: Optional[int] = None
    name: str = ""
    description: str = ""
    cost_price: float = 0.0
    cost_currency: Currency = Currency.USD
    margin_percent: float = 0.0
    sale_price: Optional[float] = None
    sale_currency: Optional[Currency] = None
    created_at: Optional[datetime] = None
    updated_at: Optional[datetime] = None
    
    def calculate_auto_sale_price(self, exchange_rate: float) -> float:
        """Calculate sale price based on cost, margin and exchange rate"""
        # Convert cost to target currency if needed
        if self.cost_currency == Currency.USD and self.sale_currency == Currency.CUP:
            cost_in_sale_currency = self.cost_price * exchange_rate
        elif self.cost_currency == Currency.CUP and self.sale_currency == Currency.USD:
            cost_in_sale_currency = self.cost_price / exchange_rate
        else:
            cost_in_sale_currency = self.cost_price
        
        # Apply margin
        margin_multiplier = 1 + (self.margin_percent / 100)
        return cost_in_sale_currency * margin_multiplier
    
    def get_final_sale_price(self, exchange_rate: float) -> float:
        """Get final sale price (manual or calculated)"""
        if self.sale_price is not None:
            return self.sale_price
        return self.calculate_auto_sale_price(exchange_rate)
