from typing import List, Optional
from src.core.interfaces import Service
from src.features.backup.models import Backup
from src.features.backup.repository import BackupRepository

class BackupService(Service[Backup]):
    """Service for Backup business logic"""
    
    def __init__(self, repository: BackupRepository = None):
        self._repository = repository or BackupRepository()
    
    def create(self, entity: Backup) -> Backup:
        """Create a new backup (implementation of abstract method)"""
        return self.create_backup(entity.name if entity else None)
    
    def create_backup(self, backup_name: Optional[str] = None) -> Backup:
        """Create a new backup with validation"""
        if backup_name and not backup_name.strip():
            raise ValueError("El nombre del backup no puede estar vacío")
        
        return self._repository.create_backup(backup_name)
    
    def get_by_id(self, id: int) -> Optional[Backup]:
        """Not implemented for backups (use get_all)"""
        return None
    
    def get_all(self) -> List[Backup]:
        """Get all available backups"""
        return self._repository.get_all()
    
    def update(self, entity: Backup) -> Backup:
        """Not implemented for backups"""
        raise NotImplementedError("Los backups no se pueden actualizar")
    
    def delete(self, id: int) -> bool:
        """Not implemented for backups (use delete_by_path)"""
        raise NotImplementedError("Use delete_by_path para eliminar backups")
    
    def restore_backup(self, backup_path: str) -> bool:
        """Restore database from backup"""
        if not backup_path:
            raise ValueError("La ruta del backup es requerida")
        
        return self._repository.restore_backup(backup_path)
    
    def delete_backup_by_path(self, backup_path: str) -> bool:
        """Delete backup by file path"""
        if not backup_path:
            raise ValueError("La ruta del backup es requerida")
        
        return self._repository.delete_backup(backup_path)
    
    def export_to_json(self, export_path: str) -> str:
        """Export all data to JSON format"""
        if not export_path:
            raise ValueError("La ruta de exportación es requerida")
        
        return self._repository.export_to_json(export_path)
    
    def import_from_json(self, import_path: str) -> bool:
        """Import data from JSON file"""
        if not import_path:
            raise ValueError("La ruta de importación es requerida")
        
        return self._repository.import_from_json(import_path)
    
    def cleanup_old_backups(self, keep_count: int = 10) -> int:
        """Clean up old backups, keeping only the most recent ones"""
        if keep_count < 1:
            raise ValueError("Debe mantener al menos 1 backup")
        
        return self._repository.cleanup_old_backups(keep_count)
