from typing import List
from datetime import datetime
from src.core.interfaces import Service
from src.core.validators import InputValidator, ValidationError
from src.features.sales.models import Sale
from src.features.sales.repository import SaleRepository

class SaleService(Service[Sale]):
    """Service for Sale business logic"""
    
    def __init__(self, repository: SaleRepository = None):
        self._repository = repository or SaleRepository()
    
    def create(self, sale: Sale) -> Sale:
        # Validate all input fields
        try:
            sale.product_id = InputValidator.validate_id(sale.product_id)
            sale.sale_price = float(InputValidator.validate_price(sale.sale_price, "precio de venta"))
            sale.sale_currency = InputValidator.validate_currency(sale.sale_currency)
            sale.quantity = InputValidator.validate_quantity(sale.quantity)
            
        except ValidationError as e:
            raise ValueError(str(e))
        
        return self._repository.create(sale)
    
    def get_by_id(self, id: int) -> Sale:
        return self._repository.get_by_id(id)
    
    def get_all(self) -> List[Sale]:
        return self._repository.get_all()
    
    def update(self, sale: Sale) -> Sale:
        # Validate all input fields
        try:
            sale.id = InputValidator.validate_id(sale.id)
            sale.product_id = InputValidator.validate_id(sale.product_id)
            sale.sale_price = float(InputValidator.validate_price(sale.sale_price, "precio de venta"))
            sale.sale_currency = InputValidator.validate_currency(sale.sale_currency)
            sale.quantity = InputValidator.validate_quantity(sale.quantity)
            
        except ValidationError as e:
            raise ValueError(str(e))
        
        return self._repository.update(sale)
    
    def delete(self, id: int) -> bool:
        return self._repository.delete(id)
    
    def get_by_date_range(self, start_date: datetime, end_date: datetime) -> List[Sale]:
        return self._repository.get_by_date_range(start_date, end_date)
    
    def get_by_product(self, product_id: int) -> List[Sale]:
        return self._repository.get_by_product(product_id)
    
    def get_daily_sales(self, date: datetime = None) -> List[Sale]:
        """Get sales for a specific date (defaults to today)"""
        if date is None:
            date = datetime.now()
        start = date.replace(hour=0, minute=0, second=0, microsecond=0)
        end = date.replace(hour=23, minute=59, second=59, microsecond=999999)
        return self._repository.get_by_date_range(start, end)
