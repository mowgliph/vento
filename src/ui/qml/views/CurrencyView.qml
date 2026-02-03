//==================================================================
// CurrencyView.qml
// Vento - Vista de gestión de tipo de cambio
//==================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Vento.Currency

ColumnLayout {
    id: currencyView
    
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
                text: "💱 Tipo de Cambio"
                font.pixelSize: 28
                font.weight: Font.Bold
                color: "#1A1A1A"
            }
            
            Text {
                text: "Gestiona la tasa de cambio y actualiza precios automáticamente"
                font.pixelSize: 14
                color: "#6B6B6B"
            }
        }
        
        Item { Layout.fillWidth: true }
        
        Button {
            text: "🔄 Actualizar Precios"
            
            background: Rectangle {
                color: parent.hovered ? "#107C10" : "#107C10"
                radius: 8
            }
            
            contentItem: Text {
                text: parent.text
                color: "white"
                font.pixelSize: 14
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            
            onClicked: updateAllPricesDialog.open()
        }
    }
    
    //==========================================================================
    // Tarjeta de tasa actual
    //==========================================================================
    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 120
        color: "#FFFFFF"
        radius: 12
        border.color: "#E0E0E0"
        border.width: 1
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20
            
            // Icono y tasa actual
            Column {
                spacing: 8
                
                Text {
                    text: "💵"
                    font.pixelSize: 32
                }
                
                Text {
                    text: "USD → CUP"
                    font.pixelSize: 12
                    color: "#6B6B6B"
                }
            }
            
            Rectangle {
                Layout.preferredWidth: 1
                Layout.fillHeight: true
                color: "#E0E0E0"
            }
            
            // Tasa actual
            Column {
                spacing: 4
                
                Text {
                    text: "Tasa Actual"
                    font.pixelSize: 12
                    color: "#6B6B6B"
                }
                
                Text {
                    id: currentRateLabel
                    text: CurrencyService.exchangeRateFormatted
                    font.pixelSize: 24
                    font.weight: Font.Bold
                    color: "#0078D4"
                }
                
                Text {
                    id: lastUpdateLabel
                    text: "Actualizado: " + CurrencyService.lastUpdateFormatted
                    font.pixelSize: 11
                    color: "#6B6B6B"
                }
            }
            
            Item { Layout.fillWidth: true }
            
            // Trend indicator
            Column {
                spacing: 4
                
                Text {
                    id: trendIcon
                    text: CurrencyService.trend === "up" ? "📈" : 
                          CurrencyService.trend === "down" ? "📉" : "➡️"
                    font.pixelSize: 24
                }
                
                Text {
                    id: changePercentLabel
                    text: (CurrencyService.changePercent >= 0 ? "+" : "") + 
                          CurrencyService.changePercent.toFixed(2) + "%"
                    font.pixelSize: 12
                    color: CurrencyService.changePercent >= 0 ? "#107C10" : "#D13438"
                }
            }
        }
    }
    
    //==========================================================================
    // Controles de actualización
    //==========================================================================
    Rectangle {
        Layout.fillWidth: true
        Layout.preferredHeight: 100
        color: "#FFFFFF"
        radius: 12
        border.color: "#E0E0E0"
        border.width: 1
        
        RowLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 16
            
            Column {
                spacing: 8
                
                Text {
                    text: "Nueva Tasa:"
                    font.pixelSize: 12
                    color: "#6B6B6B"
                }
                
                TextField {
                    id: newRateField
                    Layout.preferredWidth: 120
                    placeholderText: "0.00"
                    text: CurrencyService.exchangeRate.toFixed(2)
                    
                    validator: DoubleValidator {
                        bottom: 0.01
                        top: 1000000
                        decimals: 2
                    }
                    
                    background: Rectangle {
                        radius: 6
                        color: newRateField.activeFocus ? "#F0F0F0" : "#FFFFFF"
                        border.color: newRateField.activeFocus ? "#0078D4" : "#E0E0E0"
                        border.width: 1
                    }
                }
            }
            
            Column {
                spacing: 8
                
                Text {
                    text: "Fuente:"
                    font.pixelSize: 12
                    color: "#6B6B6B"
                }
                
                ComboBox {
                    id: sourceCombo
                    Layout.preferredWidth: 120
                    model: ["Manual", "BCV", "DolarToday", "Binance"]
                    currentIndex: 0
                    
                    background: Rectangle {
                        radius: 6
                        color: "#FFFFFF"
                        border.color: "#E0E0E0"
                        border.width: 1
                    }
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: "Actualizar Tasa"
                
                background: Rectangle {
                    color: parent.hovered ? "#005A9E" : "#0078D4"
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
                    var newRate = parseFloat(newRateField.text)
                    if (!isNaN(newRate) && newRate > 0) {
                        var source = sourceCombo.currentText.toLowerCase()
                        CurrencyService.updateRateFromSource(newRate, source)
                    }
                }
            }
        }
    }
    
    //==========================================================================
    // Historial de tasas
    //==========================================================================
    Rectangle {
        Layout.fillWidth: true
        Layout.fillHeight: true
        color: "#FFFFFF"
        radius: 12
        border.color: "#E0E0E0"
        border.width: 1
        
        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 16
            
            Row {
                Layout.fillWidth: true
                
                Text {
                    text: "📊 Historial de Tasas"
                    font.pixelSize: 16
                    font.weight: Font.Bold
                    color: "#1A1A1A"
                }
                
                Item { Layout.fillWidth: true }
                
                Button {
                    text: "🔄 Refrescar"
                    implicitWidth: 80
                    implicitHeight: 32
                    
                    background: Rectangle {
                        color: parent.hovered ? "#F3F3F3" : "#FFFFFF"
                        radius: 6
                        border.color: "#E0E0E0"
                        border.width: 1
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        color: "#1A1A1A"
                        font.pixelSize: 11
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    onClicked: historyModel.reload()
                }
            }
            
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                
                ListView {
                    id: historyList
                    model: CurrencyService.getRateHistory(50)
                    
                    delegate: Rectangle {
                        width: historyList.width
                        height: 40
                        color: index % 2 === 0 ? "#FAFAFA" : "#FFFFFF"
                        border.color: "#F0F0F0"
                        border.width: 1
                        
                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 12
                            anchors.rightMargin: 12
                            spacing: 16
                            
                            Text {
                                Layout.preferredWidth: 120
                                text: modelData.timestamp
                                font.pixelSize: 11
                                color: "#6B6B6B"
                            }
                            
                            Text {
                                Layout.preferredWidth: 80
                                text: modelData.rate
                                font.pixelSize: 12
                                font.weight: Font.Bold
                                color: "#1A1A1A"
                                horizontalAlignment: Text.AlignRight
                            }
                            
                            Text {
                                Layout.preferredWidth: 60
                                text: modelData.changePercent + "%"
                                font.pixelSize: 11
                                color: modelData.changePercent >= 0 ? "#107C10" : "#D13438"
                                horizontalAlignment: Text.AlignRight
                            }
                            
                            Rectangle {
                                Layout.preferredWidth: 80
                                Layout.preferredHeight: 20
                                radius: 10
                                color: modelData.source === "manual" ? "#6B6B6B" :
                                       modelData.source === "bcv" ? "#0078D4" :
                                       modelData.source === "dolartoday" ? "#FF8C00" : "#F7630C"
                                
                                Text {
                                    anchors.centerIn: parent
                                    text: modelData.source.toUpperCase()
                                    color: "white"
                                    font.pixelSize: 9
                                    font.weight: Font.Bold
                                }
                            }
                            
                            Item { Layout.fillWidth: true }
                            
                            Text {
                                text: modelData.trend === "up" ? "📈" : 
                                      modelData.trend === "down" ? "📉" : "➡️"
                                font.pixelSize: 14
                            }
                        }
                    }
                }
            }
        }
    }
    
    //==========================================================================
    // Dialogs
    //==========================================================================
    Dialog {
        id: updateAllPricesDialog
        
        title: "Actualizar Todos los Precios"
        modal: true
        
        ColumnLayout {
            spacing: 16
            
            Text {
                text: "¿Desea actualizar todos los precios de los productos basados en la nueva tasa de cambio?"
                font.pixelSize: 14
                wrapMode: Text.WordWrap
                Layout.preferredWidth: 400
            }
            
            Text {
                text: "Esta acción recalculará el precio de costo local y el precio de venta para todos los productos en USD."
                font.pixelSize: 12
                color: "#6B6B6B"
                wrapMode: Text.WordWrap
                Layout.preferredWidth: 400
            }
            
            Row {
                Layout.fillWidth: true
                spacing: 12
                
                Button {
                    text: "Cancelar"
                    Layout.preferredWidth: 100
                    
                    onClicked: updateAllPricesDialog.reject()
                }
                
                Item { Layout.fillWidth: true }
                
                Button {
                    text: "Actualizar Precios"
                    Layout.preferredWidth: 120
                    
                    background: Rectangle {
                        color: parent.hovered ? "#005A9E" : "#0078D4"
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
                        // Call inventory service to update all prices
                        updateAllPricesDialog.accept()
                    }
                }
            }
        }
    }
    
    //==========================================================================
    // Conexiones
    //==========================================================================
    Connections {
        target: CurrencyService
        
        function onExchangeRateChanged() {
            currentRateLabel.text = CurrencyService.exchangeRateFormatted
            newRateField.text = CurrencyService.exchangeRate.toFixed(2)
        }
        
        function onRateUpdated(newRate, previousRate, changePercent) {
            // Show success notification
            console.log("Tasa actualizada:", previousRate, "→", newRate)
        }
        
        function onErrorOccurred(error) {
            // Show error notification
            console.log("Error en tasa:", error)
        }
    }
}
