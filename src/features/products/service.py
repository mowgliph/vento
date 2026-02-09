from typing import List, Optional
from src.core.interfaces import Service
from src.core.validators import InputValidator, ValidationError
from src.features.products.models import Product
from src.features.products.repository import ProductRepository

class ProductService(Service[Product]):
    """Service for Product business logic"""
    
    def __init__(self, repository: ProductRepository = None):
        self._repository = repository or ProductRepository()
    
    def create(self, product: Product) -> Product:
        # Validate all input fields
        try:
            product.name = InputValidator.validate_product_name(product.name)
            product.description = InputValidator.validate_description(product.description)
            product.cost_price = float(InputValidator.validate_price(product.cost_price, "precio de costo"))
            product.cost_currency = InputValidator.validate_currency_enum(product.cost_currency.value)
            product.margin_percent = float(InputValidator.validate_margin_percent(product.margin_percent))
            
            if product.sale_price is not None:
                product.sale_price = float(InputValidator.validate_price(product.sale_price, "precio de venta"))
            
            if product.sale_currency is not None:
                product.sale_currency = InputValidator.validate_currency_enum(product.sale_currency.value)
                
        except ValidationError as e:
            raise ValueError(str(e))
        
        return self._repository.create(product)
    
    def get_by_id(self, id: int) -> Optional[Product]:
        return self._repository.get_by_id(id)
    
    def get_all(self) -> List[Product]:
        return self._repository.get_all()
    
    def update(self, product: Product) -> Product:
        # Validate ID
        try:
            product.id = InputValidator.validate_id(product.id)
            # Validate all other fields
            product.name = InputValidator.validate_product_name(product.name)
            product.description = InputValidator.validate_description(product.description)
            product.cost_price = float(InputValidator.validate_price(product.cost_price, "precio de costo"))
            product.cost_currency = InputValidator.validate_currency_enum(product.cost_currency.value)
            product.margin_percent = float(InputValidator.validate_margin_percent(product.margin_percent))
            
            if product.sale_price is not None:
                product.sale_price = float(InputValidator.validate_price(product.sale_price, "precio de venta"))
            
            if product.sale_currency is not None:
                product.sale_currency = InputValidator.validate_currency_enum(product.sale_currency.value)
                
        except ValidationError as e:
            raise ValueError(str(e))
        
        return self._repository.update(product)
    
    def delete(self, id: int) -> bool:
        return self._repository.delete(id)
    
    def search(self, query: str) -> List[Product]:
        try:
            validated_query = InputValidator.validate_search_query(query)
            if not validated_query:
                return self.get_all()
            return self._repository.search(validated_query)
        except ValidationError as e:
            raise ValueError(str(e))
