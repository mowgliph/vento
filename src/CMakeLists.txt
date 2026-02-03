#===============================================================================
# src/CMakeLists.txt
# Configuración de subdirectorios y ejecutable principal
#===============================================================================

#===============================================================================
# Agregar bibliotecas de cada módulo
#===============================================================================

# Core - Utilidades compartidas
add_subdirectory(core)

# Features - Módulos de negocio
add_subdirectory(features)

# UI - Interfaz de usuario
add_subdirectory(ui)

#===============================================================================
# Ejecutable principal
#===============================================================================
qt_add_executable(${PROJECT_NAME}
    main.cpp
)

#===============================================================================
# Linkear bibliotecas
#===============================================================================
target_link_libraries(${PROJECT_NAME} PRIVATE
    # Qt6
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Sql
    Qt6::Widgets
    Qt6::PrintSupport
    
    # Módulos internos
    Vento::Core
    Vento::Features
    Vento::UI
)

#===============================================================================
# Propiedades del ejecutable
#===============================================================================
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "Vento"
)

# Copiar QML a directorio de build (desarrollo)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/ui/qml
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/qml
    )
endif()
