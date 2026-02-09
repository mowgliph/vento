from typing import List, Optional
from src.core.database import get_db
from src.core.interfaces import Repository
from src.core.audit_logger import get_audit_logger
from src.features.products.models import Product, Currency

class ProductRepository(Repository[Product]):
    """Repository for Product entity"""
    
    def __init__(self):
        self._audit_logger = get_audit_logger()
    
    def create(self, product: Product) -> Product:
        try:
            with get_db().get_connection() as conn:
                cursor = conn.cursor()
                cursor.execute('''
                    INSERT INTO products (name, description, cost_price, cost_currency, 
                                        margin_percent, sale_price, sale_currency)
                    VALUES (?, ?, ?, ?, ?, ?, ?)
                ''', (
                    product.name, product.description, product.cost_price,
                    product.cost_currency.value, product.margin_percent,
                    product.sale_price, 
                    product.sale_currency.value if product.sale_currency else None
                ))
                product.id = cursor.lastrowid
                
                # Log audit event
                self._audit_logger.log_data_operation(
                    operation="CREATE",
                    table="products",
                    record_id=product.id,
                    new_values={
                        "name": product.name,
                        "cost_price": str(product.cost_price)
                    }
                )
                
                return product
        except Exception as e:
            self._audit_logger.log_data_operation(
                operation="CREATE",
                table="products",
                success=False
            )
            raise
    
    def get_by_id(self, id: int) -> Optional[Product]:
        with get_db().get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute('SELECT * FROM products WHERE id = ?', (id,))
            row = cursor.fetchone()
            return self._row_to_product(row) if row else None
    
    def get_all(self) -> List[Product]:
        with get_db().get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute('SELECT * FROM products ORDER BY name')
            return [self._row_to_product(row) for row in cursor.fetchall()]
    
    def update(self, product: Product) -> Product:
        with get_db().get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute('''
                UPDATE products 
                SET name = ?, description = ?, cost_price = ?, cost_currency = ?,
                    margin_percent = ?, sale_price = ?, sale_currency = ?,
                    updated_at = CURRENT_TIMESTAMP
                WHERE id = ?
            ''', (
                product.name, product.description, product.cost_price,
                product.cost_currency.value, product.margin_percent,
                product.sale_price,
                product.sale_currency.value if product.sale_currency else None,
                product.id
            ))
            return product
    
    def delete(self, id: int) -> bool:
        with get_db().get_connection() as conn:
            cursor = conn.cursor()
            
            # Count associated sales for audit purposes
            cursor.execute('SELECT COUNT(*) FROM sales WHERE product_id = ?', (id,))
            sales_count = cursor.fetchone()[0]
            
            # Delete product (cascade will delete associated sales automatically)
            cursor.execute('DELETE FROM products WHERE id = ?', (id,))
            deleted = cursor.rowcount > 0
            
            # Log cascade deletion
            if deleted and sales_count > 0:
                self._audit_logger.log_data_operation(
                    operation="DELETE",
                    table="products",
                    record_id=id,
                    success=True,
                    metadata={"cascade_sales_deleted": sales_count}
                )
            
            return deleted
    
    def search(self, query: str) -> List[Product]:
        with get_db().get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute('''
                SELECT * FROM products 
                WHERE name LIKE ? OR description LIKE ?
                ORDER BY name
            ''', (f'%{query}%', f'%{query}%'))
            return [self._row_to_product(row) for row in cursor.fetchall()]
    
    def _row_to_product(self, row: sqlite3.Row) -> Product:
        return Product(
            id=row['id'],
            name=row['name'],
            description=row['description'],
            cost_price=row['cost_price'],
            cost_currency=Currency(row['cost_currency']),
            margin_percent=row['margin_percent'],
            sale_price=row['sale_price'],
            sale_currency=Currency(row['sale_currency']) if row['sale_currency'] else None,
            created_at=row['created_at'],
            updated_at=row['updated_at']
        )
