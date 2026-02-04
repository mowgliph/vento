import customtkinter as ctk
from tkinter import ttk
from typing import Callable, Optional
from src.features.products.models import Product, Currency
from src.features.products.service import ProductService
from src.features.exchange_rate.service import ExchangeRateService
from src.ui.components import create_entry_field, show_error, show_info, show_confirm, create_button

class ProductView:
    """View for product management"""
    
    def __init__(self, parent: ctk.CTkFrame, 
                 product_service: ProductService = None,
                 exchange_service: ExchangeRateService = None):
        self.parent = parent
        self.service = product_service or ProductService()
        self.exchange_service = exchange_service or ExchangeRateService()
        self.selected_product: Optional[Product] = None
        self.on_product_changed: Optional[Callable] = None
        
        self._build_ui()
        self._load_products()
    
    def _build_ui(self):
        # Main layout
        self.parent.grid_columnconfigure(0, weight=1)
        self.parent.grid_rowconfigure(0, weight=1)
        
        # Left panel - Product list
        list_frame = ctk.CTkFrame(self.parent)
        list_frame.grid(row=0, column=0, sticky="nsew", padx=10, pady=10)
        list_frame.grid_rowconfigure(1, weight=1)
        
        # Search bar
        search_frame = ctk.CTkFrame(list_frame, fg_color="transparent")
        search_frame.pack(fill="x", padx=10, pady=10)
        
        self.search_entry = ctk.CTkEntry(search_frame, placeholder_text="Buscar producto...")
        self.search_entry.pack(side="left", fill="x", expand=True, padx=(0, 10))
        self.search_entry.bind("<KeyRelease>", lambda e: self._on_search())
        
        ctk.CTkButton(search_frame, text="Buscar", width=80, 
                     command=self._on_search).pack(side="right")
        
        # Product tree
        columns = ("id", "name", "cost", "sale", "currency")
        self.tree = ttk.Treeview(list_frame, columns=columns, show="headings")
        self.tree.heading("id", text="ID")
        self.tree.heading("name", text="Nombre")
        self.tree.heading("cost", text="Costo")
        self.tree.heading("sale", text="Venta")
        self.tree.heading("currency", text="Moneda")
        
        self.tree.column("id", width=50)
        self.tree.column("name", width=200)
        self.tree.column("cost", width=100)
        self.tree.column("sale", width=100)
        self.tree.column("currency", width=80)
        
        scrollbar = ctk.CTkScrollbar(list_frame, command=self.tree.yview)
        self.tree.configure(yscrollcommand=scrollbar.set)
        
        self.tree.pack(side="left", fill="both", expand=True, padx=10, pady=5)
        scrollbar.pack(side="right", fill="y", pady=5)
        
        self.tree.bind("<<TreeviewSelect>>", self._on_select)
        
        # Right panel - Product form
        form_frame = ctk.CTkFrame(self.parent)
        form_frame.grid(row=0, column=1, sticky="nsew", padx=10, pady=10)
        
        ctk.CTkLabel(form_frame, text="Gestión de Productos", 
                    font=("Roboto", 20, "bold")).pack(pady=20)
        
        # Form fields
        form_container = ctk.CTkFrame(form_frame, fg_color="transparent")
        form_container.pack(fill="both", expand=True, padx=20, pady=10)
        
        self.name_entry = create_entry_field(form_container, "Nombre:")
        self.desc_entry = create_entry_field(form_container, "Descripción:")
        
        # Cost section
        cost_frame = ctk.CTkFrame(form_container, fg_color="transparent")
        cost_frame.pack(fill="x", padx=10, pady=5)
        
        ctk.CTkLabel(cost_frame, text="Precio Costo:", width=120, anchor="w").pack(side="left")
        self.cost_entry = ctk.CTkEntry(cost_frame, width=120)
        self.cost_entry.pack(side="left", padx=(10, 5))
        
        self.cost_currency = ctk.CTkOptionMenu(cost_frame, values=["USD", "CUP"], width=80)
        self.cost_currency.set("USD")
        self.cost_currency.pack(side="left")
        
        # Margin
        self.margin_entry = create_entry_field(form_container, "Margen (%):", "20")
        
        # Sale price section
        sale_frame = ctk.CTkFrame(form_container, fg_color="transparent")
        sale_frame.pack(fill="x", padx=10, pady=5)
        
        ctk.CTkLabel(sale_frame, text="Precio Venta:", width=120, anchor="w").pack(side="left")
        self.sale_entry = ctk.CTkEntry(sale_frame, width=120, placeholder_text="Auto")
        self.sale_entry.pack(side="left", padx=(10, 5))
        
        self.sale_currency = ctk.CTkOptionMenu(sale_frame, values=["USD", "CUP"], width=80)
        self.sale_currency.set("USD")
        self.sale_currency.pack(side="left")
        
        # Buttons
        btn_frame = ctk.CTkFrame(form_frame, fg_color="transparent")
        btn_frame.pack(fill="x", padx=20, pady=20)
        
        create_button(btn_frame, "Nuevo", self._clear_form, "info").pack(side="left", padx=5)
        create_button(btn_frame, "Guardar", self._save_product, "success").pack(side="left", padx=5)
        create_button(btn_frame, "Eliminar", self._delete_product, "danger").pack(side="left", padx=5)
        create_button(btn_frame, "Calcular", self._calculate_sale, "warning").pack(side="left", padx=5)
    
    def _load_products(self):
        for item in self.tree.get_children():
            self.tree.delete(item)
        
        products = self.service.get_all()
        rate = self.exchange_service.get_current_rate()
        
        for product in products:
            sale_price = product.get_final_sale_price(rate.usd_to_cup)
            sale_curr = product.sale_currency.value if product.sale_currency else product.cost_currency.value
            
            self.tree.insert("", "end", values=(
                product.id,
                product.name,
                f"${product.cost_price:.2f} {product.cost_currency.value}",
                f"${sale_price:.2f}",
                sale_curr
            ))
    
    def _on_search(self):
        query = self.search_entry.get()
        for item in self.tree.get_children():
            self.tree.delete(item)
        
        products = self.service.search(query)
        rate = self.exchange_service.get_current_rate()
        
        for product in products:
            sale_price = product.get_final_sale_price(rate.usd_to_cup)
            sale_curr = product.sale_currency.value if product.sale_currency else product.cost_currency.value
            
            self.tree.insert("", "end", values=(
                product.id,
                product.name,
                f"${product.cost_price:.2f} {product.cost_currency.value}",
                f"${sale_price:.2f}",
                sale_curr
            ))
    
    def _on_select(self, event):
        selection = self.tree.selection()
        if not selection:
            return
        
        item = self.tree.item(selection[0])
        product_id = item["values"][0]
        
        product = self.service.get_by_id(product_id)
        if product:
            self.selected_product = product
            self._populate_form(product)
    
    def _populate_form(self, product: Product):
        self.name_entry.delete(0, "end")
        self.name_entry.insert(0, product.name)
        
        self.desc_entry.delete(0, "end")
        self.desc_entry.insert(0, product.description)
        
        self.cost_entry.delete(0, "end")
        self.cost_entry.insert(0, str(product.cost_price))
        self.cost_currency.set(product.cost_currency.value)
        
        self.margin_entry.delete(0, "end")
        self.margin_entry.insert(0, str(product.margin_percent))
        
        self.sale_entry.delete(0, "end")
        if product.sale_price:
            self.sale_entry.insert(0, str(product.sale_price))
        
        if product.sale_currency:
            self.sale_currency.set(product.sale_currency.value)
        else:
            self.sale_currency.set(product.cost_currency.value)
    
    def _clear_form(self):
        self.selected_product = None
        self.name_entry.delete(0, "end")
        self.desc_entry.delete(0, "end")
        self.cost_entry.delete(0, "end")
        self.cost_currency.set("USD")
        self.margin_entry.delete(0, "end")
        self.margin_entry.insert(0, "20")
        self.sale_entry.delete(0, "end")
        self.sale_currency.set("USD")
        self.tree.selection_remove(self.tree.selection())
    
    def _get_form_data(self) -> Product:
        sale_price = self.sale_entry.get()
        sale_currency = self.sale_currency.get()
        
        return Product(
            id=self.selected_product.id if self.selected_product else None,
            name=self.name_entry.get(),
            description=self.desc_entry.get(),
            cost_price=float(self.cost_entry.get() or 0),
            cost_currency=Currency(self.cost_currency.get()),
            margin_percent=float(self.margin_entry.get() or 0),
            sale_price=float(sale_price) if sale_price else None,
            sale_currency=Currency(sale_currency) if sale_price else None
        )
    
    def _calculate_sale(self):
        try:
            product = self._get_form_data()
            rate = self.exchange_service.get_current_rate()
            calculated = product.calculate_auto_sale_price(rate.usd_to_cup)
            
            self.sale_entry.delete(0, "end")
            self.sale_entry.insert(0, f"{calculated:.2f}")
            
        except ValueError as e:
            show_error(str(e))
    
    def _save_product(self):
        try:
            product = self._get_form_data()
            
            if product.id:
                self.service.update(product)
                show_info("Producto actualizado correctamente")
            else:
                self.service.create(product)
                show_info("Producto creado correctamente")
            
            self._load_products()
            self._clear_form()
            
            if self.on_product_changed:
                self.on_product_changed()
                
        except ValueError as e:
            show_error(str(e))
    
    def _delete_product(self):
        if not self.selected_product:
            show_error("Seleccione un producto para eliminar")
            return
        
        if show_confirm(f"¿Eliminar el producto '{self.selected_product.name}'?"):
            try:
                self.service.delete(self.selected_product.id)
                self._load_products()
                self._clear_form()
                show_info("Producto eliminado")
                
                if self.on_product_changed:
                    self.on_product_changed()
                    
            except Exception as e:
                show_error(f"Error al eliminar: {str(e)}")
