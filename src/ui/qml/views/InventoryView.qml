//==================================================================
// InventoryView.qml
// Vento - Vista de gestión de inventario
//==================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Vento.Inventory
import Vento.Currency

ColumnLayout {
    id: inventoryView
    
    property var appColors: ({
        primary: "#0078D4",
        primaryDark: "#005A9E", 
        secondary: "#6B6B6B",
        success: "#107C10",
        warning: "#FF8C00",
        danger: "#D13438",
        surface: "#FFFFFF",
        background: "#F3F3F3",
        text: "#1A1A1A",
        textSecondary: "#6B6B6B"
    })
    
    spacing: 16
    anchors.fill: parent
    anchors.margins: 24
    
    //==========================================================================
    // Encabezado
    //==========================================================================
    Row {
        Layout.fillWidth: true
        spacing: 16
        
        Column {
            spacing: 4
            
            Text {
                text: "📦 Inventario"
                font.pixelSize: 28
                font.weight: Font.Bold
                color: appColors.text
            }
            
            Text {
                text: "Gestiona tu catálogo de productos"
                font.pixelSize: 14
                color: appColors.textSecondary
            }
        }
        
        Item { Layout.fillWidth: true }
        
        Button {
            text: "➕ Nuevo Producto"
            
            background: Rectangle {
                color: parent.hovered ? appColors.primaryDark : appColors.primary
                radius: 8
            }
            
            contentItem: Text {
                text: parent.text
                color: "white"
                font.pixelSize: 14
                font.weight: Font.Medium
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
        spacing: 16
        
        TextField {
            id: searchField
            Layout.preferredWidth: 300
            placeholderText: "🔍 Buscar productos..."
            
            background: Rectangle {
                radius: 8
                color: appColors.surface
                border.color: searchField.activeFocus ? appColors.primary : "#E0E0E0"
                border.width: 2
            }
        }
        
        Item { Layout.fillWidth: true }
        
        ComboBox {
            id: categoryCombo
            model: ["Todas las categorías"] + InventoryService.getAllCategories()
            currentIndex: 0
            
            background: Rectangle {
                color: appColors.surface
                border.color: "#E0E0E0"
                border.width: 1
                radius: 8
            }
        }
        
        Button {
            text: "� Actualizar Precios"
            flat: true
            
            background: Rectangle {
                color: parent.hovered ? "#F0F0F0" : "transparent"
                radius: 8
            }
            
            contentItem: Text {
                text: parent.text
                color: appColors.primary
                font.pixelSize: 12
            }
            
            onClicked: {
                InventoryService.updatePricesWithExchangeRate(CurrencyService.exchangeRate)
            }
        }
    }
    
    //==========================================================================
    // Vista toggle (Grid/List)
    //==========================================================================
    Row {
        Layout.fillWidth: true
        spacing: 8
        
        ButtonGroup {
            id: viewToggleGroup
        }
        
        Button {
            id: gridViewBtn
            
            text: "⊞"
            checked: true
            autoExclusive: true
            
            background: Rectangle {
                color: gridViewBtn.checked ? appColors.primary : 
                       gridViewBtn.hovered ? "#F0F0F0" : "transparent"
                radius: 6
            }
            
            contentItem: Text {
                text: parent.text
                color: gridViewBtn.checked ? "white" : appColors.text
                font.pixelSize: 16
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
                color: listViewBtn.checked ? appColors.primary : 
                       listViewBtn.hovered ? "#F0F0F0" : "transparent"
                radius: 6
            }
            
            contentItem: Text {
                text: parent.text
                color: listViewBtn.checked ? "white" : appColors.text
                font.pixelSize: 16
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            
            ButtonGroup.group: viewToggleGroup
            onClicked: productView.currentIndex = 1
        }
        
        Item { Layout.fillWidth: true }
        
        Text {
            text: InventoryService.count + " productos"
            font.pixelSize: 12
            color: appColors.textSecondary
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
                
                model: InventoryService.getActiveProducts()
                
                delegate: Rectangle {
                    width: productGrid.cellWidth - 16
                    height: productGrid.cellHeight - 16
                    color: appColors.surface
                    radius: 12
                    border.color: "#E0E0E0"
                    border.width: 1
                    
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 12
                        
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
                                color: modelData.stockQuantity <= 5 ? appColors.warning : 
                                       modelData.stockQuantity <= 0 ? appColors.danger : appColors.success
                                
                                Text {
                                    anchors.centerIn: parent
                                    text: modelData.stockQuantity <= 0 ? "Agotado" : 
                                          modelData.stockQuantity <= 5 ? "Bajo" : "OK"
                                    color: "white"
                                    font.pixelSize: 10
                                    font.weight: Font.Bold
                                }
                            }
                        }
                        
                        // Product info
                        Column {
                            Layout.fillWidth: true
                            spacing: 4
                            
                            Text {
                                text: modelData.name
                                font.pixelSize: 14
                                font.weight: Font.Bold
                                color: appColors.text
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                            }
                            
                            Text {
                                text: modelData.description || ""
                                font.pixelSize: 12
                                color: appColors.textSecondary
                                wrapMode: Text.WordWrap
                                Layout.fillWidth: true
                                visible: modelData.description !== ""
                            }
                            
                            Text {
                                text: modelData.category || "Sin categoría"
                                font.pixelSize: 11
                                color: appColors.textSecondary
                            }
                        }
                        
                        Item { Layout.fillHeight: true }
                        
                        // Price and stock
                        Column {
                            Layout.fillWidth: true
                            spacing: 4
                            
                            Text {
                                text: CurrencyService.localCurrencySymbol + " " + modelData.salePrice.toFixed(2)
                                font.pixelSize: 18
                                font.weight: Font.Bold
                                color: appColors.primary
                            }
                            
                            Text {
                                text: "Costo: $" + modelData.costUsd.toFixed(2) + " → " + CurrencyService.localCurrencySymbol + modelData.costLocal.toFixed(2)
                                font.pixelSize: 10
                                color: appColors.textSecondary
                            }
                            
                            Text {
                                text: "Stock: " + modelData.stockQuantity
                                font.pixelSize: 12
                                color: modelData.stockQuantity <= 5 ? appColors.warning : appColors.text
                            }
                        }
                        
                        // Actions
                        Row {
                            Layout.fillWidth: true
                            spacing: 8
                            
                            Button {
                                Layout.preferredWidth: 60
                                Layout.preferredHeight: 32
                                text: "👁️"
                                flat: true
                                
                                background: Rectangle {
                                    color: parent.hovered ? "#F0F0F0" : "transparent"
                                    radius: 6
                                }
                                
                                onClicked: viewProduct(modelData.id)
                            }
                            
                            Button {
                                Layout.preferredWidth: 60
                                Layout.preferredHeight: 32
                                text: "✏️"
                                flat: true
                                
                                background: Rectangle {
                                    color: parent.hovered ? appColors.warning + "20" : "transparent"
                                    radius: 6
                                }
                                
                                onClicked: editProduct(modelData.id)
                            }
                            
                            Button {
                                Layout.preferredWidth: 60
                                Layout.preferredHeight: 32
                                text: "🗑️"
                                flat: true
                                
                                background: Rectangle {
                                    color: parent.hovered ? appColors.danger + "20" : "transparent"
                                    radius: 6
                                }
                                
                                onClicked: deleteProduct(modelData.id)
                            }
                        }
                    }
                }
            }
        }
        
        //======================================================================
        // List View
        //======================================================================
        ScrollView {
            ListView {
                id: productList
                
                model: InventoryService.getActiveProducts()
                
                delegate: Rectangle {
                    width: productList.width
                    height: 80
                    
                    color: index % 2 === 0 ? "transparent" : appColors.background
                    
                    Row {
                        anchors.fill: parent
                        anchors.leftMargin: 16
                        anchors.rightMargin: 16
                        spacing: 16
                        
                        // Icon
                        Rectangle {
                            width: 60
                            height: 60
                            color: appColors.surface
                            radius: 8
                            border.color: "#E0E0E0"
                            border.width: 1
                            
                            Text {
                                anchors.centerIn: parent
                                text: "📦"
                                font.pixelSize: 24
                            }
                        }
                        
                        // Info
                        Column {
                            Layout.fillWidth: true
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 2
                            
                            Text {
                                text: modelData.name
                                font.pixelSize: 14
                                font.weight: Font.Bold
                                color: appColors.text
                            }
                            
                            Text {
                                text: (modelData.description || "").substring(0, 50) + ((modelData.description || "").length > 50 ? "..." : "")
                                font.pixelSize: 12
                                color: appColors.textSecondary
                            }
                        }
                        
                        // Price
                        Text {
                            text: CurrencyService.localCurrencySymbol + " " + modelData.salePrice.toFixed(2)
                            font.pixelSize: 16
                            font.weight: Font.Bold
                            color: appColors.primary
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        
                        // Stock
                        Rectangle {
                            width: 80
                            height: 28
                            color: modelData.stockQuantity <= 5 ? appColors.warning + "20" : appColors.success + "20"
                            radius: 14
                            border.color: modelData.stockQuantity <= 5 ? appColors.warning : appColors.success
                            border.width: 1
                            
                            Text {
                                anchors.centerIn: parent
                                text: "Stock: " + modelData.stockQuantity
                                font.pixelSize: 11
                                color: modelData.stockQuantity <= 5 ? appColors.warning : appColors.success
                                font.weight: Font.Medium
                            }
                        }
                        
                        // Actions
                        Row {
                            spacing: 4
                            anchors.verticalCenter: parent.verticalCenter
                            
                            Button {
                                width: 32
                                height: 32
                                text: "👁️"
                                flat: true
                                
                                background: Rectangle {
                                    color: parent.hovered ? "#F0F0F0" : "transparent"
                                    radius: 6
                                }
                                
                                onClicked: viewProduct(modelData.id)
                            }
                            
                            Button {
                                width: 32
                                height: 32
                                text: "✏️"
                                flat: true
                                
                                background: Rectangle {
                                    color: parent.hovered ? appColors.warning + "20" : "transparent"
                                    radius: 6
                                }
                                
                                onClicked: editProduct(modelData.id)
                            }
                            
                            Button {
                                width: 32
                                height: 32
                                text: "🗑️"
                                flat: true
                                
                                background: Rectangle {
                                    color: parent.hovered ? appColors.danger + "20" : "transparent"
                                    radius: 6
                                }
                                
                                onClicked: deleteProduct(modelData.id)
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
        
        title: editingProduct ? "Editar Producto" : "Nuevo Producto"
        width: 600
        height: 650
        
        property bool editingProduct: false
        property var currentProduct: null
        
        background: Rectangle {
            color: appColors.surface
            radius: 12
            border.color: "#E0E0E0"
            border.width: 1
        }
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 16
            
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                ColumnLayout {
                    width: parent.width
                    spacing: 16
                    
                    // Basic Information
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 200
                        color: appColors.background
                        radius: 8
                        border.color: "#E0E0E0"
                        border.width: 1
                        
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 16
                            spacing: 12
                            
                            Text {
                                text: "📝 Información Básica"
                                font.pixelSize: 14
                                font.weight: Font.Bold
                                color: appColors.text
                            }
                            
                            TextField {
                                id: nameField
                                Layout.fillWidth: true
                                placeholderText: "Nombre del producto *"
                                
                                background: Rectangle {
                                    radius: 6
                                    color: appColors.surface
                                    border.color: nameField.activeFocus ? appColors.primary : "#E0E0E0"
                                    border.width: 1
                                }
                            }
                            
                            TextField {
                                id: descriptionField
                                Layout.fillWidth: true
                                Layout.preferredHeight: 60
                                placeholderText: "Descripción del producto"
                                
                                background: Rectangle {
                                    radius: 6
                                    color: appColors.surface
                                    border.color: descriptionField.activeFocus ? appColors.primary : "#E0E0E0"
                                    border.width: 1
                                }
                            }
                            
                            Row {
                                Layout.fillWidth: true
                                spacing: 12
                                
                                TextField {
                                    id: skuField
                                    Layout.preferredWidth: 150
                                    placeholderText: "SKU"
                                    
                                    background: Rectangle {
                                        radius: 6
                                        color: appColors.surface
                                        border.color: skuField.activeFocus ? appColors.primary : "#E0E0E0"
                                        border.width: 1
                                    }
                                }
                                
                                TextField {
                                    id: barcodeField
                                    Layout.preferredWidth: 150
                                    placeholderText: "Código de barras"
                                    
                                    background: Rectangle {
                                        radius: 6
                                        color: appColors.surface
                                        border.color: barcodeField.activeFocus ? appColors.primary : "#E0E0E0"
                                        border.width: 1
                                    }
                                }
                                
                                Item { Layout.fillWidth: true }
                                
                                TextField {
                                    id: categoryField
                                    Layout.preferredWidth: 150
                                    placeholderText: "Categoría"
                                    
                                    background: Rectangle {
                                        radius: 6
                                        color: appColors.surface
                                        border.color: categoryField.activeFocus ? appColors.primary : "#E0E0E0"
                                        border.width: 1
                                    }
                                }
                            }
                        }
                    }
                    
                    // Pricing Information
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 200
                        color: appColors.background
                        radius: 8
                        border.color: "#E0E0E0"
                        border.width: 1
                        
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 16
                            spacing: 12
                            
                            Text {
                                text: "💰 Precios y Costos"
                                font.pixelSize: 14
                                font.weight: Font.Bold
                                color: appColors.text
                            }
                            
                            Row {
                                Layout.fillWidth: true
                                spacing: 12
                                
                                Column {
                                    spacing: 4
                                    
                                    Text {
                                        text: "Costo USD:"
                                        font.pixelSize: 12
                                        color: appColors.textSecondary
                                    }
                                    
                                    TextField {
                                        id: costUsdField
                                        width: 120
                                        text: "0.00"
                                        
                                        validator: DoubleValidator {
                                            bottom: 0
                                            decimals: 2
                                        }
                                        
                                        background: Rectangle {
                                            radius: 6
                                            color: appColors.surface
                                            border.color: costUsdField.activeFocus ? appColors.primary : "#E0E0E0"
                                            border.width: 1
                                        }
                                        
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
                                    spacing: 4
                                    
                                    Text {
                                        text: "Margen (%):"
                                        font.pixelSize: 12
                                        color: appColors.textSecondary
                                    }
                                    
                                    TextField {
                                        id: marginField
                                        width: 80
                                        text: "30"
                                        
                                        validator: DoubleValidator {
                                            bottom: 0
                                            decimals: 2
                                        }
                                        
                                        background: Rectangle {
                                            radius: 6
                                            color: appColors.surface
                                            border.color: marginField.activeFocus ? appColors.primary : "#E0E0E0"
                                            border.width: 1
                                        }
                                        
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
                                    spacing: 4
                                    
                                    Text {
                                        text: "Precio Venta:"
                                        font.pixelSize: 12
                                        color: appColors.textSecondary
                                    }
                                    
                                    TextField {
                                        id: calculatedSalePriceField
                                        width: 120
                                        text: "0.00"
                                        enabled: false
                                        
                                        background: Rectangle {
                                            radius: 6
                                            color: "#F0F0F0"
                                            border.color: "#E0E0E0"
                                            border.width: 1
                                        }
                                    }
                                }
                                
                                Item { Layout.fillWidth: true }
                                
                                Column {
                                    spacing: 4
                                    
                                    Text {
                                        text: "Tasa actual:"
                                        font.pixelSize: 12
                                        color: appColors.textSecondary
                                    }
                                    
                                    Text {
                                        text: CurrencyService.localCurrencySymbol + " " + CurrencyService.exchangeRateFormatted
                                        font.pixelSize: 14
                                        font.weight: Font.Bold
                                        color: appColors.primary
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
                            
                            TextField {
                                id: manualSalePriceField
                                Layout.fillWidth: true
                                placeholderText: "Precio de venta manual"
                                enabled: false
                                
                                validator: DoubleValidator {
                                    bottom: 0
                                    decimals: 2
                                }
                                
                                background: Rectangle {
                                    radius: 6
                                    color: appColors.surface
                                    border.color: manualSalePriceField.activeFocus ? appColors.primary : "#E0E0E0"
                                    border.width: 1
                                }
                            }
                        }
                    }
                    
                    // Stock Information
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 100
                        color: appColors.background
                        radius: 8
                        border.color: "#E0E0E0"
                        border.width: 1
                        
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 16
                            spacing: 12
                            
                            Text {
                                text: "📦 Inventario"
                                font.pixelSize: 14
                                font.weight: Font.Bold
                                color: appColors.text
                            }
                            
                            Row {
                                Layout.fillWidth: true
                                spacing: 12
                                
                                Column {
                                    spacing: 4
                                    
                                    Text {
                                        text: "Stock inicial:"
                                        font.pixelSize: 12
                                        color: appColors.textSecondary
                                    }
                                    
                                    TextField {
                                        id: stockField
                                        width: 80
                                        text: "0"
                                        
                                        validator: IntValidator {
                                            bottom: 0
                                        }
                                        
                                        background: Rectangle {
                                            radius: 6
                                            color: appColors.surface
                                            border.color: stockField.activeFocus ? appColors.primary : "#E0E0E0"
                                            border.width: 1
                                        }
                                    }
                                }
                                
                                Column {
                                    spacing: 4
                                    
                                    Text {
                                        text: "Alerta bajo stock:"
                                        font.pixelSize: 12
                                        color: appColors.textSecondary
                                    }
                                    
                                    TextField {
                                        id: minStockField
                                        width: 80
                                        text: "5"
                                        
                                        validator: IntValidator {
                                            bottom: 0
                                        }
                                        
                                        background: Rectangle {
                                            radius: 6
                                            color: appColors.surface
                                            border.color: minStockField.activeFocus ? appColors.primary : "#E0E0E0"
                                            border.width: 1
                                        }
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
                }
            }
            
            // Action buttons
            Row {
                Layout.alignment: Qt.AlignRight
                spacing: 12
                
                Button {
                    text: "Cancelar"
                    flat: true
                    
                    background: Rectangle {
                        color: parent.hovered ? "#F0F0F0" : "transparent"
                        radius: 8
                    }
                    
                    onClicked: {
                        clearForm()
                        newProductDialog.close()
                    }
                }
                
                Button {
                    text: editingProduct ? "Actualizar Producto" : "Guardar Producto"
                    
                    background: Rectangle {
                        color: parent.hovered ? appColors.primaryDark : appColors.primary
                        radius: 8
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font.pixelSize: 14
                        font.weight: Font.Medium
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    onClicked: saveProduct()
                }
            }
        }
        
        function clearForm() {
            nameField.text = ""
            descriptionField.text = ""
            skuField.text = ""
            barcodeField.text = ""
            categoryField.text = ""
            costUsdField.text = "0.00"
            marginField.text = "30"
            manualPriceCheck.checked = false
            manualSalePriceField.text = "0.00"
            stockField.text = "0"
            minStockField.text = "5"
            activeCheck.checked = true
            editingProduct = false
            currentProduct = null
        }
        
        function loadProduct(productId) {
            var result = InventoryService.getProduct(productId)
            if (result.success) {
                var product = result.product
                nameField.text = product.name || ""
                descriptionField.text = product.description || ""
                skuField.text = product.sku || ""
                barcodeField.text = product.barcode || ""
                categoryField.text = product.category || ""
                costUsdField.text = product.costUsd ? product.costUsd.toFixed(2) : "0.00"
                marginField.text = product.marginPercent ? product.marginPercent.toFixed(2) : "30"
                manualSalePriceField.text = product.salePrice ? product.salePrice.toFixed(2) : "0.00"
                stockField.text = product.stockQuantity || 0
                minStockField.text = product.minStockAlert || 5
                activeCheck.checked = product.isActive !== false
                editingProduct = true
                currentProduct = product
                open()
            }
        }
        
        function saveProduct() {
            if (nameField.text.trim() === "") {
                console.log("Error: El nombre es requerido")
                return
            }
            
            var productData = {
                name: nameField.text.trim(),
                description: descriptionField.text.trim(),
                sku: skuField.text.trim(),
                barcode: barcodeField.text.trim(),
                category: categoryField.text.trim(),
                costUsd: parseFloat(costUsdField.text) || 0,
                marginPercent: parseFloat(marginField.text) || 0,
                salePrice: manualPriceCheck.checked ? parseFloat(manualSalePriceField.text) || 0 : parseFloat(calculatedSalePriceField.text) || 0,
                stockQuantity: parseInt(stockField.text) || 0,
                minStockAlert: parseInt(minStockField.text) || 5,
                isActive: activeCheck.checked
            }
            
            var result
            if (editingProduct && currentProduct) {
                result = InventoryService.updateProduct(currentProduct.id, productData)
            } else {
                result = InventoryService.createProduct(productData)
            }
            
            if (result.success) {
                clearForm()
                close()
                console.log("Producto guardado exitosamente")
            } else {
                console.log("Error al guardar producto:", result.error)
            }
        }
    }
    
    //==========================================================================
    // Funciones
    //==========================================================================
    function viewProduct(productId) {
        newProductDialog.loadProduct(productId)
    }
    
    function editProduct(productId) {
        newProductDialog.loadProduct(productId)
    }
    
    function deleteProduct(productId) {
        // Simple confirmation dialog
        var result = InventoryService.getProduct(productId)
        if (result.success) {
            console.log("Eliminar producto:", result.product.name)
            InventoryService.deleteProduct(productId)
        }
    }
    
    // Search functionality
    function searchProducts() {
        if (searchField.text.trim() === "") {
            productGrid.model = InventoryService.getActiveProducts()
            productList.model = InventoryService.getActiveProducts()
        } else {
            var results = InventoryService.searchProducts(searchField.text.trim())
            productGrid.model = results
            productList.model = results
        }
    }
    
    // Category filter
    function filterByCategory() {
        if (categoryCombo.currentIndex === 0) {
            searchProducts()
        } else {
            var category = categoryCombo.currentText
            var results = InventoryService.getProductsByCategory(category)
            productGrid.model = results
            productList.model = results
        }
    }
    
    // Connections for real-time updates
    Connections {
        target: InventoryService
        
        function onProductAdded(product) {
            searchProducts()
        }
        
        function onProductUpdated(product) {
            searchProducts()
        }
        
        function onProductRemoved(productId) {
            searchProducts()
        }
        
        function onCountChanged() {
            searchProducts()
        }
    }
    
    Connections {
        target: CurrencyService
        
        function onExchangeRateChanged() {
            // Update price displays when exchange rate changes
            searchProducts()
        }
    }
    
    // Connect search and category filters
    Component.onCompleted: {
        searchField.textChanged.connect(searchProducts)
        categoryCombo.currentIndexChanged.connect(filterByCategory)
        searchProducts()
    }
}
