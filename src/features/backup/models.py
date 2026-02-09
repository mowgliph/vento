from dataclasses import dataclass
from datetime import datetime
from typing import Optional

@dataclass
class Backup:
    """Backup entity"""
    id: Optional[int] = None
    name: str = ""
    file_path: str = ""
    created_at: Optional[datetime] = None
    size: int = 0
    size_formatted: str = "0 B"
