#===============================================================================
# src/features/inventory/CMakeLists.txt
# Feature: Gestión de inventario
#===============================================================================

set(INVENTORY_SOURCES
    InventoryService.cpp
    ProductRepository.cpp
    ProductModel.cpp
)

set(INVENTORY_HEADERS
    InventoryTypes.hpp
    Product.hpp
    ProductRepository.hpp
    ProductModel.hpp
    InventoryService.hpp
)

add_library(VentoInventory STATIC
    ${INVENTORY_SOURCES}
    ${INVENTORY_HEADERS}
)

add_library(Vento::Inventory ALIAS VentoInventory)

target_include_directories(VentoInventory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(VentoInventory
    PUBLIC
        Qt6::Core
        Qt6::Qml
        Vento::Core
        Vento::Currency
)
