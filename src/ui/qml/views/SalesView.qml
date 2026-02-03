//==================================================================
// SalesView.qml
// Vento - Vista de gestión de ventas
//==================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vento.UI
import Vento.Sales

ColumnLayout {
    id: salesView
    
    spacing: Theme.spacingLg
    anchors.fill: parent
    anchors.margins: Theme.spacingXl
    
    //==========================================================================
    // Encabezado
    //==========================================================================
    Row {
        Layout.fillWidth: true
        spacing: Theme.spacingLg
        
        Column {
            spacing: Theme.spacingXs
            
            Text {
                text: "🛒 Ventas"
                font.pixelSize: Theme.fontSizeDisplay
                font.weight: Theme.fontWeightBold
                color: Theme.textPrimary
            }
            
            Text {
                text: "Gestiona tus ventas y procesa pedidos"
                font.pixelSize: Theme.fontSizeMd
                color: Theme.textSecondary
            }
        }
        
        Item { Layout.fillWidth: true }
        
        Button {
            text: "➕ Nueva Venta"
            
            background: Rectangle {
                color: parent.hovered ? Theme.primaryDark : Theme.primary
                radius: Theme.radiusMd
            }
            
            contentItem: Text {
                text: parent.text
                color: Theme.textOnPrimary
                font.pixelSize: Theme.fontSizeMd
                font.weight: Theme.fontWeightMedium
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            
            onClicked: newSaleDialog.open()
        }
    }
    
    //==========================================================================
    // Barra de herramientas
    //==========================================================================
    Row {
        Layout.fillWidth: true
        spacing: Theme.spacingLg
        
        SearchBar {
            Layout.preferredWidth: 300
            placeholderText: "Buscar ventas..."
        }
        
        Item { Layout.fillWidth: true }
        
        ComboBox {
            model: ["Todas", "Hoy", "Esta semana", "Este mes"]
            currentIndex: 0
            
            background: Rectangle {
                color: Theme.surface
                border.color: Theme.border
                border.width: 1
                radius: Theme.radiusMd
            }
        }
        
        Button {
            text: "📥 Exportar"
            flat: true
            
            background: Rectangle {
                color: parent.hovered ? Theme.primaryContainer : "transparent"
                radius: Theme.radiusMd
            }
            
            contentItem: Text {
                text: parent.text
                color: Theme.primary
                font.pixelSize: Theme.fontSizeMd
            }
        }
    }
    
    //==========================================================================
    // Estadísticas rápidas
    //==========================================================================
    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 80
        
        color: Theme.primaryContainer
        border.color: Theme.primary
        border.width: 1
        radius: Theme.radiusLg
        
        Row {
            anchors.fill: parent
            anchors.leftMargin: Theme.spacingLg
            anchors.rightMargin: Theme.spacingLg
            
            Repeater {
                model: [
                    { label: "Ventas Hoy", value: "Bs. 2,450", icon: "💰" },
                    { label: "Pedidos", value: "23", icon: "🛒" },
                    { label: "Promedio", value: "Bs. 106.52", icon: "📊" },
                    { label: "Canceladas", value: "2", icon: "❌" }
                ]
                
                delegate: Item {
                    width: (parent.width - Theme.spacingLg * 3) / 4
                    height: parent.height
                    
                    Row {
                        anchors.centerIn: parent
                        spacing: Theme.spacingSm
                        
                        Text {
                            text: model.icon
                            font.pixelSize: Theme.fontSizeXl
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        
                        Column {
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 2
                            
                            Text {
                                text: model.value
                                font.pixelSize: Theme.fontSizeLg
                                font.weight: Theme.fontWeightBold
                                color: Theme.primaryDark
                            }
                            
                            Text {
                                text: model.label
                                font.pixelSize: Theme.fontSizeXs
                                color: Theme.textSecondary
                            }
                        }
                    }
                }
            }
        }
    }
    
    //==========================================================================
    // Tabla de ventas
    //==========================================================================
    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true
        
        color: Theme.surface
        border.color: Theme.cardBorder
        border.width: 1
        radius: Theme.radiusLg
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 0
            
            // Encabezado de tabla
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 48
                
                color: Theme.background
                border.color: Theme.border
                border.width: 1
                
                Row {
                    anchors.fill: parent
                    anchors.leftMargin: Theme.spacingLg
                    anchors.rightMargin: Theme.spacingLg
                    
                    Text {
                        width: 80
                        text: "ID"
                        font.pixelSize: Theme.fontSizeSm
                        font.weight: Theme.fontWeightSemiBold
                        color: Theme.textPrimary
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    Text {
                        Layout.fillWidth: true
                        text: "Cliente"
                        font.pixelSize: Theme.fontSizeSm
                        font.weight: Theme.fontWeightSemiBold
                        color: Theme.textPrimary
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    Text {
                        width: 100
                        text: "Total"
                        font.pixelSize: Theme.fontSizeSm
                        font.weight: Theme.fontWeightSemiBold
                        color: Theme.textPrimary
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    Text {
                        width: 100
                        text: "Estado"
                        font.pixelSize: Theme.fontSizeSm
                        font.weight: Theme.fontWeightSemiBold
                        color: Theme.textPrimary
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    Text {
                        width: 120
                        text: "Fecha"
                        font.pixelSize: Theme.fontSizeSm
                        font.weight: Theme.fontWeightSemiBold
                        color: Theme.textPrimary
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    Text {
                        width: 80
                        text: "Acciones"
                        font.pixelSize: Theme.fontSizeSm
                        font.weight: Theme.fontWeightSemiBold
                        color: Theme.textPrimary
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
            
            // Lista de ventas
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                ListView {
                    id: salesList
                    
                    model: 15 // Número de ventas de ejemplo
                    
                    delegate: Rectangle {
                        width: salesList.width
                        height: 56
                        
                        color: index % 2 === 0 ? "transparent" : Theme.background
                        
                        Row {
                            anchors.fill: parent
                            anchors.leftMargin: Theme.spacingLg
                            anchors.rightMargin: Theme.spacingLg
                            
                            Text {
                                width: 80
                                text: "#" + (1000 + index)
                                font.pixelSize: Theme.fontSizeSm
                                color: Theme.textSecondary
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            
                            Text {
                                Layout.fillWidth: true
                                text: "Cliente " + (index + 1)
                                font.pixelSize: Theme.fontSizeSm
                                color: Theme.textPrimary
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            
                            Text {
                                width: 100
                                text: "Bs. " + (100 + index * 25).toFixed(2)
                                font.pixelSize: Theme.fontSizeSm
                                font.weight: Theme.fontWeightMedium
                                color: Theme.textPrimary
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            
                            Rectangle {
                                width: 100
                                height: 24
                                color: index % 3 === 0 ? Theme.successLight : 
                                       index % 3 === 1 ? Theme.warningLight : Theme.errorLight
                                radius: Theme.radiusSm
                                
                                Text {
                                    anchors.centerIn: parent
                                    text: index % 3 === 0 ? "Completada" : 
                                          index % 3 === 1 ? "Pendiente" : "Cancelada"
                                    font.pixelSize: Theme.fontSizeXs
                                    color: index % 3 === 0 ? Theme.successDark : 
                                          index % 3 === 1 ? Theme.warningDark : Theme.errorDark
                                    font.weight: Theme.fontWeightMedium
                                }
                            }
                            
                            Text {
                                width: 120
                                text: "2024-01-" + (10 + index)
                                font.pixelSize: Theme.fontSizeSm
                                color: Theme.textSecondary
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            
                            Row {
                                width: 80
                                spacing: Theme.spacingXs
                                anchors.verticalCenter: parent.verticalCenter
                                
                                Button {
                                    width: 28
                                    height: 28
                                    text: "👁️"
                                    flat: true
                                    
                                    background: Rectangle {
                                        color: parent.hovered ? Theme.primaryContainer : "transparent"
                                        radius: Theme.radiusSm
                                    }
                                    
                                    onClicked: viewSaleDetails(1000 + index)
                                }
                                
                                Button {
                                    width: 28
                                    height: 28
                                    text: "✏️"
                                    flat: true
                                    
                                    background: Rectangle {
                                        color: parent.hovered ? Theme.warningLight : "transparent"
                                        radius: Theme.radiusSm
                                    }
                                    
                                    onClicked: editSale(1000 + index)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    //==========================================================================
    // Diálogo de nueva venta
    //==========================================================================
    Dialog {
        id: newSaleDialog
        
        title: "Nueva Venta"
        width: 600
        height: 400
        
        background: Rectangle {
            color: Theme.surface
            radius: Theme.radiusLg
            border.color: Theme.border
            border.width: 1
        }
        
        ColumnLayout {
            anchors.fill: parent
            spacing: Theme.spacingLg
            
            Text {
                text: "Crear una nueva venta"
                font.pixelSize: Theme.fontSizeLg
                font.weight: Theme.fontWeightSemiBold
                color: Theme.textPrimary
            }
            
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: Theme.background
                radius: Theme.radiusMd
                
                Text {
                    anchors.centerIn: parent
                    text: "📝 Formulario de venta próximamente"
                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontSizeMd
                }
            }
            
            Row {
                Layout.alignment: Qt.AlignRight
                spacing: Theme.spacingMd
                
                Button {
                    text: "Cancelar"
                    flat: true
                    
                    background: Rectangle {
                        color: parent.hovered ? Theme.surfaceHover : "transparent"
                        radius: Theme.radiusMd
                    }
                    
                    onClicked: newSaleDialog.close()
                }
                
                Button {
                    text: "Crear Venta"
                    
                    background: Rectangle {
                        color: parent.hovered ? Theme.primaryDark : Theme.primary
                        radius: Theme.radiusMd
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        color: Theme.textOnPrimary
                        font.pixelSize: Theme.fontSizeMd
                        font.weight: Theme.fontWeightMedium
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    onClicked: {
                        // Lógica para crear venta
                        newSaleDialog.close()
                    }
                }
            }
        }
    }
    
    //==========================================================================
    // Funciones
    //==========================================================================
    function viewSaleDetails(saleId) {
        console.log("Ver detalles de venta:", saleId)
    }
    
    function editSale(saleId) {
        console.log("Editar venta:", saleId)
    }
}
