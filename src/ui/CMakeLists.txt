#===============================================================================
# src/ui/CMakeLists.txt
# Módulo de interfaz de usuario QML
#===============================================================================

#===============================================================================
# Módulo QML usando qt_add_qml_module
#===============================================================================
qt_add_library(VentoUI STATIC)
add_library(Vento::UI ALIAS VentoUI)

qt_add_qml_module(VentoUI
    URI Vento.UI
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/theme/Theme.qml
        qml/components/Sidebar.qml
        qml/components/AppHeader.qml
        qml/components/StatCard.qml
        qml/components/ProductCard.qml
        qml/components/SearchBar.qml
        qml/components/Toast.qml
        qml/views/DashboardView.qml
        qml/views/SalesView.qml
        qml/views/InventoryView.qml
        qml/views/SettingsView.qml
    RESOURCES
        qml/qml.qrc
    OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR}/Vento/UI
)

target_include_directories(VentoUI
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(VentoUI
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Qml
        Qt6::Quick
        Qt6::QuickControls2
)
