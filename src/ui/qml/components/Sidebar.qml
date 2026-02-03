//==================================================================
// Sidebar.qml
// Vento - Navegación lateral estilo Windows 11
//==================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vento.UI

Rectangle {
    id: sidebar
    
    //==========================================================================
    // Propiedades públicas
    //==========================================================================
    property bool collapsed: false
    property string currentView: "dashboard"
    
    signal viewChanged(string viewName)
    
    //==========================================================================
    // Propiedades de diseño
    //==========================================================================
    color: Theme.sidebar
    width: collapsed ? Theme.sidebarCollapsedWidth : Theme.sidebarWidth
    
    Behavior on width {
        NumberAnimation { 
            duration: Theme.animationNormal
            easing.type: Easing.OutCubic 
        }
    }
    
    //==========================================================================
    // Contenido principal
    //==========================================================================
    ColumnLayout {
        anchors.fill: parent
        anchors.topMargin: Theme.spacingLg
        spacing: Theme.spacingXs
        
        //======================================================================
        // Logo/Brand
        //======================================================================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 48
            Layout.leftMargin: Theme.spacingMd
            Layout.rightMargin: Theme.spacingMd
            color: "transparent"
            
            Row {
                anchors.centerIn: parent
                spacing: Theme.spacingSm
                
                Text {
                    text: "🛒"
                    font.pixelSize: Theme.fontSizeXl
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                Text {
                    text: "Vento"
                    font.pixelSize: Theme.fontSizeLg
                    font.weight: Theme.fontWeightSemiBold
                    color: Theme.textOnSidebar
                    visible: !collapsed
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
        
        //======================================================================
        // Separador
        //======================================================================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            Layout.leftMargin: Theme.spacingMd
            Layout.rightMargin: Theme.spacingMd
            color: Theme.alpha(Theme.textOnSidebar, 0.2)
        }
        
        //======================================================================
        // Items de navegación
        //======================================================================
        Repeater {
            model: [
                { 
                    icon: "📊", 
                    text: "Dashboard", 
                    view: "dashboard",
                    tooltip: "Panel principal"
                },
                { 
                    icon: "💱", 
                    text: "Tipo de Cambio", 
                    view: "currency",
                    tooltip: "Gestionar tasas de cambio"
                },
                { 
                    icon: "📦", 
                    text: "Inventario", 
                    view: "inventory",
                    tooltip: "Gestionar productos"
                },
                { 
                    icon: "🛒", 
                    text: "Ventas", 
                    view: "sales",
                    tooltip: "Procesar ventas"
                },
                { 
                    icon: "📈", 
                    text: "Reportes", 
                    view: "reports",
                    tooltip: "Ver reportes"
                },
                { 
                    icon: "⚙️", 
                    text: "Configuración", 
                    view: "settings",
                    tooltip: "Configuración del sistema"
                }
            ]
            
            delegate: ItemDelegate {
                id: navItem
                
                Layout.fillWidth: true
                Layout.preferredHeight: 48
                Layout.leftMargin: Theme.spacingSm
                Layout.rightMargin: Theme.spacingSm
                
                highlighted: currentView === modelData.view
                
                //==================================================================
                // Contenido del item
                //==================================================================
                contentItem: Row {
                    spacing: collapsed ? 0 : Theme.spacingMd
                    anchors.centerIn: parent
                    
                    Text {
                        text: modelData.icon
                        font.pixelSize: Theme.fontSizeLg
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    Text {
                        text: modelData.text
                        color: Theme.textOnSidebar
                        font.pixelSize: Theme.fontSizeMd
                        visible: !collapsed
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
                
                //==================================================================
                // Background con animación
                //==================================================================
                background: Rectangle {
                    color: highlighted ? Theme.sidebarActive : "transparent"
                    radius: Theme.radiusMd
                    
                    // Efecto de hover
                    Rectangle {
                        anchors.fill: parent
                        color: Theme.alpha(Theme.textOnSidebar, 0.1)
                        radius: Theme.radiusMd
                        visible: navItem.hovered && !highlighted
                    }
                    
                    Behavior on color {
                        ColorAnimation { 
                            duration: Theme.animationFast 
                        }
                    }
                }
                
                //==================================================================
                // Tooltip
                //==================================================================
                ToolTip.visible: collapsed && hovered
                ToolTip.text: modelData.tooltip
                ToolTip.delay: 500
                
                //==================================================================
                // Click handler
                //==================================================================
                onClicked: {
                    currentView = modelData.view
                    viewChanged(modelData.view)
                }
            }
        }
        
        //======================================================================
        // Espaciador flexible
        //======================================================================
        Item { Layout.fillHeight: true }
        
        //======================================================================
        // Footer
        //======================================================================
        ColumnLayout {
            Layout.fillWidth: true
            Layout.leftMargin: Theme.spacingMd
            Layout.rightMargin: Theme.spacingMd
            Layout.bottomMargin: Theme.spacingLg
            spacing: Theme.spacingXs
            
            // Separador
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: Theme.alpha(Theme.textOnSidebar, 0.2)
            }
            
            // Versión
            Text {
                Layout.fillWidth: true
                text: "Vento v1.0.0"
                color: Theme.alpha(Theme.textOnSidebar, 0.7)
                font.pixelSize: Theme.fontSizeXs
                horizontalAlignment: Text.AlignHCenter
                visible: !collapsed
            }
            
            // Estado del sistema
            Row {
                Layout.fillWidth: true
                Layout.preferredHeight: 20
                spacing: Theme.spacingXs
                visible: !collapsed
                
                Rectangle {
                    width: 8
                    height: 8
                    radius: Theme.radiusFull
                    color: Theme.success
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                Text {
                    text: "Conectado"
                    color: Theme.alpha(Theme.textOnSidebar, 0.7)
                    font.pixelSize: Theme.fontSizeXs
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }
    
    //==========================================================================
    // Borde derecho
    //==========================================================================
    Rectangle {
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: Theme.alpha(Theme.textOnSidebar, 0.1)
    }
}
