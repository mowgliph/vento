# El Plan del Arquitecto 🏗️

Como arquitecto de software, he diseñado una estructura basada en **Feature Slicing** (Arquitectura por Características). En lugar de separar por capas (Modelos, Vistas, Controladores), separaremos por *dominio del negocio*. Esto facilita mantener SOLID y escalar.

### Stack Tecnológico Definido

* **Lenguaje:** C++ (Estándar C++17 o C++20).
* **UI Framework:** Qt 6 (QML) - Estilo `Fluent` o `Material` para simular Windows 11.
* **Base de Datos:** SQLite (Nativo, sin ORMs pesados, usaremos un wrapper ligero).
* **Build System:** CMake.

### Mapa de Características (Folder Structure)

La estructura de carpetas que la IA deberá generar será así:

1. **Shared/Core:** (Base de datos, Logs, Utils).
2. **Features/Currency:** (Manejo del precio del dólar).
3. **Features/Inventory:** (Productos, cálculo de precios, márgenes).
4. **Features/Sales:** (Registro de ventas, historial).
5. **Features/Reporting:** (Generación de reportes).

### Diagrama de Flujo de Datos

1. El usuario actualiza el **Dólar**.
2. El sistema dispara un evento (Observer Pattern) a **Inventario**.
3. **Inventario** recalcula `Precio Venta = (CostoUSD * Tasa) + Margen`.
4. **Ventas** lee el inventario actualizado y registra la transacción.