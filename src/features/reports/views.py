import customtkinter as ctk
from tkinter import ttk, scrolledtext
from datetime import datetime
from src.features.reports.service import ReportService
from src.ui.components import create_button, show_error

class ReportsView:
    """View for reports and printing"""
    
    def __init__(self, parent: ctk.CTkFrame, service: ReportService = None):
        self.parent = parent
        self.service = service or ReportService()
        
        self._build_ui()
    
    def _build_ui(self):
        self.parent.grid_columnconfigure(0, weight=1)
        self.parent.grid_rowconfigure(1, weight=1)
        
        # Controls
        controls_frame = ctk.CTkFrame(self.parent)
        controls_frame.grid(row=0, column=0, sticky="ew", padx=10, pady=10)
        
        ctk.CTkLabel(controls_frame, text="Reportes de Ventas", 
                    font=("Roboto", 20, "bold")).pack(side="left", padx=10)
        
        ctk.CTkLabel(controls_frame, text="Período:").pack(side="left", padx=(20, 5))
        self.period_combo = ctk.CTkOptionMenu(controls_frame, 
                                              values=["Hoy", "Esta semana", "Este mes", "Personalizado"],
                                              command=self._on_period_change)
        self.period_combo.set("Hoy")
        self.period_combo.pack(side="left", padx=5)
        
        create_button(controls_frame, "Generar", self._generate_report, "success").pack(side="left", padx=5)
        create_button(controls_frame, "Imprimir", self._print_report, "info").pack(side="left", padx=5)
        
        # Report display
        self.report_text = scrolledtext.ScrolledText(self.parent, wrap="word", 
                                                     font=("Courier", 10))
        self.report_text.grid(row=1, column=0, sticky="nsew", padx=10, pady=10)
        
        # Summary frame
        self.summary_frame = ctk.CTkFrame(self.parent)
        self.summary_frame.grid(row=2, column=0, sticky="ew", padx=10, pady=10)
        
        self.summary_label = ctk.CTkLabel(self.summary_frame, 
                                         text="Genera un reporte para ver el resumen",
                                         font=("Roboto", 14))
        self.summary_label.pack(pady=10)
    
    def _on_period_change(self, choice):
        pass  # Just placeholder
    
    def _generate_report(self):
        try:
            period = self.period_combo.get()
            
            if period == "Hoy":
                report = self.service.generate_daily_report()
            elif period == "Esta semana":
                from datetime import datetime, timedelta
                end = datetime.now()
                start = end - timedelta(days=7)
                report = self.service.generate_sales_report(start, end)
            elif period == "Este mes":
                from datetime import datetime, timedelta
                end = datetime.now()
                start = end - timedelta(days=30)
                report = self.service.generate_sales_report(start, end)
            else:
                report = self.service.generate_daily_report()
            
            # Display report
            text_content = self.service.export_to_text(report)
            self.report_text.delete("1.0", "end")
            self.report_text.insert("1.0", text_content)
            
            # Update summary
            summary_text = (
                f"Total USD: ${report.total_usd:.2f} | "
                f"Total CUP: ${report.total_cup:.2f} MN | "
                f"Equiv. USD: ${report.total_converted_to_usd:.2f} | "
                f"Equiv. CUP: ${report.total_converted_to_cup:.2f} MN"
            )
            self.summary_label.configure(text=summary_text)
            
        except Exception as e:
            show_error(f"Error al generar reporte: {str(e)}")
    
    def _print_report(self):
        content = self.report_text.get("1.0", "end")
        if not content.strip():
            show_error("Genere un reporte primero")
            return
        
        # Create print dialog
        print_window = ctk.CTkToplevel(self.parent)
        print_window.title("Vista Previa de Impresión")
        print_window.geometry("600x700")
        
        preview = scrolledtext.ScrolledText(print_window, wrap="word", 
                                           font=("Courier", 10))
        preview.pack(fill="both", expand=True, padx=10, pady=10)
        preview.insert("1.0", content)
        preview.config(state="disabled")
        
        def do_print():
            import tempfile
            import os
            
            # Save to temp file and open with default printer
            with tempfile.NamedTemporaryFile(mode='w', suffix='.txt', delete=False) as f:
                f.write(content)
                temp_path = f.name
            
            # Open file (this will open with default application)
            os.startfile(temp_path, "print")
            print_window.destroy()
        
        ctk.CTkButton(print_window, text="Imprimir", command=do_print).pack(pady=10)
