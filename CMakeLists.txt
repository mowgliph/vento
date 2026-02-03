#===============================================================================
# CMakeLists.txt - Vento
# Proyecto principal - Arquitectura Feature-Based
#===============================================================================
cmake_minimum_required(VERSION 3.21)

project(Vento
    VERSION 1.0.0
    DESCRIPTION "Sistema de gestión de inventario y ventas"
    LANGUAGES CXX
)

#===============================================================================
# Configuración de C++20
#===============================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Habilitar exportación de símbolos en Windows
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Compilación posicional independiente (para shared libraries)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#===============================================================================
# Configuración de Qt6
#===============================================================================
set(CMAKE_AUTOMOC ON)  # Auto Meta-Object Compiler
set(CMAKE_AUTORCC ON)  # Auto Resource Compiler
set(CMAKE_AUTOUIC ON)  # Auto UI Compiler

# Buscar paquetes Qt6 requeridos
find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Gui
    Qml
    Quick
    QuickControls2
    Sql
    Widgets
)

# Política para versiones de QML
qt_policy(SET QTP0001 NEW)

#===============================================================================
# Opciones de compilación
#===============================================================================
option(VENTO_BUILD_TESTS "Compilar tests unitarios" OFF)
option(VENTO_ENABLE_SANITIZERS "Habilitar sanitizers (debug)" OFF)

# Warnings estrictos
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -Werror=return-type
        -Wno-unused-parameter
    )
elseif(MSVC)
    add_compile_options(
        /W4
        /permissive-
        /utf-8
    )
endif()

# Sanitizers (solo en debug)
if(VENTO_ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-fsanitize=address,undefined)
    add_link_options(-fsanitize=address,undefined)
endif()

#===============================================================================
# Directorios de inclusión globales
#===============================================================================
include_directories(${CMAKE_SOURCE_DIR}/src)

#===============================================================================
# Variables para organización de archivos
#===============================================================================
set(VENTO_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(VENTO_CORE_DIR ${VENTO_SRC_DIR}/core)
set(VENTO_FEATURES_DIR ${VENTO_SRC_DIR}/features)
set(VENTO_UI_DIR ${VENTO_SRC_DIR}/ui)

#===============================================================================
# Subdirectorios del proyecto
#===============================================================================
add_subdirectory(src)

#===============================================================================
# Tests (opcional)
#===============================================================================
if(VENTO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

#===============================================================================
# Información de compilación
#===============================================================================
message(STATUS "")
message(STATUS "╔═══════════════════════════════════════════════════════════╗")
message(STATUS "║              Vento - Configuración                     ║")
message(STATUS "╠═══════════════════════════════════════════════════════════╣")
message(STATUS "║  Versión:        ${PROJECT_VERSION}")
message(STATUS "║  Compilador:     ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "║  Estándar C++:   ${CMAKE_CXX_STANDARD}")
message(STATUS "║  Qt Version:     ${Qt6_VERSION}")
message(STATUS "║  Build Type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "║  Tests:          ${VENTO_BUILD_TESTS}")
message(STATUS "╚═══════════════════════════════════════════════════════════╝")
message(STATUS "")
