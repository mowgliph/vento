from typing import List, Optional
from datetime import datetime
import sqlite3
from src.core.database import get_db
from src.core.interfaces import Repository
from src.features.sales.models import Sale

class SaleRepository(Repository[Sale]):
    """Repository for Sale entity"""
    
    def create(self, sale: Sale) -> Sale:
        with get_db().get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute('''
                INSERT INTO sales (product_id, sale_price, sale_currency, quantity, sale_date)
                VALUES (?, ?, ?, ?, ?)
            ''', (sale.product_id, sale.sale_price, sale.sale_currency, 
                  sale.quantity, sale.sale_date or datetime.now()))
            sale.id = cursor.lastrowid
            return sale
    
    def get_by_id(self, id: int) -> Optional[Sale]:
        with get_db().get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute('''
                SELECT s.*, p.name as product_name 
                FROM sales s
                JOIN products p ON s.product_id = p.id
                WHERE s.id = ?
            ''', (id,))
            row = cursor.fetchone()
            return self._row_to_sale(row) if row else None
    
    def get_all(self) -> List[Sale]:
        with get_db().get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute('''
                SELECT s.*, p.name as product_name 
                FROM sales s
                JOIN products p ON s.product_id = p.id
                ORDER BY s.sale_date DESC
            ''')
            return [self._row_to_sale(row) for row in cursor.fetchall()]
    
    def get_by_date_range(self, start_date: datetime, end_date: datetime) -> List[Sale]:
        with get_db().get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute('''
                SELECT s.*, p.name as product_name 
                FROM sales s
                JOIN products p ON s.product_id = p.id
                WHERE s.sale_date BETWEEN ? AND ?
                ORDER BY s.sale_date DESC
            ''', (start_date, end_date))
            return [self._row_to_sale(row) for row in cursor.fetchall()]
    
    def get_by_product(self, product_id: int) -> List[Sale]:
        with get_db().get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute('''
                SELECT s.*, p.name as product_name 
                FROM sales s
                JOIN products p ON s.product_id = p.id
                WHERE s.product_id = ?
                ORDER BY s.sale_date DESC
            ''', (product_id,))
            return [self._row_to_sale(row) for row in cursor.fetchall()]
    
    def update(self, sale: Sale) -> Sale:
        with get_db().get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute('''
                UPDATE sales 
                SET product_id = ?, sale_price = ?, sale_currency = ?, 
                    quantity = ?, sale_date = ?
                WHERE id = ?
            ''', (sale.product_id, sale.sale_price, sale.sale_currency,
                  sale.quantity, sale.sale_date, sale.id))
            return sale
    
    def delete(self, id: int) -> bool:
        with get_db().get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute('DELETE FROM sales WHERE id = ?', (id,))
            return cursor.rowcount > 0
    
    def _row_to_sale(self, row: sqlite3.Row) -> Sale:
        sale_date = row['sale_date']
        # Convert string to datetime if necessary
        if isinstance(sale_date, str):
            sale_date = datetime.fromisoformat(sale_date)
        return Sale(
            id=row['id'],
            product_id=row['product_id'],
            product_name=row['product_name'] if 'product_name' in row.keys() else '',
            sale_price=row['sale_price'],
            sale_currency=row['sale_currency'],
            quantity=row['quantity'],
            sale_date=sale_date
        )
