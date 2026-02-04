import customtkinter as ctk
from tkinter import ttk
from datetime import datetime
from typing import Callable, Optional, List
from src.features.sales.models import Sale
from src.features.sales.service import SaleService
from src.features.products.models import Product
from src.features.products.service import ProductService
from src.features.exchange_rate.service import ExchangeRateService
from src.ui.components import create_entry_field, show_error, show_info, show_confirm, create_button

class SaleView:
    """View for sales management"""
    
    def __init__(self, parent: ctk.CTkFrame,
                 sale_service: SaleService = None,
                 product_service: ProductService = None,
                 exchange_service: ExchangeRateService = None):
        self.parent = parent
        self.service = sale_service or SaleService()
        self.product_service = product_service or ProductService()
        self.exchange_service = exchange_service or ExchangeRateService()
        self.selected_sale: Optional[Sale] = None
        self.on_sale_changed: Optional[Callable] = None
        
        self._build_ui()
        self._load_sales()
        self._load_products_combo()
    
    def _build_ui(self):
        self.parent.grid_columnconfigure(0, weight=1)
        self.parent.grid_rowconfigure(0, weight=1)
        
        # Left panel - Sales list
        list_frame = ctk.CTkFrame(self.parent)
        list_frame.grid(row=0, column=0, sticky="nsew", padx=10, pady=10)
        list_frame.grid_rowconfigure(1, weight=1)
        
        # Filter
        filter_frame = ctk.CTkFrame(list_frame, fg_color="transparent")
        filter_frame.pack(fill="x", padx=10, pady=10)
        
        ctk.CTkLabel(filter_frame, text="Filtro:").pack(side="left", padx=(0, 10))
        self.filter_combo = ctk.CTkOptionMenu(filter_frame, values=["Hoy", "Esta semana", "Este mes", "Todas"],
                                              command=self._on_filter)
        self.filter_combo.set("Hoy")
        self.filter_combo.pack(side="left")
        
        # Sales tree
        columns = ("id", "date", "product", "quantity", "price", "total")
        self.tree = ttk.Treeview(list_frame, columns=columns, show="headings")
        self.tree.heading("id", text="ID")
        self.tree.heading("date", text="Fecha")
        self.tree.heading("product", text="Producto")
        self.tree.heading("quantity", text="Cant")
        self.tree.heading("price", text="Precio")
        self.tree.heading("total", text="Total")
        
        self.tree.column("id", width=50)
        self.tree.column("date", width=120)
        self.tree.column("product", width=180)
        self.tree.column("quantity", width=50)
        self.tree.column("price", width=100)
        self.tree.column("total", width=100)
        
        scrollbar = ctk.CTkScrollbar(list_frame, command=self.tree.yview)
        self.tree.configure(yscrollcommand=scrollbar.set)
        
        self.tree.pack(side="left", fill="both", expand=True, padx=10, pady=5)
        scrollbar.pack(side="right", fill="y", pady=5)
        
        self.tree.bind("<<TreeviewSelect>>", self._on_select)
        
        # Right panel - Sale form
        form_frame = ctk.CTkFrame(self.parent)
        form_frame.grid(row=0, column=1, sticky="nsew", padx=10, pady=10)
        
        ctk.CTkLabel(form_frame, text="Registro de Ventas", 
                    font=("Roboto", 20, "bold")).pack(pady=20)
        
        form_container = ctk.CTkFrame(form_frame, fg_color="transparent")
        form_container.pack(fill="both", expand=True, padx=20, pady=10)
        
        # Product selection
        prod_frame = ctk.CTkFrame(form_container, fg_color="transparent")
        prod_frame.pack(fill="x", padx=10, pady=5)
        
        ctk.CTkLabel(prod_frame, text="Producto:", width=120, anchor="w").pack(side="left")
        self.product_combo = ctk.CTkComboBox(prod_frame, values=[], width=200)
        self.product_combo.pack(side="left", padx=(10, 0))
        self.product_combo.bind("<<ComboboxSelected>>", self._on_product_select)
        
        # Quantity
        qty_frame = ctk.CTkFrame(form_container, fg_color="transparent")
        qty_frame.pack(fill="x", padx=10, pady=5)
        
        ctk.CTkLabel(qty_frame, text="Cantidad:", width=120, anchor="w").pack(side="left")
        self.quantity_entry = ctk.CTkEntry(qty_frame, width=80)
        self.quantity_entry.insert(0, "1")
        self.quantity_entry.pack(side="left", padx=(10, 10))
        
        # Sale price
        price_frame = ctk.CTkFrame(form_container, fg_color="transparent")
        price_frame.pack(fill="x", padx=10, pady=5)
        
        ctk.CTkLabel(price_frame, text="Precio Venta:", width=120, anchor="w").pack(side="left")
        self.price_entry = ctk.CTkEntry(price_frame, width=100)
        self.price_entry.pack(side="left", padx=(10, 5))
        
        self.price_currency = ctk.CTkOptionMenu(price_frame, values=["USD", "CUP"], width=80)
        self.price_currency.set("USD")
        self.price_currency.pack(side="left")
        
        # Auto-fill price button
        ctk.CTkButton(form_container, text="Usar precio del producto", 
                     command=self._auto_fill_price).pack(pady=10)
        
        # Info label
        self.info_label = ctk.CTkLabel(form_container, text="", 
                                      font=("Roboto", 12), text_color="gray")
        self.info_label.pack(pady=10)
        
        # Buttons
        btn_frame = ctk.CTkFrame(form_frame, fg_color="transparent")
        btn_frame.pack(fill="x", padx=20, pady=20)
        
        create_button(btn_frame, "Nueva", self._clear_form, "info").pack(side="left", padx=5)
        create_button(btn_frame, "Registrar", self._save_sale, "success").pack(side="left", padx=5)
        create_button(btn_frame, "Eliminar", self._delete_sale, "danger").pack(side="left", padx=5)
    
    def _load_products_combo(self):
        products = self.product_service.get_all()
        self.products_map = {f"{p.id}: {p.name}": p for p in products}
        values = list(self.products_map.keys())
        self.product_combo.configure(values=values)
        if values:
            self.product_combo.set(values[0])
    
    def _load_sales(self, sales: List[Sale] = None):
        for item in self.tree.get_children():
            self.tree.delete(item)
        
        if sales is None:
            sales = self.service.get_daily_sales()
        
        for sale in sales:
            total = sale.sale_price * sale.quantity
            self.tree.insert("", "end", values=(
                sale.id,
                sale.sale_date.strftime("%d/%m/%Y %H:%M"),
                sale.product_name,
                sale.quantity,
                f"${sale.sale_price:.2f} {sale.sale_currency}",
                f"${total:.2f}"
            ))
    
    def _on_filter(self, choice):
        from datetime import datetime, timedelta
        
        if choice == "Hoy":
            sales = self.service.get_daily_sales()
        elif choice == "Esta semana":
            end = datetime.now()
            start = end - timedelta(days=7)
            sales = self.service.get_by_date_range(start, end)
        elif choice == "Este mes":
            end = datetime.now()
            start = end - timedelta(days=30)
            sales = self.service.get_by_date_range(start, end)
        else:
            sales = self.service.get_all()
        
        self._load_sales(sales)
    
    def _on_select(self, event):
        selection = self.tree.selection()
        if not selection:
            return
        
        item = self.tree.item(selection[0])
        sale_id = item["values"][0]
        
        sale = self.service.get_by_id(sale_id)
        if sale:
            self.selected_sale = sale
            self._populate_form(sale)
    
    def _populate_form(self, sale: Sale):
        product_key = f"{sale.product_id}: {sale.product_name}"
        self.product_combo.set(product_key)
        self.quantity_entry.delete(0, "end")
        self.quantity_entry.insert(0, str(sale.quantity))
        self.price_entry.delete(0, "end")
        self.price_entry.insert(0, str(sale.sale_price))
        self.price_currency.set(sale.sale_currency)
    
    def _clear_form(self):
        self.selected_sale = None
        if self.product_combo.cget("values"):
            self.product_combo.set(self.product_combo.cget("values")[0])
        self.quantity_entry.delete(0, "end")
        self.quantity_entry.insert(0, "1")
        self.price_entry.delete(0, "end")
        self.price_currency.set("USD")
        self.info_label.configure(text="")
        self.tree.selection_remove(self.tree.selection())
    
    def _on_product_select(self, event=None):
        selection = self.product_combo.get()
        if selection in self.products_map:
            product = self.products_map[selection]
            rate = self.exchange_service.get_current_rate()
            price = product.get_final_sale_price(rate.usd_to_cup)
            currency = product.sale_currency.value if product.sale_currency else product.cost_currency.value
            self.info_label.configure(
                text=f"Precio sugerido: ${price:.2f} {currency} (Margen: {product.margin_percent}%)"
            )
    
    def _auto_fill_price(self):
        selection = self.product_combo.get()
        if selection in self.products_map:
            product = self.products_map[selection]
            rate = self.exchange_service.get_current_rate()
            price = product.get_final_sale_price(rate.usd_to_cup)
            currency = product.sale_currency.value if product.sale_currency else product.cost_currency.value
            
            self.price_entry.delete(0, "end")
            self.price_entry.insert(0, f"{price:.2f}")
            self.price_currency.set(currency)
    
    def _get_form_data(self) -> Sale:
        selection = self.product_combo.get()
        if not selection or selection not in self.products_map:
            raise ValueError("Seleccione un producto válido")
        
        product = self.products_map[selection]
        
        return Sale(
            id=self.selected_sale.id if self.selected_sale else None,
            product_id=product.id,
            product_name=product.name,
            sale_price=float(self.price_entry.get() or 0),
            sale_currency=self.price_currency.get(),
            quantity=int(self.quantity_entry.get() or 1)
        )
    
    def _save_sale(self):
        try:
            sale = self._get_form_data()
            
            if sale.id:
                self.service.update(sale)
                show_info("Venta actualizada correctamente")
            else:
                self.service.create(sale)
                show_info("Venta registrada correctamente")
            
            self._load_sales()
            self._clear_form()
            
            if self.on_sale_changed:
                self.on_sale_changed()
                
        except ValueError as e:
            show_error(str(e))
    
    def _delete_sale(self):
        if not self.selected_sale:
            show_error("Seleccione una venta para eliminar")
            return
        
        if show_confirm("¿Eliminar esta venta?"):
            try:
                self.service.delete(self.selected_sale.id)
                self._load_sales()
                self._clear_form()
                show_info("Venta eliminada")
                
                if self.on_sale_changed:
                    self.on_sale_changed()
                    
            except Exception as e:
                show_error(f"Error al eliminar: {str(e)}")
