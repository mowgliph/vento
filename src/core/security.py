import os
import base64
import hashlib
from cryptography.fernet import Fernet
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from typing import Optional, Tuple
import secrets

class SecurityManager:
    """Manages encryption and security operations"""
    
    def __init__(self, password: Optional[str] = None):
        self._password = password or self._get_or_create_password()
        self._salt = self._get_or_create_salt()
        self._fernet = self._create_fernet()
    
    def _get_or_create_password(self) -> str:
        """Get existing password or create new one"""
        password_file = os.path.expanduser("~/.vento_security_key")
        if os.path.exists(password_file):
            with open(password_file, 'r') as f:
                return f.read().strip()
        else:
            password = secrets.token_urlsafe(32)
            os.makedirs(os.path.dirname(password_file), exist_ok=True)
            with open(password_file, 'w') as f:
                f.write(password)
            os.chmod(password_file, 0o600)  # Read/write for owner only
            return password
    
    def _get_or_create_salt(self) -> bytes:
        """Get existing salt or create new one"""
        salt_file = os.path.expanduser("~/.vento_salt")
        if os.path.exists(salt_file):
            with open(salt_file, 'rb') as f:
                return f.read()
        else:
            salt = os.urandom(16)
            os.makedirs(os.path.dirname(salt_file), exist_ok=True)
            with open(salt_file, 'wb') as f:
                f.write(salt)
            os.chmod(salt_file, 0o600)
            return salt
    
    def _create_fernet(self) -> Fernet:
        """Create Fernet encryption key"""
        kdf = PBKDF2HMAC(
            algorithm=hashes.SHA256(),
            length=32,
            salt=self._salt,
            iterations=100000,
        )
        key = base64.urlsafe_b64encode(kdf.derive(self._password.encode()))
        return Fernet(key)
    
    def encrypt_data(self, data: str) -> bytes:
        """Encrypt string data"""
        return self._fernet.encrypt(data.encode())
    
    def decrypt_data(self, encrypted_data: bytes) -> str:
        """Decrypt encrypted data"""
        return self._fernet.decrypt(encrypted_data).decode()
    
    def hash_password(self, password: str) -> Tuple[str, str]:
        """Hash password with salt"""
        salt = secrets.token_hex(16)
        pwd_hash = hashlib.pbkdf2_hmac('sha256', 
                                      password.encode('utf-8'), 
                                      salt.encode('utf-8'), 
                                      100000)
        return salt, pwd_hash.hex()
    
    def verify_password(self, password: str, salt: str, stored_hash: str) -> bool:
        """Verify password against stored hash"""
        pwd_hash = hashlib.pbkdf2_hmac('sha256', 
                                      password.encode('utf-8'), 
                                      salt.encode('utf-8'), 
                                      100000)
        return pwd_hash.hex() == stored_hash

# Global security manager instance
_security_manager = None

def get_security_manager() -> SecurityManager:
    """Get global security manager instance"""
    global _security_manager
    if _security_manager is None:
        _security_manager = SecurityManager()
    return _security_manager
