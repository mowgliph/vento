#===============================================================================
# src/features/CMakeLists.txt
# Módulos de características de negocio
#===============================================================================

# Agregar cada feature como submódulo
add_subdirectory(currency)
add_subdirectory(inventory)
add_subdirectory(sales)
add_subdirectory(reporting)

#===============================================================================
# Biblioteca agregadora de Features
#===============================================================================
add_library(VentoFeatures INTERFACE)
add_library(Vento::Features ALIAS VentoFeatures)

target_link_libraries(VentoFeatures
    INTERFACE
        Vento::Currency
        Vento::Inventory
        Vento::Sales
        Vento::Reporting
)
