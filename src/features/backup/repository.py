from typing import List, Optional
from src.core.backup_manager import get_backup_manager, BackupInfo
from src.features.backup.models import Backup

class BackupRepository:
    """Repository for Backup entity"""
    
    def __init__(self):
        self._manager = get_backup_manager()
    
    def create_backup(self, backup_name: Optional[str] = None) -> Backup:
        """Create a new backup"""
        backup_path = self._manager.create_backup(backup_name)
        
        # Get backup info
        backups = self._manager.list_backups()
        backup_info = next((b for b in backups if b.file_path == backup_path), None)
        
        if backup_info:
            return Backup(
                name=backup_info.name,
                file_path=backup_info.file_path,
                created_at=backup_info.created_at,
                size=backup_info.size,
                size_formatted=backup_info.size_formatted
            )
        
        return Backup(name=backup_name, file_path=backup_path)
    
    def restore_backup(self, backup_path: str) -> bool:
        """Restore from backup"""
        return self._manager.restore_backup(backup_path)
    
    def get_all(self) -> List[Backup]:
        """Get all backups"""
        backups = self._manager.list_backups()
        return [
            Backup(
                name=b.name,
                file_path=b.file_path,
                created_at=b.created_at,
                size=b.size,
                size_formatted=b.size_formatted
            )
            for b in backups
        ]
    
    def delete_backup(self, backup_path: str) -> bool:
        """Delete a backup"""
        return self._manager.delete_backup(backup_path)
    
    def export_to_json(self, export_path: str) -> str:
        """Export data to JSON"""
        return self._manager.export_to_json(export_path)
    
    def import_from_json(self, import_path: str) -> bool:
        """Import data from JSON"""
        return self._manager.import_from_json(import_path)
    
    def cleanup_old_backups(self, keep_count: int = 10) -> int:
        """Clean up old backups"""
        return self._manager.cleanup_old_backups(keep_count)
