#===============================================================================
# src/core/CMakeLists.txt
# Biblioteca Core - Utilidades compartidas
#===============================================================================

set(CORE_SOURCES
    database/DatabaseManager.cpp
    logger/Logger.cpp
)

set(CORE_HEADERS
    database/DatabaseManager.hpp
    database/DatabaseSchema.hpp
    logger/Logger.hpp
    utils/EventBus.hpp
    utils/Types.hpp
)

#===============================================================================
# Biblioteca estática Core
#===============================================================================
add_library(VentoCore STATIC
    ${CORE_SOURCES}
    ${CORE_HEADERS}
)

# Alias para uso limpio
add_library(Vento::Core ALIAS VentoCore)

#===============================================================================
# Configuración de la biblioteca
#===============================================================================
target_include_directories(VentoCore
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(VentoCore
    PUBLIC
        Qt6::Core
        Qt6::Sql
)

# Features de C++20
target_compile_features(VentoCore PUBLIC cxx_std_20)
