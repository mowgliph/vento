import logging
import traceback
from pathlib import Path
from typing import Optional, Any
from datetime import datetime
import json

class SecureErrorHandler:
    """Secure error handling that prevents information disclosure"""
    
    def __init__(self, app_name: str = "Vento"):
        self.app_name = app_name
        self._setup_logging()
    
    def _setup_logging(self):
        """Setup secure logging configuration"""
        log_dir = Path(__file__).parent.parent.parent / "logs"
        log_dir.mkdir(exist_ok=True)
        
        # Configure logging to prevent sensitive data exposure
        logging.basicConfig(
            level=logging.INFO,
            handlers=[
                logging.FileHandler(log_dir / "app.log"),
                logging.FileHandler(log_dir / "security.log")
            ],
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        
        self.logger = logging.getLogger(self.app_name)
    
    def handle_exception(self, exception: Exception, context: Optional[str] = None) -> str:
        """Handle exception securely without exposing sensitive information"""
        error_id = self._generate_error_id()
        
        # Log detailed error for debugging
        self._log_detailed_error(exception, context, error_id)
        
        # Return user-friendly message
        return self._get_user_friendly_message(exception, error_id)
    
    def _generate_error_id(self) -> str:
        """Generate unique error ID for tracking"""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        return f"ERR_{timestamp}_{hash(str(id(object()))) % 10000:04d}"
    
    def _log_detailed_error(self, exception: Exception, context: Optional[str], error_id: str):
        """Log detailed error information for debugging"""
        error_info = {
            "error_id": error_id,
            "timestamp": datetime.now().isoformat(),
            "exception_type": type(exception).__name__,
            "exception_message": str(exception),
            "context": context,
            "traceback": traceback.format_exc()
        }
        
        # Log to security log for monitoring
        security_logger = logging.getLogger(f"{self.app_name}.security")
        security_logger.error(f"Security Event: {json.dumps(error_info)}")
        
        # Log to application log
        self.logger.error(f"Error {error_id}: {type(exception).__name__} - {str(exception)}")
    
    def _get_user_friendly_message(self, exception: Exception, error_id: str) -> str:
        """Get user-friendly error message without exposing technical details"""
        # Map common exceptions to user-friendly messages
        error_mapping = {
            "ValidationError": "Los datos ingresados no son válidos. Por favor, verifique la información.",
            "ValueError": "El valor ingresado no es correcto. Por favor, intente nuevamente.",
            "PermissionError": "No tiene permisos para realizar esta acción.",
            "FileNotFoundError": "El archivo solicitado no fue encontrado.",
            "ConnectionError": "Error de conexión. Por favor, verifique su conexión a internet.",
            "TimeoutError": "La operación tomó demasiado tiempo. Por favor, intente nuevamente.",
            "MemoryError": "El sistema se quedó sin memoria. Por favor, reinicie la aplicación.",
            "OverflowError": "El número es demasiado grande para procesar.",
            "ZeroDivisionError": "No se puede dividir por cero.",
        }
        
        exception_name = type(exception).__name__
        user_message = error_mapping.get(exception_name, "Ocurrió un error inesperado.")
        
        return f"{user_message}\nCódigo de error: {error_id}"
    
    def log_security_event(self, event_type: str, details: dict, user_id: Optional[str] = None):
        """Log security-related events"""
        security_event = {
            "timestamp": datetime.now().isoformat(),
            "event_type": event_type,
            "user_id": user_id,
            "details": details
        }
        
        security_logger = logging.getLogger(f"{self.app_name}.security")
        security_logger.info(f"Security Event: {json.dumps(security_event)}")
    
    def log_data_access(self, operation: str, table: str, record_id: Optional[Any] = None, 
                       user_id: Optional[str] = None, success: bool = True):
        """Log data access for audit trail"""
        access_log = {
            "timestamp": datetime.now().isoformat(),
            "operation": operation,
            "table": table,
            "record_id": record_id,
            "user_id": user_id,
            "success": success
        }
        
        audit_logger = logging.getLogger(f"{self.app_name}.audit")
        audit_logger.info(f"Data Access: {json.dumps(access_log)}")
    
    def sanitize_error_message(self, message: str) -> str:
        """Sanitize error message to prevent information disclosure"""
        # Remove potential sensitive information
        sensitive_patterns = [
            r'password',
            r'token',
            r'key',
            r'secret',
            r'credential',
            r'auth',
            r'/path/',
            r'C:\\',
            r'/home/',
            r'\\Users\\',
            r'\.db',
            r'\.sqlite',
        ]
        
        import re
        sanitized = message
        
        for pattern in sensitive_patterns:
            sanitized = re.sub(pattern, '[REDACTED]', sanitized, flags=re.IGNORECASE)
        
        # Limit length to prevent information disclosure
        if len(sanitized) > 200:
            sanitized = sanitized[:200] + "..."
        
        return sanitized

# Global error handler instance
_error_handler = None

def get_error_handler() -> SecureErrorHandler:
    """Get global error handler instance"""
    global _error_handler
    if _error_handler is None:
        _error_handler = SecureErrorHandler()
    return _error_handler

def handle_exception(exception: Exception, context: Optional[str] = None) -> str:
    """Convenience function to handle exceptions"""
    return get_error_handler().handle_exception(exception, context)
