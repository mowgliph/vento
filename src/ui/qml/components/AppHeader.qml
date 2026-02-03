//==================================================================
// AppHeader.qml
// Vento - Encabezado principal estilo Windows 11
//==================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vento.UI

Rectangle {
    id: header
    
    //==========================================================================
    // Propiedades públicas
    //==========================================================================
    property bool sidebarCollapsed: false
    property alias title: titleLabel.text
    property alias searchText: searchField.text
    
    signal toggleSidebar()
    signal searchChanged(string text)
    signal notificationsClicked()
    signal settingsClicked()
    
    //==========================================================================
    // Propiedades de diseño
    //==========================================================================
    color: Theme.surface
    height: Theme.headerHeight
    z: 100
    
    //==========================================================================
    // Sombra sutil
    //==========================================================================
    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 1
        color: Theme.border
    }
    
    //==========================================================================
    // Contenido principal
    //==========================================================================
    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: Theme.spacingLg
        anchors.rightMargin: Theme.spacingLg
        spacing: Theme.spacingLg
        
        //======================================================================
        // Botón de menú hamburguesa
        //======================================================================
        Button {
            id: menuButton
            
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            
            flat: true
            
            // Icono de hamburguesa usando SVG paths
            contentItem: Item {
                Row {
                    anchors.centerIn: parent
                    spacing: 4
                    
                    Rectangle {
                        width: 18
                        height: 2
                        color: Theme.textPrimary
                        radius: 1
                    }
                    Rectangle {
                        width: 18
                        height: 2
                        color: Theme.textPrimary
                        radius: 1
                    }
                    Rectangle {
                        width: 18
                        height: 2
                        color: Theme.textPrimary
                        radius: 1
                    }
                }
            }
            
            background: Rectangle {
                color: menuButton.hovered ? Theme.surfaceHover : "transparent"
                radius: Theme.radiusMd
            }
            
            onClicked: toggleSidebar()
            
            ToolTip.visible: hovered
            ToolTip.text: sidebarCollapsed ? "Expandir menú" : "Colapsar menú"
        }
        
        //======================================================================
        // Logo y título
        //======================================================================
        Row {
            spacing: Theme.spacingSm
            
            Text {
                text: "🛒"
                font.pixelSize: Theme.fontSizeXl
                anchors.verticalCenter: parent.verticalCenter
            }
            
            Text {
                id: titleLabel
                text: "Vento"
                font.pixelSize: Theme.fontSizeXl
                font.weight: Theme.fontWeightSemiBold
                color: Theme.primary
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        
        //======================================================================
        // Espaciador
        //======================================================================
        Item { Layout.fillWidth: true }
        
        //======================================================================
        // Barra de búsqueda
        //======================================================================
        Rectangle {
            Layout.preferredWidth: 300
            Layout.preferredHeight: 40
            Layout.maximumWidth: 400
            
            color: Theme.background
            border.color: searchField.activeFocus ? Theme.borderFocus : Theme.border
            border.width: searchField.activeFocus ? 2 : 1
            radius: Theme.radiusMd
            
            Behavior on border.color {
                ColorAnimation { duration: Theme.animationFast }
            }
            
            Row {
                anchors.fill: parent
                anchors.leftMargin: Theme.spacingMd
                anchors.rightMargin: Theme.spacingMd
                spacing: Theme.spacingSm
                
                Text {
                    text: "🔍"
                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontSizeMd
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                TextField {
                    id: searchField
                    
                    Layout.fillWidth: true
                    anchors.verticalCenter: parent.verticalCenter
                    
                    placeholderText: "Buscar productos, ventas..."
                    placeholderTextColor: Theme.textSecondary
                    
                    background: Rectangle {
                        color: "transparent"
                        border.width: 0
                    }
                    
                    onTextChanged: searchChanged(text)
                }
            }
        }
        
        //======================================================================
        // Espaciador
        //======================================================================
        Item { Layout.fillWidth: true }
        
        //======================================================================
        // Indicador de tasa de cambio
        //======================================================================
        Rectangle {
            Layout.preferredHeight: 32
            Layout.preferredWidth: implicitWidth
            Layout.rightMargin: Theme.spacingMd
            
            color: Theme.successLight
            border.color: Theme.success
            border.width: 1
            radius: Theme.radiusSm
            
            implicitWidth: changeRow.implicitWidth + Theme.spacingMd
            
            Row {
                id: changeRow
                
                anchors.centerIn: parent
                anchors.leftMargin: Theme.spacingSm
                anchors.rightMargin: Theme.spacingSm
                spacing: Theme.spacingXs
                
                Text {
                    text: "💵"
                    font.pixelSize: Theme.fontSizeSm
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                Text {
                    text: "USD:"
                    color: Theme.successDark
                    font.pixelSize: Theme.fontSizeSm
                    font.weight: Theme.fontWeightMedium
                    anchors.verticalCenter: parent.verticalCenter
                }
                
                Text {
                    id: exchangeRateLabel
                    text: "36.50"  // Se conectará al servicio
                    color: Theme.successDark
                    font.pixelSize: Theme.fontSizeSm
                    font.weight: Theme.fontWeightBold
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
        
        //======================================================================
        // Botones de acción
        //======================================================================
        Button {
            id: notificationsButton
            
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            
            flat: true
            
            contentItem: Text {
                text: "🔔"
                font.pixelSize: Theme.fontSizeLg
                anchors.centerIn: parent
            }
            
            background: Rectangle {
                color: notificationsButton.hovered ? Theme.surfaceHover : "transparent"
                radius: Theme.radiusMd
            }
            
            onClicked: notificationsClicked()
            
            ToolTip.visible: hovered
            ToolTip.text: "Notificaciones"
            
            // Indicador de notificaciones no leídas
            Rectangle {
                width: 8
                height: 8
                radius: Theme.radiusFull
                color: Theme.error
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.topMargin: 8
                anchors.rightMargin: 8
                visible: false // Se activará cuando haya notificaciones
            }
        }
        
        Button {
            id: settingsButton
            
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40
            
            flat: true
            
            contentItem: Text {
                text: "⚙️"
                font.pixelSize: Theme.fontSizeLg
                anchors.centerIn: parent
            }
            
            background: Rectangle {
                color: settingsButton.hovered ? Theme.surfaceHover : "transparent"
                radius: Theme.radiusMd
            }
            
            onClicked: settingsClicked()
            
            ToolTip.visible: hovered
            ToolTip.text: "Configuración"
        }
    }
}
