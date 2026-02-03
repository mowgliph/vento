//==================================================================
// StatCard.qml
// Vento - Tarjeta de estadística estilo Windows 11
//==================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vento.UI

Rectangle {
    id: statCard
    
    //==========================================================================
    // Propiedades públicas
    //==========================================================================
    property string title: "Título"
    property string value: "0"
    property string icon: "📊"
    property color iconColor: Theme.primary
    property color valueColor: Theme.textPrimary
    property bool loading: false
    
    //==========================================================================
    // Propiedades de diseño
    //==========================================================================
    color: Theme.surface
    border.color: Theme.cardBorder
    border.width: 1
    radius: Theme.radiusLg
    
    // Efecto de hover
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        
        onEntered: {
            statCard.color = Theme.cardHover
            statCard.scale = 1.02
        }
        
        onExited: {
            statCard.color = Theme.surface
            statCard.scale = 1.0
        }
    }
    
    Behavior on color {
        ColorAnimation { duration: Theme.animationFast }
    }
    
    Behavior on scale {
        NumberAnimation { 
            duration: Theme.animationFast
            easing.type: Easing.OutCubic
        }
    }
    
    //==========================================================================
    // Contenido principal
    //==========================================================================
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd
        
        //======================================================================
        // Icono y título
        //======================================================================
        Row {
            Layout.fillWidth: true
            spacing: Theme.spacingSm
            
            Text {
                text: icon
                font.pixelSize: Theme.fontSizeXxl
                color: iconColor
                anchors.verticalCenter: parent.verticalCenter
            }
            
            Text {
                text: title
                font.pixelSize: Theme.fontSizeSm
                color: Theme.textSecondary
                font.weight: Theme.fontWeightMedium
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        
        //======================================================================
        // Espaciador
        //======================================================================
        Item { Layout.fillHeight: true }
        
        //======================================================================
        // Valor
        //======================================================================
        Text {
            Layout.fillWidth: true
            text: loading ? "..." : value
            font.pixelSize: Theme.fontSizeXxl
            font.weight: Theme.fontWeightBold
            color: valueColor
            
            // Animación de carga
            SequentialAnimation on opacity {
                running: loading
                loops: Animation.Infinite
                NumberAnimation { to: 0.3; duration: 500 }
                NumberAnimation { to: 1.0; duration: 500 }
            }
        }
    }
    
    //==========================================================================
    // Indicador de progreso (opcional)
    //==========================================================================
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        height: 3
        color: Theme.alpha(iconColor, 0.2)
        
        Rectangle {
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            width: parent.width * 0.75 // 75% de progreso
            color: iconColor
            
            Behavior on width {
                NumberAnimation { duration: Theme.animationSlow }
            }
        }
    }
}
