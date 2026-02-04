## Estructura del proyecto:

vento/
├── src/
│   ├── core/
│   │   ├── interfaces.py      # Clases abstractas (Repository, Service)
│   │   ├── database.py        # DatabaseManager singleton
│   │   └── config.py          # Configuración
│   ├── features/
│   │   ├── products/
│   │   │   ├── models.py      # Entidad Product
│   │   │   ├── repository.py  # ProductRepository
│   │   │   ├── service.py     # ProductService
│   │   │   └── views.py       # UI ProductView
│   │   ├── sales/
│   │   │   ├── models.py      # Entidad Sale
│   │   │   ├── repository.py  # SaleRepository
│   │   │   ├── service.py     # SaleService
│   │   │   └── views.py       # UI SaleView
│   │   ├── exchange_rate/
│   │   │   ├── models.py      # Entidad ExchangeRate
│   │   │   ├── repository.py  # ExchangeRateRepository
│   │   │   ├── service.py     # ExchangeRateService
│   │   │   └── views.py       # UI ExchangeRateView
│   │   └── reports/
│   │       ├── service.py     # ReportService
│   │       └── views.py       # UI ReportsView
│   └── ui/
│       ├── components.py      # Componentes reutilizables
│       └── main_window.py     # Ventana principal
├── main.py                    # Punto de entrada
└── requirements.txt