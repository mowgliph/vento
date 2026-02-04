import os
import shutil
import gzip
import json
from pathlib import Path
from datetime import datetime
from typing import Optional, List
from .security import get_security_manager
from .audit_logger import get_audit_logger
from .database import get_db

class BackupManager:
    """Secure encrypted backup management"""
    
    def __init__(self, backup_dir: Optional[str] = None):
        if backup_dir is None:
            backup_dir = Path(__file__).parent.parent.parent / "backups"
        
        self.backup_dir = Path(backup_dir)
        self.backup_dir.mkdir(parents=True, exist_ok=True)
        self._security_manager = get_security_manager()
        self._audit_logger = get_audit_logger()
        
        # Set secure permissions
        os.chmod(self.backup_dir, 0o700)
    
    def create_backup(self, backup_name: Optional[str] = None) -> str:
        """Create encrypted backup of database"""
        
        if backup_name is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            backup_name = f"vento_backup_{timestamp}"
        
        backup_path = self.backup_dir / f"{backup_name}.enc"
        
        try:
            # Get database path
            db_path = get_db().db_path
            
            # Create temporary backup file
            temp_backup = self.backup_dir / f"temp_{backup_name}.db"
            
            # Copy database file
            shutil.copy2(db_path, temp_backup)
            
            # Compress and encrypt
            encrypted_data = self._compress_and_encrypt(temp_backup)
            
            # Write encrypted backup
            with open(backup_path, 'wb') as f:
                f.write(encrypted_data)
            
            # Remove temporary file
            temp_backup.unlink()
            
            # Create backup metadata
            self._create_backup_metadata(backup_name, backup_path)
            
            # Log backup creation
            self._audit_logger.log_event(
                event_type=self._audit_logger.AuditEventType.DATA_CREATE,
                resource="backup",
                action="create",
                details={
                    "backup_name": backup_name,
                    "backup_path": str(backup_path),
                    "original_db_size": os.path.getsize(db_path),
                    "backup_size": os.path.getsize(backup_path)
                }
            )
            
            return str(backup_path)
            
        except Exception as e:
            # Clean up on failure
            if temp_backup.exists():
                temp_backup.unlink()
            if backup_path.exists():
                backup_path.unlink()
            
            self._audit_logger.log_security_event(
                event_type="backup_failure",
                description=f"Backup creation failed: {str(e)}",
                severity="high"
            )
            raise
    
    def restore_backup(self, backup_path: str) -> bool:
        """Restore database from encrypted backup"""
        
        backup_file = Path(backup_path)
        
        if not backup_file.exists():
            raise FileNotFoundError(f"Backup file not found: {backup_path}")
        
        try:
            # Decrypt and decompress
            temp_restore = self.backup_dir / f"temp_restore_{datetime.now().strftime('%Y%m%d_%H%M%S')}.db"
            
            self._decrypt_and_decompress(backup_file, temp_restore)
            
            # Verify backup integrity
            if not self._verify_backup_integrity(temp_restore):
                raise ValueError("Backup integrity check failed")
            
            # Get current database path
            db_path = get_db().db_path
            
            # Create backup of current database before restore
            current_backup = self.backup_dir / f"pre_restore_{datetime.now().strftime('%Y%m%d_%H%M%S')}.db"
            shutil.copy2(db_path, current_backup)
            
            # Replace database
            shutil.copy2(temp_restore, db_path)
            
            # Clean up temporary files
            temp_restore.unlink()
            
            # Log restore operation
            self._audit_logger.log_event(
                event_type=self._audit_logger.AuditEventType.DATA_UPDATE,
                resource="database",
                action="restore",
                details={
                    "backup_path": backup_path,
                    "current_backup": str(current_backup)
                }
            )
            
            return True
            
        except Exception as e:
            # Clean up on failure
            if temp_restore.exists():
                temp_restore.unlink()
            
            self._audit_logger.log_security_event(
                event_type="restore_failure",
                description=f"Database restore failed: {str(e)}",
                severity="high"
            )
            raise
    
    def list_backups(self) -> List[dict]:
        """List available backups with metadata"""
        
        backups = []
        
        for backup_file in self.backup_dir.glob("*.enc"):
            try:
                metadata_file = backup_file.with_suffix('.meta')
                
                if metadata_file.exists():
                    with open(metadata_file, 'r') as f:
                        metadata = json.load(f)
                else:
                    metadata = {
                        "name": backup_file.stem,
                        "created_at": datetime.fromtimestamp(backup_file.stat().st_ctime).isoformat(),
                        "size": backup_file.stat().st_size
                    }
                
                backups.append({
                    "file": str(backup_file),
                    "metadata": metadata
                })
                
            except Exception:
                continue
        
        return sorted(backups, key=lambda x: x["metadata"]["created_at"], reverse=True)
    
    def delete_backup(self, backup_path: str) -> bool:
        """Delete backup file and metadata"""
        
        backup_file = Path(backup_path)
        metadata_file = backup_file.with_suffix('.meta')
        
        try:
            if backup_file.exists():
                backup_file.unlink()
            
            if metadata_file.exists():
                metadata_file.unlink()
            
            # Log deletion
            self._audit_logger.log_event(
                event_type=self._audit_logger.AuditEventType.DATA_DELETE,
                resource="backup",
                action="delete",
                details={"backup_path": backup_path}
            )
            
            return True
            
        except Exception as e:
            self._audit_logger.log_security_event(
                event_type="backup_deletion_failure",
                description=f"Failed to delete backup: {str(e)}",
                severity="medium"
            )
            return False
    
    def _compress_and_encrypt(self, file_path: Path) -> bytes:
        """Compress and encrypt a file"""
        
        # Read and compress
        with open(file_path, 'rb') as f:
            data = f.read()
        
        compressed_data = gzip.compress(data)
        
        # Encrypt
        return self._security_manager.encrypt_data(compressed_data.decode('latin-1'))
    
    def _decrypt_and_decompress(self, encrypted_file: Path, output_path: Path):
        """Decrypt and decompress a file"""
        
        # Read encrypted data
        with open(encrypted_file, 'rb') as f:
            encrypted_data = f.read()
        
        # Decrypt
        decrypted_data = self._security_manager.decrypt_data(encrypted_data)
        
        # Decompress
        decompressed_data = gzip.decrypt(decrypted_data.encode('latin-1'))
        
        # Write output
        with open(output_path, 'wb') as f:
            f.write(decompressed_data)
    
    def _verify_backup_integrity(self, backup_path: Path) -> bool:
        """Verify backup database integrity"""
        
        try:
            # Try to open database and run integrity check
            import sqlite3
            conn = sqlite3.connect(str(backup_path))
            cursor = conn.cursor()
            
            # Check if tables exist
            cursor.execute("""
                SELECT name FROM sqlite_master 
                WHERE type='table' AND name IN ('products', 'sales', 'exchange_rates')
            """)
            
            tables = cursor.fetchall()
            if len(tables) < 3:
                return False
            
            # Run integrity check
            cursor.execute("PRAGMA integrity_check")
            result = cursor.fetchone()
            
            conn.close()
            
            return result[0] == "ok"
            
        except Exception:
            return False
    
    def _create_backup_metadata(self, backup_name: str, backup_path: Path):
        """Create backup metadata file"""
        
        metadata = {
            "name": backup_name,
            "created_at": datetime.now().isoformat(),
            "size": backup_path.stat().st_size,
            "version": "1.0",
            "database_path": str(get_db().db_path),
            "checksum": self._calculate_checksum(backup_path)
        }
        
        metadata_file = backup_path.with_suffix('.meta')
        with open(metadata_file, 'w') as f:
            json.dump(metadata, f, indent=2)
        
        # Secure permissions
        os.chmod(metadata_file, 0o600)
    
    def _calculate_checksum(self, file_path: Path) -> str:
        """Calculate file checksum for integrity verification"""
        
        import hashlib
        
        hash_sha256 = hashlib.sha256()
        
        with open(file_path, 'rb') as f:
            for chunk in iter(lambda: f.read(4096), b""):
                hash_sha256.update(chunk)
        
        return hash_sha256.hexdigest()
    
    def cleanup_old_backups(self, keep_count: int = 10) -> int:
        """Remove old backups, keeping only the most recent ones"""
        
        backups = self.list_backups()
        
        if len(backups) <= keep_count:
            return 0
        
        # Remove oldest backups
        backups_to_delete = backups[keep_count:]
        deleted_count = 0
        
        for backup in backups_to_delete:
            if self.delete_backup(backup["file"]):
                deleted_count += 1
        
        return deleted_count

# Global backup manager instance
_backup_manager = None

def get_backup_manager() -> BackupManager:
    """Get global backup manager instance"""
    global _backup_manager
    if _backup_manager is None:
        _backup_manager = BackupManager()
    return _backup_manager
