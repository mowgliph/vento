//==================================================================
// SalesView.qml
// Vento - Vista de gestión de ventas
//==================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Vento.Sales
import Vento.Inventory
import Vento.Currency

ColumnLayout {
    id: salesView
    
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
                text: "🛒 Ventas"
                font.pixelSize: 28
                font.weight: Font.Bold
                color: appColors.text
            }
            
            Text {
                text: "Gestiona tus ventas y procesa pedidos"
                font.pixelSize: 14
                color: appColors.textSecondary
            }
        }
        
        Item { Layout.fillWidth: true }
        
        Button {
            text: "📋 Ver Ventas"
            
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
            
            onClicked: salesHistoryDialog.open()
        }
    }
    
    //==========================================================================
    // Área principal de ventas
    //==========================================================================
    Row {
        Layout.fillWidth: true
        Layout.fillHeight: true
        spacing: 16
        
        //======================================================================
        // Panel izquierdo - Carrito
        //======================================================================
        Rectangle {
            Layout.preferredWidth: 400
            Layout.fillHeight: true
            color: appColors.surface
            radius: 12
            border.color: "#E0E0E0"
            border.width: 1
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 16
                
                // Header del carrito
                Row {
                    Layout.fillWidth: true
                    
                    Text {
                        text: "🛒 Carrito de Compras"
                        font.pixelSize: 16
                        font.weight: Font.Bold
                        color: appColors.text
                    }
                    
                    Item { Layout.fillWidth: true }
                    
                    Button {
                        text: "🗑️ Vaciar"
                        implicitWidth: 60
                        implicitHeight: 32
                        
                        background: Rectangle {
                            color: parent.hovered ? appColors.danger + "20" : "transparent"
                            radius: 6
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            color: appColors.danger
                            font.pixelSize: 11
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        
                        onClicked: SalesService.clearCart()
                    }
                }
                
                // Items del carrito
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    ListView {
                        id: cartList
                        
                        model: [] // Will be connected to SalesService cart
                        
                        delegate: Rectangle {
                            width: cartList.width
                            height: 80
                            color: index % 2 === 0 ? "transparent" : appColors.background
                            border.color: "#F0F0F0"
                            border.width: 1
                            
                            Row {
                                anchors.fill: parent
                                anchors.leftMargin: 12
                                anchors.rightMargin: 12
                                spacing: 12
                                
                                // Producto info
                                Column {
                                    Layout.fillWidth: true
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 2
                                    
                                    Text {
                                        text: modelData.name || "Producto"
                                        font.pixelSize: 13
                                        font.weight: Font.Bold
                                        color: appColors.text
                                    }
                                    
                                    Text {
                                        text: CurrencyService.localCurrencySymbol + " " + (modelData.unitPrice || 0).toFixed(2) + " x " + (modelData.quantity || 1)
                                        font.pixelSize: 11
                                        color: appColors.textSecondary
                                    }
                                }
                                
                                // Subtotal
                                Text {
                                    text: CurrencyService.localCurrencySymbol + " " + (modelData.subtotal || 0).toFixed(2)
                                    font.pixelSize: 14
                                    font.weight: Font.Bold
                                    color: appColors.primary
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                
                                // Botones de cantidad
                                Column {
                                    spacing: 2
                                    anchors.verticalCenter: parent.verticalCenter
                                    
                                    Row {
                                        spacing: 4
                                        
                                        Button {
                                            width: 24
                                            height: 24
                                            text: "-"
                                            
                                            background: Rectangle {
                                                color: parent.hovered ? appColors.warning : appColors.warning + "80"
                                                radius: 4
                                            }
                                            
                                            contentItem: Text {
                                                text: parent.text
                                                color: "white"
                                                font.pixelSize: 12
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }
                                            
                                            onClicked: {
                                                var index = cartList.indexAt(modelData.index)
                                                if (index >= 0) {
                                                    SalesService.updateCartQuantity(index, modelData.quantity - 1)
                                                }
                                            }
                                        }
                                        
                                        Text {
                                            width: 30
                                            text: modelData.quantity || 1
                                            font.pixelSize: 12
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        
                                        Button {
                                            width: 24
                                            height: 24
                                            text: "+"
                                            
                                            background: Rectangle {
                                                color: parent.hovered ? appColors.success : appColors.success + "80"
                                                radius: 4
                                            }
                                            
                                            contentItem: Text {
                                                text: parent.text
                                                color: "white"
                                                font.pixelSize: 12
                                                horizontalAlignment: Text.AlignHCenter
                                                verticalAlignment: Text.AlignVCenter
                                            }
                                            
                                            onClicked: {
                                                var index = cartList.indexAt(modelData.index)
                                                if (index >= 0) {
                                                    SalesService.updateCartQuantity(index, modelData.quantity + 1)
                                                }
                                            }
                                        }
                                    }
                                    
                                    Button {
                                        width: 50
                                        height: 20
                                        text: "Eliminar"
                                        
                                        background: Rectangle {
                                            color: parent.hovered ? appColors.danger + "20" : "transparent"
                                            radius: 4
                                        }
                                        
                                        contentItem: Text {
                                            text: parent.text
                                            color: appColors.danger
                                            font.pixelSize: 9
                                            horizontalAlignment: Text.AlignHCenter
                                            verticalAlignment: Text.AlignVCenter
                                        }
                                        
                                        onClicked: {
                                            var index = cartList.indexAt(modelData.index)
                                            if (index >= 0) {
                                                SalesService.removeFromCart(index)
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                
                // Total del carrito
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 80
                    color: appColors.background
                    radius: 8
                    border.color: "#E0E0E0"
                    border.width: 1
                    
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 8
                        
                        Row {
                            Layout.fillWidth: true
                            
                            Text {
                                text: "Subtotal:"
                                font.pixelSize: 12
                                color: appColors.textSecondary
                            }
                            
                            Text {
                                text: CurrencyService.localCurrencySymbol + " 0.00"
                                font.pixelSize: 12
                                color: appColors.text
                            }
                        }
                        
                        Row {
                            Layout.fillWidth: true
                            
                            Text {
                                text: "IVA (16%):"
                                font.pixelSize: 12
                                color: appColors.textSecondary
                            }
                            
                            Text {
                                text: CurrencyService.localCurrencySymbol + " 0.00"
                                font.pixelSize: 12
                                color: appColors.text
                            }
                        }
                        
                        Rectangle {
                            Layout.fillWidth: true
                            height: 1
                            color: "#E0E0E0"
                        }
                        
                        Row {
                            Layout.fillWidth: true
                            
                            Text {
                                text: "TOTAL:"
                                font.pixelSize: 16
                                font.weight: Font.Bold
                                color: appColors.text
                            }
                            
                            Text {
                                id: totalLabel
                                text: CurrencyService.localCurrencySymbol + " 0.00"
                                font.pixelSize: 18
                                font.weight: Font.Bold
                                color: appColors.primary
                            }
                        }
                    }
                }
                
                // Botones de acción
                Row {
                    Layout.fillWidth: true
                    spacing: 12
                    
                    Button {
                        Layout.fillWidth: true
                        text: "💳 Procesar Venta"
                        
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
                        
                        onClicked: processSaleDialog.open()
                    }
                }
            }
        }
        
        //======================================================================
        // Panel derecho - Productos
        //======================================================================
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: appColors.surface
            radius: 12
            border.color: "#E0E0E0"
            border.width: 1
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 16
                
                // Header de productos
                Row {
                    Layout.fillWidth: true
                    
                    Text {
                        text: "📦 Productos Disponibles"
                        font.pixelSize: 16
                        font.weight: Font.Bold
                        color: appColors.text
                    }
                    
                    Item { Layout.fillWidth: true }
                    
                    TextField {
                        id: productSearchField
                        Layout.preferredWidth: 200
                        placeholderText: "🔍 Buscar..."
                        
                        background: Rectangle {
                            radius: 6
                            color: appColors.surface
                            border.color: productSearchField.activeFocus ? appColors.primary : "#E0E0E0"
                            border.width: 1
                        }
                    }
                }
                
                // Scanner de código de barras
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 60
                    color: appColors.background
                    radius: 8
                    border.color: "#E0E0E0"
                    border.width: 1
                    
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 12
                        spacing: 12
                        
                        Text {
                            text: "📷 Escáner:"
                            font.pixelSize: 14
                            color: appColors.text
                        }
                        
                        TextField {
                            id: barcodeField
                            Layout.fillWidth: true
                            placeholderText: "Escanee o ingrese código de barras"
                            
                            background: Rectangle {
                                radius: 6
                                color: appColors.surface
                                border.color: barcodeField.activeFocus ? appColors.primary : "#E0E0E0"
                                border.width: 1
                            }
                            
                            Keys.onReturnPressed: {
                                if (text.trim() !== "") {
                                    SalesService.addToCartByBarcode(text.trim())
                                    text = ""
                                }
                            }
                        }
                        
                        Button {
                            text: "🔍 Buscar"
                            
                            background: Rectangle {
                                color: parent.hovered ? appColors.primaryDark : appColors.primary
                                radius: 6
                            }
                            
                            contentItem: Text {
                                text: parent.text
                                color: "white"
                                font.pixelSize: 12
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                            
                            onClicked: {
                                if (barcodeField.text.trim() !== "") {
                                    SalesService.addToCartByBarcode(barcodeField.text.trim())
                                    barcodeField.text = ""
                                }
                            }
                        }
                    }
                }
                
                // Lista de productos
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    
                    GridView {
                        id: productsGrid
                        
                        cellWidth: 200
                        cellHeight: 120
                        
                        model: InventoryService.getActiveProducts()
                        
                        delegate: Rectangle {
                            width: productsGrid.cellWidth - 8
                            height: productsGrid.cellHeight - 8
                            color: appColors.surface
                            radius: 8
                            border.color: "#E0E0E0"
                            border.width: 1
                            
                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 8
                                spacing: 4
                                
                                Text {
                                    text: modelData.name || "Producto"
                                    font.pixelSize: 11
                                    font.weight: Font.Bold
                                    color: appColors.text
                                    wrapMode: Text.WordWrap
                                    Layout.fillWidth: true
                                    maximumLineCount: 2
                                }
                                
                                Text {
                                    text: CurrencyService.localCurrencySymbol + " " + (modelData.salePrice || 0).toFixed(2)
                                    font.pixelSize: 12
                                    font.weight: Font.Bold
                                    color: appColors.primary
                                }
                                
                                Text {
                                    text: "Stock: " + (modelData.stockQuantity || 0)
                                    font.pixelSize: 10
                                    color: modelData.stockQuantity <= 5 ? appColors.warning : appColors.textSecondary
                                }
                                
                                Item { Layout.fillHeight: true }
                                
                                Button {
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 24
                                    text: "➕ Agregar"
                                    
                                    background: Rectangle {
                                        color: parent.hovered ? appColors.success : appColors.success + "80"
                                        radius: 4
                                    }
                                    
                                    contentItem: Text {
                                        text: parent.text
                                        color: "white"
                                        font.pixelSize: 10
                                        font.weight: Font.Medium
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }
                                    
                                    onClicked: {
                                        SalesService.addToCart(modelData.id, 1)
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    //==========================================================================
    // Diálogo de procesar venta
    //==========================================================================
    Dialog {
        id: processSaleDialog
        
        title: "Procesar Venta"
        width: 500
        height: 400
        
        background: Rectangle {
            color: appColors.surface
            radius: 12
            border.color: "#E0E0E0"
            border.width: 1
        }
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 16
            
            Text {
                text: "💳 Información de la Venta"
                font.pixelSize: 16
                font.weight: Font.Bold
                color: appColors.text
            }
            
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: appColors.background
                radius: 8
                border.color: "#E0E0E0"
                border.width: 1
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 12
                    
                    TextField {
                        id: customerNameField
                        Layout.fillWidth: true
                        placeholderText: "Nombre del cliente (opcional)"
                        
                        background: Rectangle {
                            radius: 6
                            color: appColors.surface
                            border.color: customerNameField.activeFocus ? appColors.primary : "#E0E0E0"
                            border.width: 1
                        }
                    }
                    
                    ComboBox {
                        id: paymentMethodCombo
                        Layout.fillWidth: true
                        model: ["Efectivo", "Tarjeta", "Transferencia", "Móvil"]
                        currentIndex: 0
                        
                        background: Rectangle {
                            color: appColors.surface
                            border.color: "#E0E0E0"
                            border.width: 1
                            radius: 6
                        }
                    }
                    
                    TextField {
                        id: notesField
                        Layout.fillWidth: true
                        Layout.preferredHeight: 60
                        placeholderText: "Notas (opcional)"
                        
                        background: Rectangle {
                            radius: 6
                            color: appColors.surface
                            border.color: notesField.activeFocus ? appColors.primary : "#E0E0E0"
                            border.width: 1
                        }
                    }
                    
                    Item { Layout.fillHeight: true }
                    
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 60
                        color: "#F8F9FA"
                        radius: 8
                        border.color: "#E0E0E0"
                        border.width: 1
                        
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 12
                            spacing: 4
                            
                            Text {
                                text: "Total a pagar:"
                                font.pixelSize: 12
                                color: appColors.textSecondary
                            }
                            
                            Text {
                                text: totalLabel.text
                                font.pixelSize: 20
                                font.weight: Font.Bold
                                color: appColors.primary
                            }
                        }
                    }
                }
            }
            
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
                    
                    onClicked: processSaleDialog.close()
                }
                
                Button {
                    text: "💳 Confirmar Venta"
                    
                    background: Rectangle {
                        color: parent.hovered ? appColors.success : "#107C10"
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
                    
                    onClicked: {
                        var result = SalesService.processSale(
                            paymentMethodCombo.currentText,
                            customerNameField.text,
                            notesField.text
                        )
                        
                        if (result.success) {
                            console.log("Venta procesada:", result.saleId, "Total:", result.total)
                            processSaleDialog.close()
                            // Show receipt or success message
                        } else {
                            console.log("Error en venta:", result.error)
                        }
                    }
                }
            }
        }
    }
    
    //==========================================================================
    // Diálogo de historial de ventas
    //==========================================================================
    Dialog {
        id: salesHistoryDialog
        
        title: "Historial de Ventas"
        width: 800
        height: 600
        
        background: Rectangle {
            color: appColors.surface
            radius: 12
            border.color: "#E0E0E0"
            border.width: 1
        }
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 16
            
            Text {
                text: "📋 Ventas Recientes"
                font.pixelSize: 16
                font.weight: Font.Bold
                color: appColors.text
            }
            
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                ListView {
                    id: salesHistoryList
                    
                    model: [] // Will be connected to SalesService sales history
                    
                    delegate: Rectangle {
                        width: salesHistoryList.width
                        height: 60
                        color: index % 2 === 0 ? "transparent" : appColors.background
                        border.color: "#F0F0F0"
                        border.width: 1
                        
                        Row {
                            anchors.fill: parent
                            anchors.leftMargin: 12
                            anchors.rightMargin: 12
                            spacing: 16
                            
                            Column {
                                Layout.preferredWidth: 150
                                spacing: 2
                                
                                Text {
                                    text: modelData.saleDate || ""
                                    font.pixelSize: 11
                                    color: appColors.textSecondary
                                }
                                
                                Text {
                                    text: "#" + (modelData.id || "")
                                    font.pixelSize: 10
                                    color: appColors.text
                                }
                            }
                            
                            Column {
                                Layout.fillWidth: true
                                spacing: 2
                                
                                Text {
                                    text: modelData.customerName || "Cliente general"
                                    font.pixelSize: 12
                                    font.weight: Font.Bold
                                    color: appColors.text
                                }
                                
                                Text {
                                    text: (modelData.itemCount || 0) + " productos"
                                    font.pixelSize: 10
                                    color: appColors.textSecondary
                                }
                            }
                            
                            Text {
                                text: CurrencyService.localCurrencySymbol + " " + (modelData.total || 0).toFixed(2)
                                font.pixelSize: 14
                                font.weight: Font.Bold
                                color: appColors.primary
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            
                            Rectangle {
                                width: 60
                                height: 24
                                radius: 12
                                color: modelData.status === "completed" ? appColors.success : 
                                       modelData.status === "cancelled" ? appColors.danger : appColors.warning
                                
                                Text {
                                    anchors.centerIn: parent
                                    text: modelData.status === "completed" ? "Completada" : 
                                          modelData.status === "cancelled" ? "Cancelada" : "Pendiente"
                                    color: "white"
                                    font.pixelSize: 9
                                    font.weight: Font.Bold
                                }
                            }
                        }
                    }
                }
            }
            
            Row {
                Layout.alignment: Qt.AlignRight
                
                Button {
                    text: "Cerrar"
                    
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
                    
                    onClicked: salesHistoryDialog.close()
                }
            }
        }
    }
    
    //==========================================================================
    // Conexiones y actualización
    //==========================================================================
    Connections {
        target: SalesService
        
        function onCartChanged() {
            // Update cart display
            updateCartDisplay()
        }
        
        function onSaleCompleted(saleId, total, receiptNumber) {
            console.log("Venta completada:", saleId, total, receiptNumber)
            // Could show receipt dialog here
        }
        
        function onErrorOccurred(error) {
            console.log("Error en ventas:", error)
        }
    }
    
    function updateCartDisplay() {
        // This would update the cart list and totals
        // For now, just update the total label
        // In a real implementation, this would connect to the cart model
        totalLabel.text = CurrencyService.localCurrencySymbol + " 0.00"
    }
    
    Component.onCompleted: {
        updateCartDisplay()
    }
}
