import customtkinter as ctk
from tkinter import messagebox
from typing import Callable, Optional

def create_entry_field(parent, label_text: str, default_value: str = "", 
                       show: str = None, width: int = 200) -> ctk.CTkEntry:
    """Create a labeled entry field"""
    frame = ctk.CTkFrame(parent, fg_color="transparent")
    frame.pack(fill="x", padx=10, pady=5)
    
    label = ctk.CTkLabel(frame, text=label_text, width=120, anchor="w")
    label.pack(side="left")
    
    entry = ctk.CTkEntry(frame, width=width, show=show)
    entry.insert(0, default_value)
    entry.pack(side="left", padx=(10, 0))
    
    return entry

def create_labeled_frame(parent, title: str) -> ctk.CTkFrame:
    """Create a frame with a title label"""
    frame = ctk.CTkFrame(parent)
    label = ctk.CTkLabel(frame, text=title, font=("Roboto", 16, "bold"))
    label.pack(pady=10)
    return frame

def show_error(message: str):
    """Show error message dialog"""
    messagebox.showerror("Error", message)

def show_info(message: str):
    """Show info message dialog"""
    messagebox.showinfo("Información", message)

def show_confirm(message: str) -> bool:
    """Show confirmation dialog"""
    return messagebox.askyesno("Confirmar", message)

def create_button(parent, text: str, command: Callable, 
                  color: str = "primary", width: int = 120) -> ctk.CTkButton:
    """Create a styled button"""
    colors = {
        "primary": "#3B82F6",
        "success": "#10B981", 
        "danger": "#EF4444",
        "warning": "#F59E0B",
        "info": "#06B6D4"
    }
    
    fg_color = colors.get(color, colors["primary"])
    
    return ctk.CTkButton(
        parent, text=text, command=command, 
        width=width, fg_color=fg_color,
        hover_color=fg_color
    )
