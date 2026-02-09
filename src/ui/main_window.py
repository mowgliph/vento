import customtkinter as ctk
from src.core.config import AppConfig
from src.features.products.views import ProductView
from src.features.sales.views import SaleView
from src.features.exchange_rate.views import ExchangeRateView
from src.features.reports.views import ReportsView
from src.features.backup.views import BackupView
from src.features.products.service import ProductService
from src.features.sales.service import SaleService
from src.features.exchange_rate.service import ExchangeRateService
from src.features.reports.service import ReportService
from src.features.backup.service import BackupService

class MainWindow:
    """Main application window with navigation"""
    
    def __init__(self):
        ctk.set_appearance_mode("light")
        ctk.set_default_color_theme("blue")
        
        self.root = ctk.CTk()
        self.root.title(AppConfig.APP_NAME)
        self.root.geometry(f"{AppConfig.WINDOW_WIDTH}x{AppConfig.WINDOW_HEIGHT}")
        
        # Shared services
        self.product_service = ProductService()
        self.sale_service = SaleService()
        self.exchange_service = ExchangeRateService()
        self.report_service = ReportService()
        self.backup_service = BackupService()
        
        self._build_ui()
    
    def _build_ui(self):
        # Main layout
        self.root.grid_columnconfigure(1, weight=1)
        self.root.grid_rowconfigure(0, weight=1)
        
        # Sidebar navigation
        sidebar = ctk.CTkFrame(self.root, width=200)
        sidebar.grid(row=0, column=0, sticky="nsew", padx=10, pady=10)
        sidebar.grid_rowconfigure(5, weight=1)
        
        ctk.CTkLabel(sidebar, text="VENTO", font=("Roboto", 24, "bold")).pack(pady=20)
        
        # Navigation buttons
        nav_buttons = [
            ("Productos", self._show_products, "primary"),
            ("Ventas", self._show_sales, "success"),
            ("Tasa de Cambio", self._show_exchange, "info"),
            ("Reportes", self._show_reports, "warning"),
            ("Backups", self._show_backups, "danger"),
        ]
        
        for text, command, color in nav_buttons:
            btn = ctk.CTkButton(sidebar, text=text, command=command, 
                               fg_color=self._get_color(color),
                               hover_color=self._get_color(color),
                               height=40)
            btn.pack(fill="x", padx=20, pady=5)
        
        # Version info at bottom
        ctk.CTkLabel(sidebar, text=f"v{AppConfig.APP_VERSION}", 
                    font=("Roboto", 10), text_color="gray").pack(side="bottom", pady=10)
        
        # Content area
        self.content_frame = ctk.CTkFrame(self.root)
        self.content_frame.grid(row=0, column=1, sticky="nsew", padx=10, pady=10)
        self.content_frame.grid_columnconfigure(0, weight=1)
        self.content_frame.grid_rowconfigure(0, weight=1)
        
        # Show products by default
        self._show_products()
    
    def _show_backups(self):
        self._clear_content()
        BackupView(self.content_frame, self.backup_service)
    
    def _get_color(self, name: str) -> str:
        colors = {
            "primary": "#3B82F6",
            "success": "#10B981",
            "info": "#06B6D4",
            "warning": "#F59E0B",
            "danger": "#EF4444"
        }
        return colors.get(name, colors["primary"])
    
    def _clear_content(self):
        for widget in self.content_frame.winfo_children():
            widget.destroy()
    
    def _show_products(self):
        self._clear_content()
        ProductView(self.content_frame, self.product_service, self.exchange_service)
    
    def _show_sales(self):
        self._clear_content()
        SaleView(self.content_frame, self.sale_service, self.product_service, self.exchange_service)
    
    def _show_exchange(self):
        self._clear_content()
        ExchangeRateView(self.content_frame, self.exchange_service)
    
    def _show_reports(self):
        self._clear_content()
        ReportsView(self.content_frame, self.report_service)
    
    def run(self):
        self.root.mainloop()
