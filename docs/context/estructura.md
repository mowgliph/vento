## 📁 Estructura Visual del Proyecto

```
Vento/
├── 📄 CMakeLists.txt                    # CMake principal
├── 📄 README.md
├── 📁 src/
│   ├── 📄 CMakeLists.txt
│   ├── 📄 main.cpp                      # Entry point
│   │
│   ├── 📁 core/                         # Shared/Core utilities
│   │   ├── 📄 CMakeLists.txt
│   │   ├── 📁 database/
│   │   │   ├── 📄 DatabaseManager.hpp
│   │   │   └── 📄 DatabaseManager.cpp
│   │   ├── 📁 logger/
│   │   │   ├── 📄 Logger.hpp
│   │   │   └── 📄 Logger.cpp
│   │   └── 📁 utils/
│   │       └── 📄 EventBus.hpp          # Observer Pattern
│   │
│   ├── 📁 features/
│   │   ├── 📄 CMakeLists.txt
│   │   │
│   │   ├── 📁 currency/                 # Feature: Tipo de cambio
│   │   │   ├── 📄 CMakeLists.txt
│   │   │   ├── 📄 CurrencyService.hpp
│   │   │   └── 📄 CurrencyService.cpp
│   │   │
│   │   ├── 📁 inventory/                # Feature: Inventario
│   │   │   ├── 📄 CMakeLists.txt
│   │   │   ├── 📄 InventoryService.hpp
│   │   │   └── 📄 InventoryService.cpp
│   │   │
│   │   └── 📁 sales/                    # Feature: Ventas
│   │       ├── 📄 CMakeLists.txt
│   │       ├── 📄 SalesService.hpp
│   │       └── 📄 SalesService.cpp
│   │
│   └── 📁 ui/
│       ├── 📄 CMakeLists.txt
│       └── 📁 qml/
│           ├── 📄 Main.qml
│           └── 📄 qml.qrc
│
├── 📁 resources/
│   └── 📄 app.qrc
│
└── 📁 tests/
    └── 📄 CMakeLists.txt
```