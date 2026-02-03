//==================================================================
// Toast.qml
// Vento - Notificaciones emergentes estilo Windows 11
//==================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vento.UI

Rectangle {
    id: toast
    
    //==========================================================================
    // Propiedades públicas
    //==========================================================================
    property string message: "Mensaje"
    property string type: "info" // info, success, warning, error
    property int duration: 3000
    
    signal clicked()
    signal dismissed()
    
    //==========================================================================
    // Propiedades de diseño
    //==========================================================================
    width: Math.min(400, parent.width - Theme.spacingXxl)
    height: implicitHeight
    implicitHeight: contentColumn.implicitHeight + Theme.spacingLg
    
    // Colores según tipo
    color: {
        switch(type) {
            case "success": return Theme.success
            case "warning": return Theme.warning
            case "error": return Theme.error
            case "info":
            default: return Theme.info
        }
    }
    
    radius: Theme.radiusMd
    
    // Animación de entrada
    SequentialAnimation {
        id: showAnimation
        
        PropertyAnimation {
            target: toast
            property: "opacity"
            from: 0
            to: 1
            duration: Theme.animationNormal
            easing.type: Easing.OutCubic
        }
        
        PropertyAnimation {
            target: toast
            property: "y"
            from: -toast.height
            to: Theme.spacingLg
            duration: Theme.animationNormal
            easing.type: Easing.OutCubic
        }
    }
    
    // Animación de salida
    SequentialAnimation {
        id: hideAnimation
        
        PropertyAnimation {
            target: toast
            property: "opacity"
            from: 1
            to: 0
            duration: Theme.animationNormal
            easing.type: Easing.InCubic
        }
        
        PropertyAnimation {
            target: toast
            property: "y"
            from: toast.y
            to: -toast.height
            duration: Theme.animationNormal
            easing.type: Easing.InCubic
        }
        
        onFinished: dismissed()
    }
    
    // Temporizador para ocultar automáticamente
    Timer {
        id: autoHideTimer
        interval: duration
        onTriggered: hide()
    }
    
    //==========================================================================
    // Contenido principal
    //==========================================================================
    ColumnLayout {
        id: contentColumn
        
        anchors.fill: parent
        anchors.margins: Theme.spacingMd
        spacing: Theme.spacingXs
        
        //======================================================================
        // Fila de icono y mensaje
        //======================================================================
        Row {
            Layout.fillWidth: true
            spacing: Theme.spacingSm
            
            Text {
                text: {
                    switch(type) {
                        case "success": return "✅"
                        case "warning": return "⚠️"
                        case "error": return "❌"
                        case "info":
                        default: return "ℹ️"
                    }
                }
                font.pixelSize: Theme.fontSizeLg
                anchors.verticalCenter: parent.verticalCenter
            }
            
            Text {
                Layout.fillWidth: true
                text: message
                color: Theme.textOnPrimary
                font.pixelSize: Theme.fontSizeMd
                font.weight: Theme.fontWeightMedium
                wrapMode: Text.WordWrap
                anchors.verticalCenter: parent.verticalCenter
            }
            
            // Botón de cerrar
            Button {
                width: 24
                height: 24
                
                flat: true
                
                contentItem: Text {
                    text: "✕"
                    color: Theme.textOnPrimary
                    font.pixelSize: Theme.fontSizeSm
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                background: Rectangle {
                    color: parent.hovered ? Theme.alpha(Theme.textOnPrimary, 0.2) : "transparent"
                    radius: Theme.radiusFull
                }
                
                onClicked: hide()
            }
        }
    }
    
    //==========================================================================
    // Área de clic
    //==========================================================================
    MouseArea {
        anchors.fill: parent
        onClicked: {
            clicked()
            hide()
        }
    }
    
    //==========================================================================
    // Funciones públicas
    //==========================================================================
    function show() {
        opacity = 0
        y = -height
        visible = true
        showAnimation.start()
        autoHideTimer.restart()
    }
    
    function hide() {
        autoHideTimer.stop()
        hideAnimation.start()
    }
}
