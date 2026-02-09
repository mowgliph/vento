# Vento - Sistema de Gestión de Tienda

Vento es una aplicación de escritorio para la gestión de productos y ventas, diseñada para simplificar las operaciones diarias de pequeñas y medianas tiendas.

## Características

- Gestión de productos
- Control de ventas
- Interfaz de usuario intuitiva
- Sistema de inventario integrado
- **Eliminación en cascada**: Al eliminar un producto, se eliminan automáticamente todas las ventas asociadas
- **Auditoría de operaciones**: Registro completo de todas las operaciones de datos

## Requisitos del Sistema

- Python 3.8 o superior
- Sistema operativo: Windows, macOS o Linux

## Instalación

### 1. Clonar el repositorio

```bash
git clone https://github.com/mowgliph/vento.git
cd vento
```

### 2. Crear entorno virtual (recomendado)

```bash
# En Windows
python -m venv venv
venv\Scripts\activate

# En macOS/Linux
python3 -m venv venv
source venv/bin/activate
```

### 3. Instalar dependencias

```bash
pip install -r requirements.txt
```

## Compilación

Vento es una aplicación Python que no requiere compilación tradicional. Sin embargo, puedes crear un ejecutable usando PyInstaller si lo deseas:

### Opcional: Crear ejecutable

```bash
# Instalar PyInstaller
pip install pyinstaller

# Crear ejecutable
pyinstaller --onefile --windowed main.py
```

El ejecutable se generará en la carpeta `dist/`.

## Ejecución

### Método 1: Ejecutar desde código fuente

```bash
python main.py
```

### Método 2: Usar el ejecutable (si fue creado)

```bash
# En Windows
dist/main.exe

# En macOS/Linux
./dist/main
```

## Estructura del Proyecto

```
vento/
├── main.py              # Punto de entrada principal
├── requirements.txt     # Dependencias del proyecto
├── src/                 # Código fuente
│   ├── core/           # Lógica central de la aplicación
│   ├── features/       # Módulos de funcionalidades
│   └── ui/             # Interfaz de usuario
├── docs/               # Documentación
└── README.md           # Este archivo
```

## Dependencias

- **customtkinter** (>=5.2.2) - Biblioteca para interfaz gráfica moderna

## Contribución

1. Fork del repositorio
2. Crear una rama para tu feature (`git checkout -b feature/nueva-funcionalidad`)
3. Commit tus cambios (`git commit -am 'Añadir nueva funcionalidad'`)
4. Push a la rama (`git push origin feature/nueva-funcionalidad`)
5. Crear un Pull Request

## Licencia

Este proyecto está licenciado bajo la Licencia MIT. Ver el archivo [LICENSE](LICENSE) para más detalles.

## Soporte

Si encuentras algún problema o tienes sugerencias, por favor abre un issue en el repositorio de GitHub.

## Eliminación en Cascada

El sistema implementa eliminación en cascada para mantener la integridad referencial de los datos:

### Comportamiento

- Al eliminar un producto, se eliminan automáticamente todas las ventas asociadas
- El sistema muestra una advertencia cuando el producto tiene ventas asociadas
- Se muestra un mensaje de confirmación detallando cuántas ventas se eliminarán
- La operación se registra en el log de auditoría

### Ejemplo de Flujo

1. Usuario selecciona un producto con 5 ventas asociadas
2. Sistema muestra: "¿Eliminar el producto 'Nombre Producto'? ⚠️ Este producto tiene 5 venta(s) asociada(s) que también se eliminarán."
3. Usuario confirma la eliminación
4. Sistema elimina el producto y las 5 ventas
5. Sistema muestra: "Producto eliminado junto con 5 venta(s) asociada(s)"
6. Operación registrada en auditoría

### Ventajas

- **Integridad de datos**: No quedan registros huérfanos
- **Experiencia de usuario**: Mensajes claros sobre el impacto de la eliminación
- **Auditoría completa**: Registro de todas las operaciones de eliminación
- **Prevención de errores**: Confirmación explícita antes de eliminar datos

## Tests

El proyecto incluye tests unitarios para validar la funcionalidad de eliminación en cascada:

```bash
# Ejecutar tests
python -m pytest tests/test_cascade_delete.py -v
```

Los tests cubren:
- Eliminación de productos sin ventas
- Eliminación de productos con ventas (valida cascade)
- Eliminación de múltiples productos con ventas
- Eliminación con muchas ventas (performance)

## Créditos

Desarrollado por [mowgliph](https://github.com/mowgliph)
