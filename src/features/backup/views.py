import customtkinter as ctk
from tkinter import ttk, filedialog
from datetime import datetime
from typing import Optional
from src.features.backup.models import Backup
from src.features.backup.service import BackupService
from src.ui.components import create_button, show_error, show_info, show_confirm

class BackupView:
    """View for backup management"""
    
    def __init__(self, parent: ctk.CTkFrame, service: BackupService = None):
        self.parent = parent
        self.service = service or BackupService()
        
        self._build_ui()
        self._load_backups()
    
    def _build_ui(self):
        self.parent.grid_columnconfigure(0, weight=1)
        self.parent.grid_rowconfigure(1, weight=1)
        
        # Title and controls
        title_frame = ctk.CTkFrame(self.parent)
        title_frame.grid(row=0, column=0, sticky="ew", padx=10, pady=10)
        
        ctk.CTkLabel(title_frame, text="Gestión de Backups", 
                    font=("Roboto", 20, "bold")).pack(side="left", padx=10)
        
        # Action buttons
        create_button(title_frame, "Crear Backup", self._create_backup, "success").pack(side="left", padx=5)
        create_button(title_frame, "Exportar JSON", self._export_json, "info").pack(side="left", padx=5)
        create_button(title_frame, "Importar JSON", self._import_json, "warning").pack(side="left", padx=5)
        create_button(title_frame, "Limpiar Antiguos", self._cleanup_old, "danger").pack(side="left", padx=5)
        
        # Main content with two panels
        content_frame = ctk.CTkFrame(self.parent)
        content_frame.grid(row=1, column=0, sticky="nsew", padx=10, pady=10)
        content_frame.grid_columnconfigure(0, weight=1)
        content_frame.grid_columnconfigure(1, weight=1)
        content_frame.grid_rowconfigure(0, weight=1)
        
        # Left panel - Backups list
        left_frame = ctk.CTkFrame(content_frame)
        left_frame.grid(row=0, column=0, sticky="nsew", padx=(0, 5), pady=10)
        left_frame.grid_rowconfigure(1, weight=1)
        
        ctk.CTkLabel(left_frame, text="Backups Disponibles", 
                    font=("Roboto", 16, "bold")).pack(pady=10)
        
        # Backups tree
        columns = ("name", "date", "size")
        self.tree = ttk.Treeview(left_frame, columns=columns, show="headings")
        self.tree.heading("name", text="Nombre")
        self.tree.heading("date", text="Fecha")
        self.tree.heading("size", text="Tamaño")
        
        self.tree.column("name", width=200)
        self.tree.column("date", width=150)
        self.tree.column("size", width=100)
        
        scrollbar = ctk.CTkScrollbar(left_frame, command=self.tree.yview)
        self.tree.configure(yscrollcommand=scrollbar.set)
        
        self.tree.pack(side="left", fill="both", expand=True, padx=10, pady=5)
        scrollbar.pack(side="right", fill="y", pady=5)
        
        self.tree.bind("<<TreeviewSelect>>", self._on_select)
        
        # Backup action buttons
        backup_btn_frame = ctk.CTkFrame(left_frame, fg_color="transparent")
        backup_btn_frame.pack(fill="x", padx=10, pady=10)
        
        create_button(backup_btn_frame, "Restaurar", self._restore_backup, "success").pack(side="left", padx=5)
        create_button(backup_btn_frame, "Eliminar", self._delete_backup, "danger").pack(side="left", padx=5)
        
        # Right panel - Info and logs
        right_frame = ctk.CTkFrame(content_frame)
        right_frame.grid(row=0, column=1, sticky="nsew", padx=(5, 0), pady=10)
        right_frame.grid_rowconfigure(1, weight=1)
        
        ctk.CTkLabel(right_frame, text="Información", 
                    font=("Roboto", 16, "bold")).pack(pady=10)
        
        # Selected backup info
        self.info_frame = ctk.CTkFrame(right_frame)
        self.info_frame.pack(fill="x", padx=10, pady=5)
        
        self.info_label = ctk.CTkLabel(self.info_frame, 
                                      text="Seleccione un backup para ver detalles",
                                      font=("Roboto", 12), text_color="gray")
        self.info_label.pack(pady=10)
        
        # Activity log
        log_frame = ctk.CTkFrame(right_frame)
        log_frame.pack(fill="both", expand=True, padx=10, pady=10)
        log_frame.grid_rowconfigure(0, weight=1)
        log_frame.grid_columnconfigure(0, weight=1)

        ctk.CTkLabel(log_frame, text="Registro de Actividad", 
                    font=("Roboto", 14, "bold")).grid(row=0, column=0, sticky="ew", pady=5)

        self.log_text = ctk.CTkTextbox(log_frame, height=200)
        self.log_text.grid(row=1, column=0, sticky="nsew", padx=5, pady=5)
        
        self._log("Sistema de backups inicializado")
    
    def _load_backups(self):
        """Load and display all backups"""
        for item in self.tree.get_children():
            self.tree.delete(item)
        
        backups = self.service.get_all()
        
        for backup in backups:
            date_str = backup.created_at.strftime("%d/%m/%Y %H:%M") if backup.created_at else "N/A"
            self.tree.insert("", "end", values=(
                backup.name,
                date_str,
                backup.size_formatted
            ))
        
        self._log(f"Cargados {len(backups)} backups")
    
    def _on_select(self, event):
        """Handle backup selection"""
        selection = self.tree.selection()
        if not selection:
            self._clear_info()
            return
        
        item = self.tree.item(selection[0])
        backup_name = item["values"][0]
        
        # Find backup by name
        backups = self.service.get_all()
        backup = next((b for b in backups if b.name == backup_name), None)
        
        if backup:
            self._display_backup_info(backup)
    
    def _display_backup_info(self, backup: Backup):
        """Display selected backup information"""
        self.selected_backup = backup
        
        info_text = f"""Nombre: {backup.name}
Ruta: {backup.file_path}
Fecha: {backup.created_at.strftime('%d/%m/%Y %H:%M:%S') if backup.created_at else 'N/A'}
Tamaño: {backup.size_formatted}"""
        
        self.info_label.configure(text=info_text, text_color="black")
    
    def _clear_info(self):
        """Clear backup info display"""
        self.selected_backup = None
        self.info_label.configure(
            text="Seleccione un backup para ver detalles",
            text_color="gray"
        )
    
    def _create_backup(self):
        """Create a new backup"""
        try:
            backup = self.service.create_backup()
            self._load_backups()
            show_info(f"Backup '{backup.name}' creado correctamente")
            self._log(f"Backup creado: {backup.name} ({backup.size_formatted})")
            
        except Exception as e:
            show_error(f"Error al crear backup: {str(e)}")
            self._log(f"Error al crear backup: {str(e)}")
    
    def _restore_backup(self):
        """Restore selected backup"""
        if not hasattr(self, 'selected_backup') or not self.selected_backup:
            show_error("Seleccione un backup para restaurar")
            return
        
        if show_confirm(f"¿Restaurar backup '{self.selected_backup.name}'?\n\n"
                       "Esto reemplazará todos los datos actuales."):
            try:
                success = self.service.restore_backup(self.selected_backup.file_path)
                if success:
                    show_info("Backup restaurado correctamente")
                    self._log(f"Backup restaurado: {self.selected_backup.name}")
                else:
                    show_error("Error al restaurar backup")
                    self._log(f"Error al restaurar backup: {self.selected_backup.name}")
                    
            except Exception as e:
                show_error(f"Error al restaurar backup: {str(e)}")
                self._log(f"Error al restaurar backup: {str(e)}")
    
    def _delete_backup(self):
        """Delete selected backup"""
        if not hasattr(self, 'selected_backup') or not self.selected_backup:
            show_error("Seleccione un backup para eliminar")
            return
        
        if show_confirm(f"¿Eliminar backup '{self.selected_backup.name}' permanentemente?"):
            try:
                success = self.service.delete_backup_by_path(self.selected_backup.file_path)
                if success:
                    self._load_backups()
                    self._clear_info()
                    show_info("Backup eliminado correctamente")
                    self._log(f"Backup eliminado: {self.selected_backup.name}")
                else:
                    show_error("Error al eliminar backup")
                    
            except Exception as e:
                show_error(f"Error al eliminar backup: {str(e)}")
                self._log(f"Error al eliminar backup: {str(e)}")
    
    def _export_json(self):
        """Export data to JSON file"""
        try:
            file_path = filedialog.asksaveasfilename(
                title="Exportar datos a JSON",
                defaultextension=".json",
                filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
            )
            
            if file_path:
                export_path = self.service.export_to_json(file_path)
                show_info(f"Datos exportados a: {export_path}")
                self._log(f"Datos exportados a: {export_path}")
                
        except Exception as e:
            show_error(f"Error al exportar datos: {str(e)}")
            self._log(f"Error al exportar datos: {str(e)}")
    
    def _import_json(self):
        """Import data from JSON file"""
        try:
            file_path = filedialog.askopenfilename(
                title="Importar datos desde JSON",
                filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
            )
            
            if file_path and show_confirm(
                "¿Importar datos desde el archivo seleccionado?\n\n"
                "Esto reemplazará todos los datos actuales."):
                
                success = self.service.import_from_json(file_path)
                if success:
                    show_info("Datos importados correctamente")
                    self._log(f"Datos importados desde: {file_path}")
                else:
                    show_error("Error al importar datos")
                    
        except Exception as e:
            show_error(f"Error al importar datos: {str(e)}")
            self._log(f"Error al importar datos: {str(e)}")
    
    def _cleanup_old(self):
        """Clean up old backups"""
        try:
            deleted = self.service.cleanup_old_backups(keep_count=5)
            self._load_backups()
            show_info(f"Se eliminaron {deleted} backups antiguos")
            self._log(f"Limpieza completada: {deleted} backups eliminados")
            
        except Exception as e:
            show_error(f"Error al limpiar backups: {str(e)}")
            self._log(f"Error al limpiar backups: {str(e)}")
    
    def _log(self, message: str):
        """Add message to activity log"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        self.log_text.insert("end", f"[{timestamp}] {message}\n")
        self.log_text.see("end")
