//==================================================================
// InventoryView.qml
// Vento - Vista de gestión de inventario
//==================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vento.UI
import Vento.Inventory

ColumnLayout {
    id: inventoryView
    
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
                text: "📦 Inventario"
                font.pixelSize: Theme.fontSizeDisplay
                font.weight: Theme.fontWeightBold
                color: Theme.textPrimary
            }
            
            Text {
                text: "Gestiona tu catálogo de productos"
                font.pixelSize: Theme.fontSizeMd
                color: Theme.textSecondary
            }
        }
        
        Item { Layout.fillWidth: true }
        
        Button {
            text: "➕ Nuevo Producto"
            
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
            
            onClicked: newProductDialog.open()
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
            placeholderText: "Buscar productos..."
        }
        
        Item { Layout.fillWidth: true }
        
        ComboBox {
            model: ["Todas las categorías", "Electrónica", "Ropa", "Alimentos", "Otros"]
            currentIndex: 0
            
            background: Rectangle {
                color: Theme.surface
                border.color: Theme.border
                border.width: 1
                radius: Theme.radiusMd
            }
        }
        
        Button {
            text: "📥 Importar"
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
        
        Button {
            text: "📤 Exportar"
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
    // Vista toggle (Grid/List)
    //==========================================================================
    Row {
        Layout.fillWidth: true
        spacing: Theme.spacingMd
        
        ButtonGroup {
            id: viewToggleGroup
        }
        
        Button {
            id: gridViewBtn
            
            text: "⊞"
            checked: true
            autoExclusive: true
            
            background: Rectangle {
                color: gridViewBtn.checked ? Theme.primary : 
                       gridViewBtn.hovered ? Theme.surfaceHover : "transparent"
                radius: Theme.radiusSm
            }
            
            contentItem: Text {
                text: parent.text
                color: gridViewBtn.checked ? Theme.textOnPrimary : Theme.textPrimary
                font.pixelSize: Theme.fontSizeLg
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            
            ButtonGroup.group: viewToggleGroup
            onClicked: productView.currentIndex = 0
        }
        
        Button {
            id: listViewBtn
            
            text: "☰"
            autoExclusive: true
            
            background: Rectangle {
                color: listViewBtn.checked ? Theme.primary : 
                       listViewBtn.hovered ? Theme.surfaceHover : "transparent"
                radius: Theme.radiusSm
            }
            
            contentItem: Text {
                text: parent.text
                color: listViewBtn.checked ? Theme.textOnPrimary : Theme.textPrimary
                font.pixelSize: Theme.fontSizeLg
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            
            ButtonGroup.group: viewToggleGroup
            onClicked: productView.currentIndex = 1
        }
        
        Item { Layout.fillWidth: true }
        
        Text {
            text: productGrid.count + " productos"
            font.pixelSize: Theme.fontSizeSm
            color: Theme.textSecondary
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    
    //==========================================================================
    // Contenido principal (Grid/List view)
    //==========================================================================
    StackLayout {
        id: productView
        
        Layout.fillWidth: true
        Layout.fillHeight: true
        currentIndex: 0
        
        //======================================================================
        // Grid View
        //======================================================================
        ScrollView {
            GridView {
                id: productGrid
                
                cellWidth: 280
                cellHeight: 320
                
                model: 12 // Número de productos de ejemplo
                
                delegate: ProductCard {
                    width: productGrid.cellWidth - Theme.spacingMd
                    height: productGrid.cellHeight - Theme.spacingMd
                    
                    name: "Producto " + (index + 1)
                    description: "Descripción del producto " + (index + 1)
                    price: 25.50 + index * 5.25
                    stock: 50 - index * 2
                    category: index % 3 === 0 ? "Electrónica" : 
                             index % 3 === 1 ? "Ropa" : "Alimentos"
                    
                    onClicked: viewProduct(index)
                    editClicked: editProduct(index)
                    deleteClicked: deleteProduct(index)
                }
            }
        }
        
        //======================================================================
        // List View
        //======================================================================
        ScrollView {
            ListView {
                id: productList
                
                model: 12
                
                delegate: Rectangle {
                    width: productList.width
                    height: 80
                    
                    color: index % 2 === 0 ? "transparent" : Theme.background
                    
                    Row {
                        anchors.fill: parent
                        anchors.leftMargin: Theme.spacingLg
                        anchors.rightMargin: Theme.spacingLg
                        spacing: Theme.spacingLg
                        
                        // Imagen/Icono
                        Rectangle {
                            width: 60
                            height: 60
                            color: Theme.background
                            radius: Theme.radiusMd
                            
                            Text {
                                anchors.centerIn: parent
                                text: "📦"
                                font.pixelSize: 24
                            }
                        }
                        
                        // Información
                        Column {
                            Layout.fillWidth: true
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 2
                            
                            Text {
                                text: "Producto " + (index + 1)
                                font.pixelSize: Theme.fontSizeMd
                                font.weight: Theme.fontWeightSemiBold
                                color: Theme.textPrimary
                            }
                            
                            Text {
                                text: "Descripción del producto " + (index + 1)
                                font.pixelSize: Theme.fontSizeSm
                                color: Theme.textSecondary
                            }
                        }
                        
                        // Precio
                        Text {
                            text: "Bs. " + (25.50 + index * 5.25).toFixed(2)
                            font.pixelSize: Theme.fontSizeLg
                            font.weight: Theme.fontWeightBold
                            color: Theme.primary
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        
                        // Stock
                        Rectangle {
                            width: 80
                            height: 28
                            color: (50 - index * 2) < 10 ? Theme.warningLight : Theme.successLight
                            radius: Theme.radiusSm
                            
                            Text {
                                anchors.centerIn: parent
                                text: "Stock: " + (50 - index * 2)
                                font.pixelSize: Theme.fontSizeXs
                                color: (50 - index * 2) < 10 ? Theme.warningDark : Theme.successDark
                                font.weight: Theme.fontWeightMedium
                            }
                        }
                        
                        // Acciones
                        Row {
                            spacing: Theme.spacingXs
                            anchors.verticalCenter: parent.verticalCenter
                            
                            Button {
                                width: 32
                                height: 32
                                text: "👁️"
                                flat: true
                                
                                background: Rectangle {
                                    color: parent.hovered ? Theme.primaryContainer : "transparent"
                                    radius: Theme.radiusSm
                                }
                                
                                onClicked: viewProduct(index)
                            }
                            
                            Button {
                                width: 32
                                height: 32
                                text: "✏️"
                                flat: true
                                
                                background: Rectangle {
                                    color: parent.hovered ? Theme.warningLight : "transparent"
                                    radius: Theme.radiusSm
                                }
                                
                                onClicked: editProduct(index)
                            }
                            
                            Button {
                                width: 32
                                height: 32
                                text: "🗑️"
                                flat: true
                                
                                background: Rectangle {
                                    color: parent.hovered ? Theme.errorLight : "transparent"
                                    radius: Theme.radiusSm
                                }
                                
                                onClicked: deleteProduct(index)
                            }
                        }
                    }
                }
            }
        }
    }
    
    //==========================================================================
    // Diálogo de nuevo producto
    //==========================================================================
    Dialog {
        id: newProductDialog
        
        title: "Nuevo Producto"
        width: 600
        height: 500
        
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
                text: "Agregar un nuevo producto al inventario"
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
                    text: "📝 Formulario de producto próximamente"
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
                    
                    onClicked: newProductDialog.close()
                }
                
                Button {
                    text: "Guardar Producto"
                    
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
                        // Lógica para guardar producto
                        newProductDialog.close()
                    }
                }
            }
        }
    }
    
    //==========================================================================
    // Funciones
    //==========================================================================
    function viewProduct(index) {
        console.log("Ver producto:", index)
    }
    
    function editProduct(index) {
        console.log("Editar producto:", index)
    }
    
    function deleteProduct(index) {
        console.log("Eliminar producto:", index)
    }
}
