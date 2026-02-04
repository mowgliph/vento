import sqlite3
from contextlib import contextmanager
from pathlib import Path
from typing import Optional

class DatabaseManager:
    """Singleton database manager for SQLite connections"""
    _instance: Optional['DatabaseManager'] = None
    
    def __new__(cls, db_path: Optional[str] = None):
        if cls._instance is None:
            cls._instance = super().__new__(cls)
            cls._instance._initialized = False
        return cls._instance
    
    def __init__(self, db_path: Optional[str] = None):
        if self._initialized:
            return
        
        if db_path is None:
            db_path = Path(__file__).parent.parent.parent / "data" / "vento.db"
        
        self.db_path = Path(db_path)
        self.db_path.parent.mkdir(parents=True, exist_ok=True)
        self._initialized = True
        self._init_database()
    
    @contextmanager
    def get_connection(self):
        """Context manager for database connections"""
        conn = sqlite3.connect(self.db_path)
        conn.row_factory = sqlite3.Row
        try:
            yield conn
            conn.commit()
        except Exception:
            conn.rollback()
            raise
        finally:
            conn.close()
    
    def _init_database(self):
        """Initialize database tables"""
        with self.get_connection() as conn:
            cursor = conn.cursor()
            
            # Exchange rate table
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS exchange_rates (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    usd_to_cup REAL NOT NULL,
                    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                )
            ''')
            
            # Products table
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS products (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    name TEXT NOT NULL,
                    description TEXT,
                    cost_price REAL NOT NULL,
                    cost_currency TEXT NOT NULL CHECK(cost_currency IN ('USD', 'CUP')),
                    margin_percent REAL DEFAULT 0,
                    sale_price REAL,
                    sale_currency TEXT CHECK(sale_currency IN ('USD', 'CUP')),
                    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                )
            ''')
            
            # Sales table
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS sales (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    product_id INTEGER NOT NULL,
                    sale_price REAL NOT NULL,
                    sale_currency TEXT NOT NULL CHECK(sale_currency IN ('USD', 'CUP')),
                    quantity INTEGER DEFAULT 1,
                    sale_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                    FOREIGN KEY (product_id) REFERENCES products(id)
                )
            ''')
            
            # Insert default exchange rate if not exists
            cursor.execute('''
                INSERT OR IGNORE INTO exchange_rates (id, usd_to_cup, updated_at)
                VALUES (1, 120.0, CURRENT_TIMESTAMP)
            ''')
            
            conn.commit()

def get_db() -> DatabaseManager:
    """Factory function to get database manager instance"""
    return DatabaseManager()
