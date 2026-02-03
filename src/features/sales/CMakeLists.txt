#===============================================================================
# src/features/sales/CMakeLists.txt
# Feature: Registro de ventas
#===============================================================================

set(SALES_SOURCES
    SalesService.cpp
    SalesRepository.cpp
    SalesModel.cpp
    CartModel.cpp
)

set(SALES_HEADERS
    SalesService.hpp
    SalesTypes.hpp
    Sale.hpp
    SaleItem.hpp
    SalesRepository.hpp
    SalesModel.hpp
    CartModel.hpp
)

add_library(VentoSales STATIC
    ${SALES_SOURCES}
    ${SALES_HEADERS}
)

add_library(Vento::Sales ALIAS VentoSales)

target_include_directories(VentoSales
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(VentoSales
    PUBLIC
        Qt6::Core
        Qt6::Qml
        Vento::Core
        Vento::Inventory
)
