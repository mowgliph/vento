import unittest
import tempfile
import os
from pathlib import Path

from src.core.database import DatabaseManager
from src.features.products.repository import ProductRepository
from src.features.products.models import Product, Currency
from src.features.sales.repository import SaleRepository
from src.features.sales.models import Sale


class TestCascadeDelete(unittest.TestCase):
    """Test cascade delete functionality for products with associated sales"""
    
    @classmethod
    def setUpClass(cls):
        """Set up test database"""
        cls.test_db = Path(tempfile.mktemp(suffix='.db'))
        cls.db_manager = DatabaseManager(str(cls.test_db))
        
    @classmethod
    def tearDownClass(cls):
        """Clean up test database"""
        if cls.test_db.exists():
            os.remove(cls.test_db)
    
    def setUp(self):
        """Set up fresh database for each test"""
        self.product_repo = ProductRepository()
        self.sale_repo = SaleRepository()
        
    def tearDown(self):
        """Clean up after each test"""
        with self.db_manager.get_connection() as conn:
            cursor = conn.cursor()
            cursor.execute('DELETE FROM sales')
            cursor.execute('DELETE FROM products')
    
    def test_delete_product_without_sales(self):
        """Test deleting a product with no associated sales"""
        # Create product
        product = Product(
            name="Test Product",
            description="Test description",
            cost_price=10.0,
            cost_currency=Currency.USD,
            margin_percent=20.0
        )
        created_product = self.product_repo.create(product)
        
        # Verify product exists
        self.assertIsNotNone(created_product.id)
        
        # Delete product
        result = self.product_repo.delete(created_product.id)
        
        # Verify deletion
        self.assertTrue(result)
        deleted_product = self.product_repo.get_by_id(created_product.id)
        self.assertIsNone(deleted_product)
    
    def test_delete_product_with_sales_cascade(self):
        """Test deleting a product with associated sales triggers cascade delete"""
        # Create product
        product = Product(
            name="Test Product",
            description="Test description",
            cost_price=10.0,
            cost_currency=Currency.USD,
            margin_percent=20.0,
            sale_price=12.0,
            sale_currency=Currency.USD
        )
        created_product = self.product_repo.create(product)
        
        # Create sales for the product
        sale1 = Sale(
            product_id=created_product.id,
            sale_price=12.0,
            sale_currency=Currency.USD,
            quantity=2
        )
        sale2 = Sale(
            product_id=created_product.id,
            sale_price=12.0,
            sale_currency=Currency.USD,
            quantity=1
        )
        
        created_sale1 = self.sale_repo.create(sale1)
        created_sale2 = self.sale_repo.create(sale2)
        
        # Verify sales exist
        self.assertIsNotNone(created_sale1.id)
        self.assertIsNotNone(created_sale2.id)
        
        # Count sales before deletion
        all_sales = self.sale_repo.get_all()
        self.assertEqual(len(all_sales), 2)
        
        # Delete product (should cascade delete sales)
        result = self.product_repo.delete(created_product.id)
        
        # Verify product deletion
        self.assertTrue(result)
        deleted_product = self.product_repo.get_by_id(created_product.id)
        self.assertIsNone(deleted_product)
        
        # Verify sales were cascade deleted
        remaining_sales = self.sale_repo.get_all()
        self.assertEqual(len(remaining_sales), 0)
    
    def test_delete_multiple_products_with_sales(self):
        """Test cascade delete with multiple products and sales"""
        # Create two products
        product1 = Product(
            name="Product 1",
            description="Description 1",
            cost_price=10.0,
            cost_currency=Currency.USD,
            margin_percent=20.0,
            sale_price=12.0,
            sale_currency=Currency.USD
        )
        product2 = Product(
            name="Product 2",
            description="Description 2",
            cost_price=15.0,
            cost_currency=Currency.USD,
            margin_percent=20.0,
            sale_price=18.0,
            sale_currency=Currency.USD
        )
        
        created_product1 = self.product_repo.create(product1)
        created_product2 = self.product_repo.create(product2)
        
        # Create sales for both products
        sale1 = Sale(
            product_id=created_product1.id,
            sale_price=12.0,
            sale_currency=Currency.USD,
            quantity=3
        )
        sale2 = Sale(
            product_id=created_product2.id,
            sale_price=18.0,
            sale_currency=Currency.USD,
            quantity=2
        )
        
        self.sale_repo.create(sale1)
        self.sale_repo.create(sale2)
        
        # Verify both products and sales exist
        self.assertEqual(len(self.product_repo.get_all()), 2)
        self.assertEqual(len(self.sale_repo.get_all()), 2)
        
        # Delete first product (should only delete its sales)
        self.product_repo.delete(created_product1.id)
        
        # Verify first product and its sales are deleted
        self.assertIsNone(self.product_repo.get_by_id(created_product1.id))
        self.assertIsNotNone(self.product_repo.get_by_id(created_product2.id))
        
        # Verify only second product's sales remain
        remaining_sales = self.sale_repo.get_all()
        self.assertEqual(len(remaining_sales), 1)
        self.assertEqual(remaining_sales[0].product_id, created_product2.id)
    
    def test_cascade_delete_with_many_sales(self):
        """Test cascade delete with product having many sales"""
        # Create product
        product = Product(
            name="Product",
            description="Description",
            cost_price=10.0,
            cost_currency=Currency.USD,
            margin_percent=20.0,
            sale_price=12.0,
            sale_currency=Currency.USD
        )
        created_product = self.product_repo.create(product)
        
        # Create 100 sales
        num_sales = 100
        for i in range(num_sales):
            sale = Sale(
                product_id=created_product.id,
                sale_price=12.0,
                sale_currency=Currency.USD,
                quantity=1
            )
            self.sale_repo.create(sale)
        
        # Verify sales exist
        self.assertEqual(len(self.sale_repo.get_all()), num_sales)
        
        # Delete product
        self.product_repo.delete(created_product.id)
        
        # Verify all sales were deleted
        self.assertEqual(len(self.sale_repo.get_all()), 0)


if __name__ == '__main__':
    unittest.main()
