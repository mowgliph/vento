from typing import Optional
from src.core.database import get_db
from src.features.exchange_rate.models import ExchangeRate

class ExchangeRateRepository:
    """Repository for Exchange Rate entity"""
    
    def get_current(self) -> ExchangeRate:
        with get_db().get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute('SELECT * FROM exchange_rates WHERE id = 1')
            row = cursor.fetchone()
            if row:
                return ExchangeRate(
                    id=row['id'],
                    usd_to_cup=row['usd_to_cup'],
                    updated_at=row['updated_at']
                )
            # Default rate
            return ExchangeRate(id=1, usd_to_cup=120.0)
    
    def update(self, rate: ExchangeRate) -> ExchangeRate:
        with get_db().get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute('''
                INSERT INTO exchange_rates (id, usd_to_cup, updated_at)
                VALUES (?, ?, CURRENT_TIMESTAMP)
                ON CONFLICT(id) DO UPDATE SET
                    usd_to_cup = excluded.usd_to_cup,
                    updated_at = excluded.updated_at
            ''', (rate.id, rate.usd_to_cup))
            return rate
