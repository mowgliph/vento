from dataclasses import dataclass
from enum import Enum

class Currency(Enum):
    USD = "USD"
    CUP = "CUP"

@dataclass
class AppConfig:
    """Application configuration"""
    APP_NAME: str = "Vento - Gestión de Tienda"
    APP_VERSION: str = "1.0.0"
    DEFAULT_MARGIN_PERCENT: float = 20.0
    WINDOW_WIDTH: int = 1200
    WINDOW_HEIGHT: int = 800
