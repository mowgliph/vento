import customtkinter as ctk
from src.features.exchange_rate.service import ExchangeRateService
from src.ui.components import show_info, create_button

class ExchangeRateView:
    """View for exchange rate management"""
    
    def __init__(self, parent: ctk.CTkFrame, service: ExchangeRateService = None):
        self.parent = parent
        self.service = service or ExchangeRateService()
        
        self._build_ui()
        self._load_current_rate()
    
    def _build_ui(self):
        # Center content
        self.parent.grid_columnconfigure(0, weight=1)
        self.parent.grid_rowconfigure(0, weight=1)
        
        container = ctk.CTkFrame(self.parent)
        container.grid(row=0, column=0, padx=20, pady=20)
        
        ctk.CTkLabel(container, text="Tasa de Cambio", 
                    font=("Roboto", 24, "bold")).pack(pady=20)
        
        # Current rate display
        self.current_frame = ctk.CTkFrame(container)
        self.current_frame.pack(fill="x", padx=20, pady=10)
        
        ctk.CTkLabel(self.current_frame, text="Tasa actual:", 
                    font=("Roboto", 14)).pack(side="left", padx=10, pady=10)
        
        self.rate_label = ctk.CTkLabel(self.current_frame, text="1 USD = ? CUP", 
                                      font=("Roboto", 16, "bold"))
        self.rate_label.pack(side="left", padx=10, pady=10)
        
        # Update section
        update_frame = ctk.CTkFrame(container)
        update_frame.pack(fill="x", padx=20, pady=20)
        
        ctk.CTkLabel(update_frame, text="Nueva tasa:", 
                    font=("Roboto", 14)).pack(pady=(10, 5))
        
        input_frame = ctk.CTkFrame(update_frame, fg_color="transparent")
        input_frame.pack(pady=5)
        
        ctk.CTkLabel(input_frame, text="1 USD = ").pack(side="left")
        self.rate_entry = ctk.CTkEntry(input_frame, width=120)
        self.rate_entry.pack(side="left", padx=5)
        ctk.CTkLabel(input_frame, text=" CUP").pack(side="left")
        
        create_button(update_frame, "Actualizar Tasa", 
                     self._update_rate, "success").pack(pady=20)
        
        # Info
        ctk.CTkLabel(container, 
                    text="Esta tasa se utiliza para calcular precios de venta automáticos",
                    font=("Roboto", 12), text_color="gray").pack(pady=10)
    
    def _load_current_rate(self):
        rate = self.service.get_current_rate()
        self.rate_label.configure(text=f"1 USD = {rate.usd_to_cup:.2f} CUP")
        self.rate_entry.delete(0, "end")
        self.rate_entry.insert(0, str(rate.usd_to_cup))
    
    def _update_rate(self):
        try:
            new_rate = float(self.rate_entry.get())
            if new_rate <= 0:
                raise ValueError("La tasa debe ser mayor que cero")
            
            self.service.update_rate(new_rate)
            self._load_current_rate()
            show_info("Tasa de cambio actualizada correctamente")
            
        except ValueError as e:
            from src.ui.components import show_error
            show_error(str(e))
