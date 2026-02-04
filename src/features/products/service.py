from typing import List, Optional
from src.core.interfaces import Service
from src.features.products.models import Product
from src.features.products.repository import ProductRepository

class ProductService(Service[Product]):
    """Service for Product business logic"""
    
    def __init__(self, repository: ProductRepository = None):
        self._repository = repository or ProductRepository()
    
    def create(self, product: Product) -> Product:
        if not product.name:
            raise ValueError("El nombre del producto es requerido")
        if product.cost_price < 0:
            raise ValueError("El precio de costo no puede ser negativo")
        return self._repository.create(product)
    
    def get_by_id(self, id: int) -> Optional[Product]:
        return self._repository.get_by_id(id)
    
    def get_all(self) -> List[Product]:
        return self._repository.get_all()
    
    def update(self, product: Product) -> Product:
        if not product.id:
            raise ValueError("El ID del producto es requerido para actualizar")
        return self._repository.update(product)
    
    def delete(self, id: int) -> bool:
        return self._repository.delete(id)
    
    def search(self, query: str) -> List[Product]:
        if not query:
            return self.get_all()
        return self._repository.search(query)
