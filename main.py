#!/usr/bin/env python3
"""
Vento - Sistema de Gestión de Tienda
Aplicación de escritorio para gestión de productos y ventas
"""

import sys
from pathlib import Path

# Add src to path
sys.path.insert(0, str(Path(__file__).parent / "src"))

from src.ui.main_window import MainWindow

def main():
    """Main entry point"""
    app = MainWindow()
    app.run()

if __name__ == "__main__":
    main()