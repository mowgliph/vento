import json
import logging
from pathlib import Path
from datetime import datetime
from typing import Optional, Dict, Any
from enum import Enum

class AuditEventType(Enum):
    """Audit event types"""
    USER_LOGIN = "user_login"
    USER_LOGOUT = "user_logout"
    DATA_CREATE = "data_create"
    DATA_READ = "data_read"
    DATA_UPDATE = "data_update"
    DATA_DELETE = "data_delete"
    DATA_EXPORT = "data_export"
    SECURITY_VIOLATION = "security_violation"
    SYSTEM_ERROR = "system_error"
    CONFIGURATION_CHANGE = "config_change"

class AuditLogger:
    """Secure audit logging for compliance and security monitoring"""
    
    def __init__(self, app_name: str = "Vento"):
        self.app_name = app_name
        self._setup_logging()
    
    def _setup_logging(self):
        """Setup secure audit logging"""
        log_dir = Path(__file__).parent.parent.parent / "logs"
        log_dir.mkdir(exist_ok=True)
        
        # Create audit logger with specific formatting
        self.audit_logger = logging.getLogger(f"{self.app_name}.audit")
        self.audit_logger.setLevel(logging.INFO)
        
        # File handler for audit logs
        audit_handler = logging.FileHandler(log_dir / "audit.log")
        audit_handler.setLevel(logging.INFO)
        
        # JSON formatter for structured logging
        formatter = logging.Formatter('%(message)s')
        audit_handler.setFormatter(formatter)
        
        self.audit_logger.addHandler(audit_handler)
        
        # Prevent propagation to avoid duplicate logs
        self.audit_logger.propagate = False
    
    def log_event(self, 
                  event_type: AuditEventType,
                  user_id: Optional[str] = None,
                  resource: Optional[str] = None,
                  action: Optional[str] = None,
                  details: Optional[Dict[str, Any]] = None,
                  success: bool = True,
                  ip_address: Optional[str] = None):
        """Log an audit event"""
        
        audit_record = {
            "timestamp": datetime.now().isoformat(),
            "event_type": event_type.value,
            "user_id": user_id or "system",
            "resource": resource,
            "action": action,
            "success": success,
            "ip_address": ip_address,
            "details": details or {}
        }
        
        # Sanitize sensitive data
        audit_record = self._sanitize_audit_record(audit_record)
        
        # Log the audit record
        self.audit_logger.info(json.dumps(audit_record))
    
    def log_data_operation(self,
                          operation: str,
                          table: str,
                          record_id: Optional[Any] = None,
                          user_id: Optional[str] = None,
                          old_values: Optional[Dict] = None,
                          new_values: Optional[Dict] = None,
                          success: bool = True):
        """Log data operations (CRUD)"""
        
        event_type_map = {
            "CREATE": AuditEventType.DATA_CREATE,
            "READ": AuditEventType.DATA_READ,
            "UPDATE": AuditEventType.DATA_UPDATE,
            "DELETE": AuditEventType.DATA_DELETE,
            "EXPORT": AuditEventType.DATA_EXPORT
        }
        
        event_type = event_type_map.get(operation.upper(), AuditEventType.DATA_READ)
        
        details = {}
        if old_values:
            details["old_values"] = self._sanitize_data(old_values)
        if new_values:
            details["new_values"] = self._sanitize_data(new_values)
        
        self.log_event(
            event_type=event_type,
            user_id=user_id,
            resource=table,
            action=operation.lower(),
            details=details,
            success=success
        )
    
    def log_security_event(self,
                          event_type: str,
                          description: str,
                          user_id: Optional[str] = None,
                          ip_address: Optional[str] = None,
                          severity: str = "medium"):
        """Log security-related events"""
        
        details = {
            "description": description,
            "severity": severity
        }
        
        self.log_event(
            event_type=AuditEventType.SECURITY_VIOLATION,
            user_id=user_id,
            action=event_type,
            details=details,
            ip_address=ip_address,
            success=False
        )
    
    def log_configuration_change(self,
                               setting: str,
                               old_value: Any,
                               new_value: Any,
                               user_id: Optional[str] = None):
        """Log configuration changes"""
        
        details = {
            "setting": setting,
            "old_value": str(old_value),
            "new_value": str(new_value)
        }
        
        self.log_event(
            event_type=AuditEventType.CONFIGURATION_CHANGE,
            user_id=user_id,
            resource="configuration",
            action="change",
            details=details
        )
    
    def _sanitize_audit_record(self, record: Dict[str, Any]) -> Dict[str, Any]:
        """Sanitize audit record to remove sensitive information"""
        sensitive_fields = [
            'password', 'token', 'key', 'secret', 'credential',
            'auth', 'session', 'cookie', 'private', 'confidential'
        ]
        
        def sanitize_dict(d: Dict) -> Dict:
            sanitized = {}
            for key, value in d.items():
                key_lower = key.lower()
                if any(field in key_lower for field in sensitive_fields):
                    sanitized[key] = '[REDACTED]'
                elif isinstance(value, dict):
                    sanitized[key] = sanitize_dict(value)
                elif isinstance(value, str):
                    sanitized[key] = self._sanitize_string(value)
                else:
                    sanitized[key] = value
            return sanitized
        
        return sanitize_dict(record)
    
    def _sanitize_data(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Sanitize data values for logging"""
        sanitized = {}
        for key, value in data.items():
            if isinstance(value, str):
                sanitized[key] = self._sanitize_string(value)
            elif isinstance(value, (dict, list)):
                sanitized[key] = str(type(value).__name__) + " object"
            else:
                sanitized[key] = value
        return sanitized
    
    def _sanitize_string(self, text: str) -> str:
        """Sanitize string to remove sensitive patterns"""
        import re
        
        # Remove file paths
        text = re.sub(r'[A-Za-z]:\\[^\\s]*', '[PATH]', text)
        text = re.sub(r'/[^\\s]*', '[PATH]', text)
        
        # Remove potential SQL injection patterns
        text = re.sub(r'(\'|\"|;|--|/\*|\*/)', '', text)
        
        # Limit length
        if len(text) > 100:
            text = text[:100] + "..."
        
        return text
    
    def get_audit_trail(self, 
                       start_date: Optional[datetime] = None,
                       end_date: Optional[datetime] = None,
                       user_id: Optional[str] = None,
                       event_type: Optional[str] = None) -> list:
        """Retrieve audit trail for investigation"""
        
        audit_log_path = Path(__file__).parent.parent.parent / "logs" / "audit.log"
        
        if not audit_log_path.exists():
            return []
        
        audit_trail = []
        
        try:
            with open(audit_log_path, 'r') as f:
                for line in f:
                    try:
                        record = json.loads(line.strip())
                        
                        # Apply filters
                        if start_date:
                            record_time = datetime.fromisoformat(record['timestamp'])
                            if record_time < start_date:
                                continue
                        
                        if end_date:
                            record_time = datetime.fromisoformat(record['timestamp'])
                            if record_time > end_date:
                                continue
                        
                        if user_id and record.get('user_id') != user_id:
                            continue
                        
                        if event_type and record.get('event_type') != event_type:
                            continue
                        
                        audit_trail.append(record)
                        
                    except json.JSONDecodeError:
                        continue
                        
        except Exception:
            pass
        
        return audit_trail

# Global audit logger instance
_audit_logger = None

def get_audit_logger() -> AuditLogger:
    """Get global audit logger instance"""
    global _audit_logger
    if _audit_logger is None:
        _audit_logger = AuditLogger()
    return _audit_logger
