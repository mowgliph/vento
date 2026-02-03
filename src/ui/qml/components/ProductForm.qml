//==============================================================================
// ProductForm.qml
// Vento - Formulario de creación/edición de productos
//==============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Vento.Inventory
import Vento.Currency
import "../theme/Theme.qml" as Theme
import "ValidatedInput.qml"

Dialog {
    id: productFormDialog
    
    property bool editingProduct: false
    property var currentProduct: null
    
    title: editingProduct ? "Editar Producto" : "Nuevo Producto"
    width: 650
    height: 700
    
    background: Rectangle {
        color: Theme.surface
        radius: Theme.radiusLg
        border.color: Theme.border
        border.width: 1
    }
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Theme.spacingLg
        spacing: Theme.spacingMd
        
        //======================================================================
        // Información Básica
        //======================================================================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            color: Theme.background
            radius: Theme.radiusMd
            border.color: Theme.border
            border.width: 1
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingMd
                spacing: Theme.spacingSm
                
                Text {
                    text: "📝 Información Básica"
                    font.pixelSize: Theme.fontSizeMd
                    font.weight: Theme.fontWeightSemiBold
                    color: Theme.textPrimary
                }
                
                ValidatedInput {
                    id: nameField
                    Layout.fillWidth: true
                    placeholderText: "Nombre del producto *"
                    required: true
                    maxLength: 100
                }
                
                ValidatedInput {
                    id: descriptionField
                    Layout.fillWidth: true
                    Layout.preferredHeight: 60
                    placeholderText: "Descripción del producto"
                    maxLength: 500
                    inputType: "text"
                }
                
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingSm
                    
                    ValidatedInput {
                        id: skuField
                        Layout.preferredWidth: 150
                        placeholderText: "SKU"
                        maxLength: 50
                    }
                    
                    ValidatedInput {
                        id: barcodeField
                        Layout.preferredWidth: 150
                        placeholderText: "Código de barras"
                        maxLength: 50
                    }
                    
                    Item { Layout.fillWidth: true }
                    
                    ValidatedInput {
                        id: categoryField
                        Layout.preferredWidth: 150
                        placeholderText: "Categoría"
                        maxLength: 50
                    }
                }
            }
        }
        
        //======================================================================
        // Precios y Costos
        //======================================================================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
            color: Theme.background
            radius: Theme.radiusMd
            border.color: Theme.border
            border.width: 1
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingMd
                spacing: Theme.spacingSm
                
                Text {
                    text: "💰 Precios y Costos"
                    font.pixelSize: Theme.fontSizeMd
                    font.weight: Theme.fontWeightSemiBold
                    color: Theme.textPrimary
                }
                
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingSm
                    
                    Column {
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Costo USD:"
                            font.pixelSize: Theme.fontSizeXs
                            color: Theme.textSecondary
                        }
                        
                        ValidatedInput {
                            id: costUsdField
                            width: 120
                            text: "0.00"
                            inputType: "number"
                            minValue: 0
                            maxValue: 999999.99
                            
                            onTextChanged: {
                                if (text.length > 0) {
                                    var cost = parseFloat(text) || 0
                                    var margin = parseFloat(marginField.text) || 0
                                    var exchangeRate = CurrencyService.exchangeRate
                                    var salePrice = (cost * exchangeRate) * (1 + margin/100)
                                    calculatedSalePriceField.text = salePrice.toFixed(2)
                                }
                            }
                        }
                    }
                    
                    Column {
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Margen (%):"
                            font.pixelSize: Theme.fontSizeXs
                            color: Theme.textSecondary
                        }
                        
                        ValidatedInput {
                            id: marginField
                            width: 80
                            text: "30"
                            inputType: "number"
                            minValue: 0
                            maxValue: 1000
                            
                            onTextChanged: {
                                if (text.length > 0) {
                                    var cost = parseFloat(costUsdField.text) || 0
                                    var margin = parseFloat(text) || 0
                                    var exchangeRate = CurrencyService.exchangeRate
                                    var salePrice = (cost * exchangeRate) * (1 + margin/100)
                                    calculatedSalePriceField.text = salePrice.toFixed(2)
                                }
                            }
                        }
                    }
                    
                    Column {
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Precio Venta:"
                            font.pixelSize: Theme.fontSizeXs
                            color: Theme.textSecondary
                        }
                        
                        ValidatedInput {
                            id: calculatedSalePriceField
                            width: 120
                            text: "0.00"
                            enabled: false
                            inputType: "number"
                        }
                    }
                    
                    Item { Layout.fillWidth: true }
                    
                    Column {
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Tasa actual:"
                            font.pixelSize: Theme.fontSizeXs
                            color: Theme.textSecondary
                        }
                        
                        Text {
                            text: CurrencyService.localCurrencySymbol + " " + CurrencyService.exchangeRateFormatted
                            font.pixelSize: Theme.fontSizeSm
                            font.weight: Theme.fontWeightSemiBold
                            color: Theme.primary
                        }
                    }
                }
                
                CheckBox {
                    id: manualPriceCheck
                    text: "Establecer precio de venta manualmente"
                    
                    onCheckedChanged: {
                        manualSalePriceField.enabled = checked
                        if (!checked) {
                            manualSalePriceField.text = calculatedSalePriceField.text
                        }
                    }
                }
                
                ValidatedInput {
                    id: manualSalePriceField
                    Layout.fillWidth: true
                    placeholderText: "Precio de venta manual"
                    enabled: false
                    inputType: "number"
                    minValue: 0
                    maxValue: 999999.99
                }
            }
        }
        
        //======================================================================
        // Inventario
        //======================================================================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            color: Theme.background
            radius: Theme.radiusMd
            border.color: Theme.border
            border.width: 1
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingMd
                spacing: Theme.spacingSm
                
                Text {
                    text: "📦 Inventario"
                    font.pixelSize: Theme.fontSizeMd
                    font.weight: Theme.fontWeightSemiBold
                    color: Theme.textPrimary
                }
                
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingSm
                    
                    Column {
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Stock inicial:"
                            font.pixelSize: Theme.fontSizeXs
                            color: Theme.textSecondary
                        }
                        
                        ValidatedInput {
                            id: stockField
                            width: 80
                            text: "0"
                            inputType: "integer"
                            minValue: 0
                            maxValue: 999999
                        }
                    }
                    
                    Column {
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Stock mínimo alerta:"
                            font.pixelSize: Theme.fontSizeXs
                            color: Theme.textSecondary
                        }
                        
                        ValidatedInput {
                            id: minStockField
                            width: 80
                            text: "5"
                            inputType: "integer"
                            minValue: 0
                            maxValue: 999999
                        }
                    }
                    
                    Item { Layout.fillWidth: true }
                    
                    CheckBox {
                        id: activeCheck
                        text: "Producto activo"
                        checked: true
                    }
                }
            }
        }
        
        //======================================================================
        // Botones de acción
        //======================================================================
        Row {
            Layout.alignment: Qt.AlignRight
            spacing: Theme.spacingSm
            
            Button {
                text: "Cancelar"
                
                background: Rectangle {
                    color: parent.hovered ? Theme.surfaceHover : "transparent"
                    radius: Theme.radiusMd
                }
                
                contentItem: Text {
                    text: parent.text
                    color: Theme.textPrimary
                    font.pixelSize: Theme.fontSizeMd
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                
                onClicked: productFormDialog.close()
            }
            
            Button {
                text: editingProduct ? "Actualizar" : "Guardar"
                
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
                
                onClicked: saveProduct()
            }
        }
    }
    
    //==========================================================================
    // Funciones
    //==========================================================================
    function saveProduct() {
        // Validar campos requeridos
        if (!nameField.isValidInput()) {
            return
        }
        
        var productData = {
            name: nameField.text,
            description: descriptionField.text,
            sku: skuField.text,
            barcode: barcodeField.text,
            category: categoryField.text,
            costUsd: parseFloat(costUsdField.text) || 0,
            marginPercent: parseFloat(marginField.text) || 0,
            salePrice: manualPriceCheck.enabled ? 
                       parseFloat(manualSalePriceField.text) || 0 :
                       parseFloat(calculatedSalePriceField.text) || 0,
            stockQuantity: parseInt(stockField.text) || 0,
            minStockAlert: parseInt(minStockField.text) || 5,
            active: activeCheck.checked
        }
        
        if (editingProduct && currentProduct) {
            var result = InventoryService.updateProduct(currentProduct.id, productData)
            if (result.success) {
                productFormDialog.close()
            }
        } else {
            var result = InventoryService.createProduct(productData)
            if (result.success) {
                productFormDialog.close()
                clearForm()
            }
        }
    }
    
    function clearForm() {
        nameField.clear()
        descriptionField.clear()
        skuField.clear()
        barcodeField.clear()
        categoryField.clear()
        costUsdField.text = "0.00"
        marginField.text = "30"
        calculatedSalePriceField.text = "0.00"
        manualSalePriceField.clear()
        manualPriceCheck.checked = false
        stockField.text = "0"
        minStockField.text = "5"
        activeCheck.checked = true
    }
    
    function loadProduct(product) {
        currentProduct = product
        editingProduct = true
        
        nameField.text = product.name || ""
        descriptionField.text = product.description || ""
        skuField.text = product.sku || ""
        barcodeField.text = product.barcode || ""
        categoryField.text = product.category || ""
        costUsdField.text = (product.costUsd || 0).toFixed(2)
        marginField.text = (product.marginPercent || 0).toFixed(2)
        calculatedSalePriceField.text = (product.salePrice || 0).toFixed(2)
        manualSalePriceField.text = (product.salePrice || 0).toFixed(2)
        stockField.text = (product.stockQuantity || 0).toString()
        minStockField.text = (product.minStockAlert || 5).toString()
        activeCheck.checked = product.active !== false
    }
    
    function openForNewProduct() {
        editingProduct = false
        currentProduct = null
        clearForm()
        open()
    }
    
    function openForEdit(product) {
        loadProduct(product)
        open()
    }
}
