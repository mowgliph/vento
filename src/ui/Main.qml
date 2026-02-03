//==============================================================================
// Main.qml
// Vento - Ventana principal de la aplicación
//==============================================================================

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import Vento.Reporting 1.0
import Vento.Currency 1.0
import Vento.Inventory 1.0
import Vento.Sales 1.0
import "../qml/theme/Theme.qml" as Theme

ApplicationWindow {
    id: mainWindow

    //--------------------------------------------------------------------------
    // Propiedades de la ventana
    //--------------------------------------------------------------------------
    width: 1280
    height: 720
    minimumWidth: 800
    minimumHeight: 600
    visible: true
    title: qsTr("Vento - Sistema de Gestión")

    //--------------------------------------------------------------------------
    // Tema Material (Windows 11 Style)
    //--------------------------------------------------------------------------
    Material.theme: Material.Light
    Material.accent: Theme.primary
    Material.primary: Theme.primaryDark
    Material.foreground: Theme.textPrimary
    Material.background: Theme.background

    //--------------------------------------------------------------------------
    // Propiedades personalizadas
    //--------------------------------------------------------------------------
    property string currentView: "dashboard"
    property bool sidebarCollapsed: false

    //--------------------------------------------------------------------------
    // Fondo de la aplicación
    //--------------------------------------------------------------------------
    background: Rectangle {
        color: Theme.background
    }

    //--------------------------------------------------------------------------
    // Header de la aplicación
    //--------------------------------------------------------------------------
    header: ToolBar {
        id: appHeader
        height: 48

        Material.elevation: 2
        Material.background: Theme.surface

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 16
            anchors.rightMargin: 16
            spacing: 16

            // Botón de menú hamburguesa
            ToolButton {
                icon.source: "qrc:/icons/menu.svg"
                icon.width: 20
                icon.height: 20
                onClicked: sidebarCollapsed = !sidebarCollapsed

                ToolTip.visible: hovered
                ToolTip.text: sidebarCollapsed ? "Expandir menú" : "Colapsar menú"
            }

            // Logo y título
            Row {
                spacing: 8

                Label {
                    text: "🛒"
                    font.pixelSize: 20
                    anchors.verticalCenter: parent.verticalCenter
                }

                Label {
                    text: "Vento"
                    font.pixelSize: 18
                    font.weight: Font.DemiBold
                    color: Theme.primary
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            // Espaciador
            Item { Layout.fillWidth: true }

            // Barra de búsqueda
            TextField {
                id: searchField
                Layout.preferredWidth: 300
                placeholderText: "🔍 Buscar productos, ventas..."

                background: Rectangle {
                    radius: 8
                    color: Theme.background
                    border.color: searchField.activeFocus ?
                                  Theme.primary : "transparent"
                    border.width: 2
                }
            }

            // Espaciador
            Item { Layout.fillWidth: true }

            // Indicador de tasa de cambio
            Row {
                spacing: 8

                Label {
                    text: "💵 USD:"
                    color: Theme.textSecondary
                    anchors.verticalCenter: parent.verticalCenter
                }

                Label {
                    id: exchangeRateLabel
                    text: CurrencyService.localCurrencySymbol + " " + CurrencyService.exchangeRateFormatted
                    font.weight: Font.Bold
                    color: Theme.success
                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            // Botones de acción
            ToolButton {
                icon.source: "qrc:/icons/notifications.svg"
                icon.width: 20
                icon.height: 20

                ToolTip.visible: hovered
                ToolTip.text: "Notificaciones"
            }

            ToolButton {
                icon.source: "qrc:/icons/settings.svg"
                icon.width: 20
                icon.height: 20

                ToolTip.visible: hovered
                ToolTip.text: "Configuración"
            }
        }
    }

    //--------------------------------------------------------------------------
    // Contenido principal con sidebar
    //--------------------------------------------------------------------------
    RowLayout {
        anchors.fill: parent
        spacing: 0

        //----------------------------------------------------------------------
        // Sidebar de navegación
        //----------------------------------------------------------------------
        Rectangle {
            id: sidebar
            Layout.preferredWidth: sidebarCollapsed ? 60 : 240
            Layout.fillHeight: true
            color: Theme.sidebar

            Behavior on Layout.preferredWidth {
                NumberAnimation { duration: 200; easing.type: Easing.OutCubic }
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.topMargin: 16
                spacing: 4

                // Items de navegación
                Repeater {
                    model: [
                        { icon: "📊", text: "Dashboard", view: "dashboard" },
                        { icon: "💱", text: "Tipo de Cambio", view: "currency" },
                        { icon: "📦", text: "Inventario", view: "inventory" },
                        { icon: "🛒", text: "Ventas", view: "sales" },
                        { icon: "📈", text: "Reportes", view: "reports" }
                    ]

                    delegate: ItemDelegate {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 48

                        highlighted: currentView === modelData.view

                        contentItem: Row {
                            spacing: sidebarCollapsed ? 0 : 12
                            anchors.centerIn: parent

                            Label {
                                text: modelData.icon
                                font.pixelSize: 18
                                anchors.verticalCenter: parent.verticalCenter
                            }

                            Label {
                                text: modelData.text
                                color: "white"
                                font.pixelSize: 14
                                visible: !sidebarCollapsed
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        background: Rectangle {
                            color: highlighted ? Theme.primary : "transparent"
                            opacity: highlighted ? 1.0 : 0.0

                            Behavior on opacity {
                                NumberAnimation { duration: 150 }
                            }
                        }

                        onClicked: currentView = modelData.view
                    }
                }

                Item { Layout.fillHeight: true }

                // Footer del sidebar
                Label {
                    Layout.fillWidth: true
                    Layout.margins: 16
                    text: "Vento v1.0.0"
                    color: "white"
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    visible: !sidebarCollapsed
                }
            }
        }

        //----------------------------------------------------------------------
        // Área principal de contenido
        //----------------------------------------------------------------------
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Theme.background

            // Dashboard View
            Column {
                anchors.fill: parent
                anchors.margins: 24
                spacing: 24
                visible: currentView === "dashboard"

                Label {
                    text: "📊 Dashboard"
                    font.pixelSize: 28
                    font.weight: Font.Bold
                    color: Theme.text
                }

                // Stats cards
                Row {
                    spacing: 16

                    Repeater {
                        model: [
                            { title: "Ventas Hoy", value: "Bs. 2,450", icon: "💰", color: Theme.success },
                            { title: "Productos", value: "156", icon: "📦", color: Theme.primary },
                            { title: "Pedidos", value: "23", icon: "🛒", color: Theme.warning },
                            { title: "Ingresos Mes", value: "Bs. 45,200", icon: "📈", color: Theme.primaryDark }
                        ]

                        delegate: Rectangle {
                            width: 200
                            height: 120
                            radius: 12
                            color: Theme.surface
                            border.color: "#E0E0E0"
                            border.width: 1

                            Column {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 8

                                Label {
                                    text: modelData.icon
                                    font.pixelSize: 32
                                }

                                Label {
                                    text: modelData.title
                                    font.pixelSize: 12
                                    color: Theme.textSecondary
                                }

                                Label {
                                    text: modelData.value
                                    font.pixelSize: 20
                                    font.weight: Font.Bold
                                    color: modelData.color
                                }
                            }
                        }
                    }
                }

                // Placeholder para más contenido
                Rectangle {
                    width: parent.width
                    height: 300
                    radius: 12
                    color: appColors.surface
                    border.color: "#E0E0E0"
                    border.width: 1

                    Label {
                        anchors.centerIn: parent
                        text: "📊 Gráficos y reportes próximamente"
                        color: Theme.textSecondary
                    }
                }
            }

            // Currency View
            CurrencyView {
                anchors.fill: parent
                anchors.margins: 24
                visible: currentView === "currency"
            }

            // Inventory View
            InventoryView {
                anchors.fill: parent
                anchors.margins: 24
                visible: currentView === "inventory"
            }

            // Sales View
            SalesView {
                anchors.fill: parent
                anchors.margins: 24
                visible: currentView === "sales"
            }

            // Reports View
            ReportsView {
                anchors.fill: parent
                anchors.margins: 24
                visible: currentView === "reports"
            }

            // Placeholder para otras vistas
            Rectangle {
                anchors.fill: parent
                anchors.margins: 24
                radius: 12
                color: appColors.surface
                border.color: "#E0E0E0"
                border.width: 1
                visible: currentView !== "dashboard" && currentView !== "currency" && currentView !== "inventory" && currentView !== "sales" && currentView !== "reports"

                Label {
                    anchors.centerIn: parent
                    text: "🚧 Vista '" + currentView + "' en desarrollo"
                    color: Theme.textSecondary
                    font.pixelSize: 16
                }
            }
        }
    }
}
