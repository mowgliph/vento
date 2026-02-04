from dataclasses import dataclass
from datetime import datetime
from typing import Optional

@dataclass
class ExchangeRate:
    """Exchange rate entity"""
    id: int = 1
    usd_to_cup: float = 120.0
    updated_at: Optional[datetime] = None
