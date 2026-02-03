//==============================================================================
// ProductList.qml
// Vento - Lista de productos con vista de cuadrícula y tabla
//==============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vento.Inventory
import Vento.Currency
import "../theme/Theme.qml" as Theme

ColumnLayout {
    id: productList
    
    property var products: []
    property string viewMode: "grid"  // "grid" or "list"
    property string searchText: ""
    property string selectedCategory: "Todas las categorías"
    
    signal productSelected(variant product)
    signal productEditRequested(variant product)
    signal productDeleteRequested(variant product)
    
    spacing: Theme.spacingMd
    
    //==========================================================================
    // Toggle de vista
    //==========================================================================
    Row {
        Layout.fillWidth: true
        spacing: Theme.spacingSm
        
        ButtonGroup {
            id: viewToggleGroup
        }
        
        Button {
            id: gridViewBtn
            text: "⊞"
            
            checkable: true
            checked: viewMode === "grid"
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
            
            onClicked: viewMode = "grid"
        }
        
        Button {
            id: listViewBtn
            text: "☰"
            
            checkable: true
            checked: viewMode === "list"
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
            
            onClicked: viewMode = "list"
        }
        
        Item { Layout.fillWidth: true }
        
        Text {
            text: filteredProducts.length + " productos"
            font.pixelSize: Theme.fontSizeSm
            color: Theme.textSecondary
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    
    //==========================================================================
    // Contenedor de productos
    //==========================================================================
    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true
        color: Theme.surface
        radius: Theme.radiusMd
        border.color: Theme.border
        border.width: 1
        
        //======================================================================
        // Vista de cuadrícula
        //======================================================================
        GridView {
            id: productGrid
            anchors.fill: parent
            anchors.margins: Theme.spacingSm
            
            visible: viewMode === "grid"
            
            cellWidth: 280
            cellHeight: 320
            
            model: filteredProducts
            
            delegate: ProductCard {
                width: productGrid.cellWidth - Theme.spacingMd
                height: productGrid.cellHeight - Theme.spacingMd
                
                product: modelData
                
                onClicked: productList.productSelected(modelData)
                editClicked: productList.productEditRequested(modelData)
                deleteClicked: productList.productDeleteRequested(modelData)
            }
        }
        
        //======================================================================
        // Vista de lista
        //======================================================================
        ListView {
            id: productListView
            anchors.fill: parent
            visible: viewMode === "list"
            
            model: filteredProducts
            
            delegate: Rectangle {
                width: productListView.width
                height: 80
                
                color: index % 2 === 0 ? "transparent" : Theme.background
                
                Row {
                    anchors.fill: parent
                    anchors.leftMargin: Theme.spacingLg
                    anchors.rightMargin: Theme.spacingLg
                    spacing: Theme.spacingLg
                    
                    // Icono
                    Rectangle {
                        width: 60
                        height: 60
                        color: Theme.background
                        radius: Theme.radiusMd
                        border.color: Theme.border
                        border.width: 1
                        
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
                            text: modelData.name || "Producto sin nombre"
                            font.pixelSize: Theme.fontSizeMd
                            font.weight: Theme.fontWeightSemiBold
                            color: Theme.textPrimary
                        }
                        
                        Text {
                            text: (modelData.description || "").substring(0, 50) + 
                                 ((modelData.description || "").length > 50 ? "..." : "")
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }
                    }
                    
                    // Precio
                    Text {
                        text: CurrencyService.localCurrencySymbol + " " + 
                             (modelData.salePrice || 0).toFixed(2)
                        font.pixelSize: Theme.fontSizeLg
                        font.weight: Theme.fontWeightBold
                        color: Theme.primary
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    
                    // Stock
                    Rectangle {
                        width: 80
                        height: 28
                        color: modelData.stockQuantity <= 5 ? 
                               Theme.warningLight : Theme.successLight
                        radius: Theme.radiusSm
                        
                        Text {
                            anchors.centerIn: parent
                            text: "Stock: " + (modelData.stockQuantity || 0)
                            font.pixelSize: Theme.fontSizeXs
                            color: modelData.stockQuantity <= 5 ? 
                                  Theme.warningDark : Theme.successDark
                            font.weight: Theme.fontWeightMedium
                        }
                    }
                    
                    // Acciones
                    Row {
                        spacing: Theme.spacingXs
                        anchors.verticalCenter: parent.verticalCenter
                        
                        Button {
                            text: "👁️"
                            flat: true
                            
                            background: Rectangle {
                                color: parent.hovered ? Theme.surfaceHover : "transparent"
                                radius: Theme.radiusSm
                            }
                            
                            onClicked: productList.productSelected(modelData)
                        }
                        
                        Button {
                            text: "✏️"
                            flat: true
                            
                            background: Rectangle {
                                color: parent.hovered ? Theme.warningLight : "transparent"
                                radius: Theme.radiusSm
                            }
                            
                            onClicked: productList.productEditRequested(modelData)
                        }
                        
                        Button {
                            text: "🗑️"
                            flat: true
                            
                            background: Rectangle {
                                color: parent.hovered ? Theme.errorLight : "transparent"
                                radius: Theme.radiusSm
                            }
                            
                            onClicked: productList.productDeleteRequested(modelData)
                        }
                    }
                }
            }
        }
        
        // Mensaje cuando no hay productos
        Label {
            anchors.centerIn: parent
            text: filteredProducts.length === 0 ? 
                  "📦 No hay productos para mostrar" : ""
            color: Theme.textSecondary
            font.pixelSize: Theme.fontSizeMd
            visible: filteredProducts.length === 0
        }
    }
    
    //==========================================================================
    // Propiedades computadas
    //==========================================================================
    property var filteredProducts: {
        var result = products
        
        // Filtrar por texto de búsqueda
        if (searchText.length > 0) {
            result = result.filter(function(product) {
                var searchLower = searchText.toLowerCase()
                return (product.name || "").toLowerCase().includes(searchLower) ||
                       (product.description || "").toLowerCase().includes(searchLower) ||
                       (product.sku || "").toLowerCase().includes(searchLower) ||
                       (product.barcode || "").toLowerCase().includes(searchLower)
            })
        }
        
        // Filtrar por categoría
        if (selectedCategory !== "Todas las categorías") {
            result = result.filter(function(product) {
                return product.category === selectedCategory
            })
        }
        
        return result
    }
    
    //==========================================================================
    // Componente ProductCard (usado en vista de cuadrícula)
    //==========================================================================
    component ProductCard: Rectangle {
        property variant product: ({})
        
        signal clicked()
        signal editClicked()
        signal deleteClicked()
        
        color: Theme.surface
        radius: Theme.radiusLg
        border.color: Theme.border
        border.width: 1
        
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: Theme.spacingMd
            spacing: Theme.spacingSm
            
            // Header
            Row {
                Layout.fillWidth: true
                
                Text {
                    text: "📦"
                    font.pixelSize: 24
                }
                
                Item { Layout.fillWidth: true }
                
                Rectangle {
                    width: 60
                    height: 24
                    radius: 12
                    color: product.stockQuantity <= 5 ? Theme.warning : 
                           product.stockQuantity <= 0 ? Theme.error : Theme.success
                    
                    Text {
                        anchors.centerIn: parent
                        text: product.stockQuantity <= 0 ? "Agotado" : 
                              product.stockQuantity <= 5 ? "Bajo" : "OK"
                        color: "white"
                        font.pixelSize: Theme.fontSizeXs
                        font.weight: Theme.fontWeightBold
                    }
                }
            }
            
            // Información del producto
            Column {
                Layout.fillWidth: true
                spacing: Theme.spacingXs
                
                Text {
                    text: product.name || "Producto sin nombre"
                    font.pixelSize: Theme.fontSizeSm
                    font.weight: Theme.fontWeightSemiBold
                    color: Theme.textPrimary
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                }
                
                Text {
                    text: product.description || ""
                    font.pixelSize: Theme.fontSizeXs
                    color: Theme.textSecondary
                    wrapMode: Text.WordWrap
                    Layout.fillWidth: true
                    visible: product.description !== ""
                }
                
                Text {
                    text: product.category || "Sin categoría"
                    font.pixelSize: Theme.fontSizeXs
                    color: Theme.textSecondary
                }
            }
            
            Item { Layout.fillHeight: true }
            
            // Precio y stock
            Column {
                Layout.fillWidth: true
                spacing: Theme.spacingXs
                
                Text {
                    text: CurrencyService.localCurrencySymbol + " " + 
                         (product.salePrice || 0).toFixed(2)
                    font.pixelSize: Theme.fontSizeLg
                    font.weight: Theme.fontWeightBold
                    color: Theme.primary
                }
                
                Text {
                    text: "Costo: $" + (product.costUsd || 0).toFixed(2) + 
                         " → " + CurrencyService.localCurrencySymbol + 
                         (product.costLocal || 0).toFixed(2)
                    font.pixelSize: Theme.fontSizeXs
                    color: Theme.textSecondary
                }
                
                Text {
                    text: "Stock: " + (product.stockQuantity || 0)
                    font.pixelSize: Theme.fontSizeSm
                    color: product.stockQuantity <= 5 ? Theme.warning : Theme.textPrimary
                }
            }
            
            // Acciones
            Row {
                Layout.fillWidth: true
                spacing: Theme.spacingSm
                
                Button {
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 32
                    text: "👁️"
                    flat: true
                    
                    background: Rectangle {
                        color: parent.hovered ? Theme.surfaceHover : "transparent"
                        radius: Theme.radiusSm
                    }
                    
                    onClicked: clicked()
                }
                
                Button {
                    Layout.preferredWidth: 60
                    Layout.preferredHeight: 32
                    text: "✏️"
                    flat: true
                    
                    background: Rectangle {
                        color: parent.hovered ? Theme.warningLight : "transparent"
                        radius: Theme.radiusSm
                    }
                    
                    onClicked: editClicked()
                }
                
                Button {
                    Layout.preferredWidth: 60
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
        
        MouseArea {
            anchors.fill: parent
            onClicked: clicked()
        }
    }
}
