from dataclasses import dataclass
from datetime import datetime
from typing import Optional

@dataclass
class Sale:
    """Sale entity"""
    id: Optional[int] = None
    product_id: int = 0
    product_name: str = ""  # For display purposes
    sale_price: float = 0.0
    sale_currency: str = "USD"
    quantity: int = 1
    sale_date: Optional[datetime] = None
