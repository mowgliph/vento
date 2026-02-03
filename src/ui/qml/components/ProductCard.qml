//==================================================================
// ProductCard.qml
// Vento - Tarjeta de producto para vista grid
//==================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vento.UI

Rectangle {
    id: productCard
    
    //==========================================================================
    // Propiedades públicas
    //==========================================================================
    property string productId: ""
    property string name: "Producto"
    property string description: "Descripción del producto"
    property double price: 0.0
    property int stock: 0
    property string category: "General"
    property string image: ""
    property bool lowStock: stock < 10
    
    signal clicked()
    signal editClicked()
    signal deleteClicked()
    
    //==========================================================================
    // Propiedades de diseño
    //==========================================================================
    color: Theme.surface
    border.color: lowStock ? Theme.warning : Theme.cardBorder
    border.width: lowStock ? 2 : 1
    radius: Theme.radiusLg
    
    // Efecto de hover
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        
        onEntered: {
            productCard.color = Theme.cardHover
            productCard.scale = 1.02
        }
        
        onExited: {
            productCard.color = Theme.surface
            productCard.scale = 1.0
        }
        
        onClicked: productCard.clicked()
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
        anchors.margins: Theme.spacingMd
        spacing: Theme.spacingSm
        
        //======================================================================
        // Imagen del producto
        //======================================================================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            color: Theme.background
            radius: Theme.radiusMd
            
            Text {
                anchors.centerIn: parent
                text: image ? "" : "📦"
                font.pixelSize: 48
                color: Theme.textSecondary
            }
            
            // Placeholder para imagen real
            Image {
                anchors.fill: parent
                anchors.margins: Theme.spacingXs
                source: image
                fillMode: Image.PreserveAspectFit
                visible: image !== ""
            }
        }
        
        //======================================================================
        // Información del producto
        //======================================================================
        ColumnLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingXs
            
            // Nombre
            Text {
                Layout.fillWidth: true
                text: name
                font.pixelSize: Theme.fontSizeMd
                font.weight: Theme.fontWeightSemiBold
                color: Theme.textPrimary
                elide: Text.ElideRight
                maximumLineCount: 2
                wrapMode: Text.WordWrap
            }
            
            // Descripción
            Text {
                Layout.fillWidth: true
                text: description
                font.pixelSize: Theme.fontSizeSm
                color: Theme.textSecondary
                elide: Text.ElideRight
                maximumLineCount: 2
                wrapMode: Text.WordWrap
                Layout.fillHeight: true
            }
            
            // Categoría
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 24
                color: Theme.primaryContainer
                radius: Theme.radiusSm
                
                Text {
                    anchors.centerIn: parent
                    text: category
                    font.pixelSize: Theme.fontSizeXs
                    color: Theme.primary
                    font.weight: Theme.fontWeightMedium
                }
            }
        }
        
        //======================================================================
        // Precio y stock
        //======================================================================
        Row {
            Layout.fillWidth: true
            spacing: Theme.spacingSm
            
            Text {
                text: "Bs. " + price.toFixed(2)
                font.pixelSize: Theme.fontSizeLg
                font.weight: Theme.fontWeightBold
                color: Theme.primary
            }
            
            Item { width: Theme.spacingSm }
            
            Rectangle {
                width: stockIndicator.implicitWidth + Theme.spacingSm
                height: 24
                color: lowStock ? Theme.warningLight : Theme.successLight
                radius: Theme.radiusSm
                
                Text {
                    id: stockIndicator
                    anchors.centerIn: parent
                    text: "Stock: " + stock
                    font.pixelSize: Theme.fontSizeXs
                    color: lowStock ? Theme.warningDark : Theme.successDark
                    font.weight: Theme.fontWeightMedium
                }
            }
        }
        
        //======================================================================
        // Botones de acción
        //======================================================================
        Row {
            Layout.fillWidth: true
            spacing: Theme.spacingXs
            
            Button {
                Layout.fillWidth: true
                text: "Editar"
                flat: true
                
                background: Rectangle {
                    color: parent.hovered ? Theme.primaryContainer : "transparent"
                    radius: Theme.radiusSm
                }
                
                contentItem: Text {
                    text: parent.text
                    color: Theme.primary
                    font.pixelSize: Theme.fontSizeSm
                    horizontalAlignment: Text.AlignHCenter
                }
                
                onClicked: editClicked()
            }
            
            Button {
                Layout.preferredWidth: 40
                Layout.preferredHeight: 32
                text: "🗑️"
                flat: true
                
                background: Rectangle {
                    color: parent.hovered ? Theme.errorLight : "transparent"
                    radius: Theme.radiusSm
                }
                
                onClicked: deleteClicked()
            }
        }
    }
}
