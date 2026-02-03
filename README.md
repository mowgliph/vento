# 🛒 Vento

Sistema de gestión de inventario y ventas con soporte para múltiples monedas.

## 🏗️ Arquitectura

Este proyecto utiliza **Feature-Based Architecture** (Arquitectura por Características),
organizando el código por dominio de negocio en lugar de capas técnicas.

```
src/
├── core/           # Utilidades compartidas (DB, Logger, EventBus)
├── features/       # Dominios de negocio
│   ├── currency/   # Gestión de tipo de cambio
│   ├── inventory/  # Gestión de productos
│   ├── sales/      # Registro de ventas
│   └── reporting/  # Generación de reportes
└── ui/             # Interfaz de usuario (QML)
```

## 🔧 Requisitos

- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- Qt 6.5+
- CMake 3.21+

## 🚀 Compilación

### Windows

1. **Instalar requisitos:**
   - Descarga e instala [Qt 6.5+](https://www.qt.io/download)
   - Instala [Visual Studio 2019+](https://visualstudio.microsoft.com/) (con el componente "Desktop development with C++")
   - Asegúrate de tener CMake 3.21+

2. **Configurar variables de entorno:**
   ```powershell
   # Añade Qt al PATH (ajusta la ruta según tu instalación)
   $env:PATH += ";C:\Qt\6.5.0\msvc2019_64\bin"
   ```

3. **Compilar:**
   ```powershell
   mkdir build
   cd build
   cmake -G "Visual Studio 17 2022" ..
   cmake --build . --config Release
   ```

### Linux/macOS

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## 📋 Licencia

MIT License
