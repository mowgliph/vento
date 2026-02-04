import re
from typing import Optional, List
from decimal import Decimal, InvalidOperation

class ValidationError(Exception):
    """Custom validation error"""
    pass

class InputValidator:
    """Centralized input validation"""
    
    @staticmethod
    def validate_product_name(name: str) -> str:
        """Validate product name"""
        if not name or not name.strip():
            raise ValidationError("El nombre del producto es requerido")
        
        name = name.strip()
        
        if len(name) < 2:
            raise ValidationError("El nombre debe tener al menos 2 caracteres")
        
        if len(name) > 200:
            raise ValidationError("El nombre no puede exceder 200 caracteres")
        
        # Allow letters, numbers, spaces, and basic punctuation
        if not re.match(r'^[a-zA-Z0-9\s\-_.,()[\]{}]+$', name):
            raise ValidationError("El nombre contiene caracteres no permitidos")
        
        return name
    
    @staticmethod
    def validate_description(description: Optional[str]) -> Optional[str]:
        """Validate product description"""
        if description is None:
            return None
        
        description = description.strip()
        
        if len(description) > 1000:
            raise ValidationError("La descripción no puede exceder 1000 caracteres")
        
        # Allow common characters but prevent script injection
        dangerous_patterns = [
            r'<script[^>]*>.*?</script>',
            r'javascript:',
            r'on\w+\s*=',
        ]
        
        for pattern in dangerous_patterns:
            if re.search(pattern, description, re.IGNORECASE):
                raise ValidationError("La descripción contiene contenido no permitido")
        
        return description if description else None
    
    @staticmethod
    def validate_price(price: float, field_name: str = "precio") -> Decimal:
        """Validate price values"""
        try:
            decimal_price = Decimal(str(price))
        except (InvalidOperation, ValueError):
            raise ValidationError(f"El {field_name} debe ser un número válido")
        
        if decimal_price < 0:
            raise ValidationError(f"El {field_name} no puede ser negativo")
        
        if decimal_price > 999999.99:
            raise ValidationError(f"El {field_name} no puede exceder 999,999.99")
        
        # Check for reasonable precision (max 2 decimal places)
        if decimal_price.as_tuple().exponent < -2:
            raise ValidationError(f"El {field_name} no puede tener más de 2 decimales")
        
        return decimal_price
    
    @staticmethod
    def validate_margin_percent(margin: float) -> Decimal:
        """Validate margin percentage"""
        try:
            decimal_margin = Decimal(str(margin))
        except (InvalidOperation, ValueError):
            raise ValidationError("El margen debe ser un número válido")
        
        if decimal_margin < 0:
            raise ValidationError("El margen no puede ser negativo")
        
        if decimal_margin > 1000:
            raise ValidationError("El margen no puede exceder 1000%")
        
        return decimal_margin
    
    @staticmethod
    def validate_quantity(quantity: int) -> int:
        """Validate quantity values"""
        if not isinstance(quantity, int):
            try:
                quantity = int(quantity)
            except (ValueError, TypeError):
                raise ValidationError("La cantidad debe ser un número entero")
        
        if quantity <= 0:
            raise ValidationError("La cantidad debe ser mayor que cero")
        
        if quantity > 99999:
            raise ValidationError("La cantidad no puede exceder 99,999")
        
        return quantity
    
    @staticmethod
    def validate_currency(currency: str) -> str:
        """Validate currency code"""
        if currency not in ['USD', 'CUP']:
            raise ValidationError("La moneda debe ser USD o CUP")
        return currency
    
    @staticmethod
    def validate_id(id_value: int) -> int:
        """Validate ID values"""
        if not isinstance(id_value, int):
            try:
                id_value = int(id_value)
            except (ValueError, TypeError):
                raise ValidationError("El ID debe ser un número entero")
        
        if id_value <= 0:
            raise ValidationError("El ID debe ser un número positivo")
        
        return id_value
    
    @staticmethod
    def validate_search_query(query: str) -> str:
        """Validate search query"""
        if not query:
            return ""
        
        query = query.strip()
        
        if len(query) > 100:
            raise ValidationError("La búsqueda no puede exceder 100 caracteres")
        
        # Remove potential SQL injection patterns
        dangerous_patterns = [
            r"'",
            r'"',
            r';',
            r'--',
            r'/\*',
            r'\*/',
            r'xp_',
            r'sp_',
        ]
        
        for pattern in dangerous_patterns:
            if re.search(pattern, query, re.IGNORECASE):
                raise ValidationError("La búsqueda contiene caracteres no permitidos")
        
        return query
    
    @staticmethod
    def sanitize_string(value: str, max_length: int = 255) -> str:
        """Basic string sanitization"""
        if not isinstance(value, str):
            value = str(value)
        
        value = value.strip()
        
        if len(value) > max_length:
            value = value[:max_length]
        
        # Remove null bytes and other dangerous characters
        value = value.replace('\x00', '')
        value = re.sub(r'[\x00-\x08\x0B\x0C\x0E-\x1F\x7F]', '', value)
        
        return value
