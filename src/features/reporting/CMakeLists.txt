#===============================================================================
# src/features/reporting/CMakeLists.txt
# Módulo de reportes
#===============================================================================

# Archivos fuente del módulo de reportes
set(REPORTING_SOURCES
    ReportingTypes.hpp
    ReportingRepository.hpp
    ReportingRepository.cpp
    ReportingService.hpp
    ReportingService.cpp
)

# Crear biblioteca estática para el módulo de reportes
add_library(VentoReporting STATIC ${REPORTING_SOURCES})
add_library(Vento::Reporting ALIAS VentoReporting)

# Incluir directorios
target_include_directories(VentoReporting
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# Linkear dependencias
target_link_libraries(VentoReporting
    PUBLIC
        Vento::Core
    PRIVATE
        Qt6::Core
        Qt6::Sql
        Qt6::PrintSupport
)

# Propiedades del objetivo
set_target_properties(VentoReporting PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)
