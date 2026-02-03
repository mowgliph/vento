//==================================================================
// SearchBar.qml
// Vento - Barra de búsqueda moderna
//==================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vento.UI

Rectangle {
    id: searchBar
    
    //==========================================================================
    // Propiedades públicas
    //==========================================================================
    property alias placeholderText: searchField.placeholderText
    property alias text: searchField.text
    property alias activeFocus: searchField.activeFocus
    
    signal searchChanged(string text)
    signal searchAccepted()
    signal clearClicked()
    
    //==========================================================================
    // Propiedades de diseño
    //==========================================================================
    color: Theme.surface
    border.color: searchField.activeFocus ? Theme.borderFocus : Theme.border
    border.width: searchField.activeFocus ? 2 : 1
    radius: Theme.radiusLg
    height: 48
    
    Behavior on border.color {
        ColorAnimation { duration: Theme.animationFast }
    }
    
    //==========================================================================
    // Contenido principal
    //==========================================================================
    Row {
        anchors.fill: parent
        anchors.leftMargin: Theme.spacingMd
        anchors.rightMargin: Theme.spacingMd
        spacing: Theme.spacingSm
        
        //======================================================================
        // Icono de búsqueda
        //======================================================================
        Text {
            text: "🔍"
            font.pixelSize: Theme.fontSizeLg
            color: Theme.textSecondary
            anchors.verticalCenter: parent.verticalCenter
        }
        
        //======================================================================
        // Campo de texto
        //======================================================================
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
            onAccepted: searchAccepted()
        }
        
        //======================================================================
        // Botón de limpiar
        //======================================================================
        Button {
            id: clearButton
            
            visible: searchField.text.length > 0
            width: 32
            height: 32
            
            flat: true
            
            contentItem: Text {
                text: "✕"
                color: Theme.textSecondary
                font.pixelSize: Theme.fontSizeMd
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            
            background: Rectangle {
                color: clearButton.hovered ? Theme.surfaceHover : "transparent"
                radius: Theme.radiusFull
            }
            
            onClicked: {
                searchField.text = ""
                searchField.forceActiveFocus()
                clearClicked()
            }
        }
    }
}
