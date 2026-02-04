import os
import shutil
import gzip
import json
from pathlib import Path
from datetime import datetime
from typing import Optional, List, Dict
from dataclasses import dataclass

@dataclass
class BackupInfo:
    """Backup metadata"""
    name: str
    file_path: str
    created_at: datetime
    size: int
    size_formatted: str

class BackupManager:
    """Simple backup management for SQLite database"""
    
    def __init__(self, backup_dir: Optional[str] = None):
        if backup_dir is None:
            backup_dir = Path(__file__).parent.parent.parent / "backups"
        
        self.backup_dir = Path(backup_dir)
        self.backup_dir.mkdir(parents=True, exist_ok=True)
    
    def create_backup(self, backup_name: Optional[str] = None) -> str:
        """Create compressed backup of database"""
        from src.core.database import get_db
        
        if backup_name is None:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            backup_name = f"vento_backup_{timestamp}"
        
        backup_path = self.backup_dir / f"{backup_name}.gz"
        
        try:
            db_path = get_db().db_path
            
            # Compress database directly
            with open(db_path, 'rb') as f_in:
                with gzip.open(backup_path, 'wb') as f_out:
                    shutil.copyfileobj(f_in, f_out)
            
            # Create metadata
            self._create_metadata(backup_name, backup_path)
            
            return str(backup_path)
            
        except Exception as e:
            if backup_path.exists():
                backup_path.unlink()
            raise Exception(f"Error creating backup: {str(e)}")
    
    def restore_backup(self, backup_path: str) -> bool:
        """Restore database from compressed backup"""
        from src.core.database import get_db
        
        backup_file = Path(backup_path)
        
        if not backup_file.exists():
            raise FileNotFoundError(f"Backup file not found: {backup_path}")
        
        try:
            db_path = get_db().db_path
            
            # Create safety backup of current database
            safety_backup = self.backup_dir / f"pre_restore_{datetime.now().strftime('%Y%m%d_%H%M%S')}.gz"
            with open(db_path, 'rb') as f_in:
                with gzip.open(safety_backup, 'wb') as f_out:
                    shutil.copyfileobj(f_in, f_out)
            
            # Decompress and restore
            with gzip.open(backup_file, 'rb') as f_in:
                with open(db_path, 'wb') as f_out:
                    shutil.copyfileobj(f_in, f_out)
            
            return True
            
        except Exception as e:
            raise Exception(f"Error restoring backup: {str(e)}")
    
    def list_backups(self) -> List[BackupInfo]:
        """List all available backups"""
        backups = []
        
        for backup_file in self.backup_dir.glob("*.gz"):
            try:
                stat = backup_file.stat()
                created_at = datetime.fromtimestamp(stat.st_ctime)
                size = stat.st_size
                
                backups.append(BackupInfo(
                    name=backup_file.stem,
                    file_path=str(backup_file),
                    created_at=created_at,
                    size=size,
                    size_formatted=self._format_size(size)
                ))
            except Exception:
                continue
        
        return sorted(backups, key=lambda x: x.created_at, reverse=True)
    
    def delete_backup(self, backup_path: str) -> bool:
        """Delete backup file and its metadata"""
        try:
            backup_file = Path(backup_path)
            metadata_file = backup_file.with_suffix('.json')
            
            if backup_file.exists():
                backup_file.unlink()
            
            if metadata_file.exists():
                metadata_file.unlink()
            
            return True
        except Exception:
            return False
    
    def export_to_json(self, export_path: str) -> str:
        """Export all data to JSON format"""
        from src.core.database import get_db
        import sqlite3
        
        try:
            with get_db().get_connection() as conn:
                data = {}
                cursor = conn.cursor()
                
                # Get all tables
                cursor.execute("SELECT name FROM sqlite_master WHERE type='table'")
                tables = cursor.fetchall()
                
                for table in tables:
                    table_name = table[0]
                    cursor.execute(f"SELECT * FROM {table_name}")
                    rows = cursor.fetchall()
                    columns = [description[0] for description in cursor.description]
                    
                    data[table_name] = {
                        'columns': columns,
                        'rows': [dict(zip(columns, row)) for row in rows]
                    }
                
                # Write JSON export
                export_file = Path(export_path)
                with open(export_file, 'w', encoding='utf-8') as f:
                    json.dump(data, f, indent=2, default=str)
                
                return str(export_file)
                
        except Exception as e:
            raise Exception(f"Error exporting data: {str(e)}")
    
    def import_from_json(self, import_path: str) -> bool:
        """Import data from JSON file"""
        from src.core.database import get_db
        import sqlite3
        
        try:
            with open(import_path, 'r', encoding='utf-8') as f:
                data = json.load(f)
            
            with get_db().get_connection() as conn:
                cursor = conn.cursor()
                
                for table_name, table_data in data.items():
                    if table_name == 'sqlite_sequence':
                        continue
                    
                    # Clear existing data
                    cursor.execute(f"DELETE FROM {table_name}")
                    
                    # Insert new data
                    if table_data['rows']:
                        columns = table_data['columns']
                        placeholders = ', '.join(['?' for _ in columns])
                        insert_sql = f"INSERT INTO {table_name} ({', '.join(columns)}) VALUES ({placeholders})"
                        
                        for row in table_data['rows']:
                            values = [row.get(col) for col in columns]
                            cursor.execute(insert_sql, values)
                
                conn.commit()
            
            return True
            
        except Exception as e:
            raise Exception(f"Error importing data: {str(e)}")
    
    def _create_metadata(self, backup_name: str, backup_path: Path):
        """Create backup metadata"""
        metadata = {
            "name": backup_name,
            "created_at": datetime.now().isoformat(),
            "size": backup_path.stat().st_size,
            "version": "1.0"
        }
        
        metadata_file = backup_path.with_suffix('.json')
        with open(metadata_file, 'w') as f:
            json.dump(metadata, f, indent=2)
    
    def _format_size(self, size_bytes: int) -> str:
        """Format file size for display"""
        for unit in ['B', 'KB', 'MB', 'GB']:
            if size_bytes < 1024:
                return f"{size_bytes:.1f} {unit}"
            size_bytes /= 1024
        return f"{size_bytes:.1f} TB"
    
    def cleanup_old_backups(self, keep_count: int = 10) -> int:
        """Remove old backups, keeping only the most recent ones"""
        backups = self.list_backups()
        
        if len(backups) <= keep_count:
            return 0
        
        deleted = 0
        for backup in backups[keep_count:]:
            if self.delete_backup(backup.file_path):
                deleted += 1
        
        return deleted

# Global backup manager instance
_backup_manager = None

def get_backup_manager() -> BackupManager:
    """Get global backup manager instance"""
    global _backup_manager
    if _backup_manager is None:
        _backup_manager = BackupManager()
    return _backup_manager
