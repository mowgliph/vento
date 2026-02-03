//==================================================================
// SettingsView.qml
// Vento - Vista de configuración
//==================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vento.UI

ScrollView {
    id: settingsView
    
    contentWidth: availableWidth
    contentHeight: settingsColumn.implicitHeight
    clip: true
    
    ScrollBar.vertical: ScrollBar {
        active: settingsView.moving
        policy: ScrollBar.AsNeeded
    }
    
    //==========================================================================
    // Contenido principal
    //==========================================================================
    ColumnLayout {
        id: settingsColumn
        
        anchors.fill: parent
        anchors.margins: Theme.spacingXl
        spacing: Theme.spacingXl
        
        //======================================================================
        // Encabezado
        //======================================================================
        Column {
            spacing: Theme.spacingXs
            
            Text {
                text: "⚙️ Configuración"
                font.pixelSize: Theme.fontSizeDisplay
                font.weight: Theme.fontWeightBold
                color: Theme.textPrimary
            }
            
            Text {
                text: "Configura las preferencias del sistema"
                font.pixelSize: Theme.fontSizeMd
                color: Theme.textSecondary
            }
        }
        
        //======================================================================
        // Sección: General
        //======================================================================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: generalColumn.implicitHeight + Theme.spacingXl
            
            color: Theme.surface
            border.color: Theme.cardBorder
            border.width: 1
            radius: Theme.radiusLg
            
            ColumnLayout {
                id: generalColumn
                
                anchors.fill: parent
                anchors.margins: Theme.spacingLg
                spacing: Theme.spacingLg
                
                Text {
                    text: "🌐 General"
                    font.pixelSize: Theme.fontSizeLg
                    font.weight: Theme.fontWeightSemiBold
                    color: Theme.textPrimary
                }
                
                // Idioma
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingLg
                    
                    Column {
                        Layout.preferredWidth: 200
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Idioma"
                            font.pixelSize: Theme.fontSizeMd
                            font.weight: Theme.fontWeightMedium
                            color: Theme.textPrimary
                        }
                        
                        Text {
                            text: "Selecciona el idioma de la interfaz"
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }
                    }
                    
                    ComboBox {
                        Layout.fillWidth: true
                        model: ["Español", "English", "Português"]
                        currentIndex: 0
                        
                        background: Rectangle {
                            color: Theme.surface
                            border.color: Theme.border
                            border.width: 1
                            radius: Theme.radiusMd
                        }
                    }
                }
                
                // Tema
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingLg
                    
                    Column {
                        Layout.preferredWidth: 200
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Tema"
                            font.pixelSize: Theme.fontSizeMd
                            font.weight: Theme.fontWeightMedium
                            color: Theme.textPrimary
                        }
                        
                        Text {
                            text: "Selecciona el tema visual"
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }
                    }
                    
                    ComboBox {
                        Layout.fillWidth: true
                        model: ["Claro", "Oscuro", "Automático"]
                        currentIndex: 0
                        
                        background: Rectangle {
                            color: Theme.surface
                            border.color: Theme.border
                            border.width: 1
                            radius: Theme.radiusMd
                        }
                    }
                }
                
                // Moneda por defecto
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingLg
                    
                    Column {
                        Layout.preferredWidth: 200
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Moneda por defecto"
                            font.pixelSize: Theme.fontSizeMd
                            font.weight: Theme.fontWeightMedium
                            color: Theme.textPrimary
                        }
                        
                        Text {
                            text: "Moneda principal para mostrar precios"
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }
                    }
                    
                    ComboBox {
                        Layout.fillWidth: true
                        model: ["Bolívares (Bs.)", "Dólares (USD)", "Euros (EUR)"]
                        currentIndex: 0
                        
                        background: Rectangle {
                            color: Theme.surface
                            border.color: Theme.border
                            border.width: 1
                            radius: Theme.radiusMd
                        }
                    }
                }
            }
        }
        
        //======================================================================
        // Sección: Base de datos
        //======================================================================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: databaseColumn.implicitHeight + Theme.spacingXl
            
            color: Theme.surface
            border.color: Theme.cardBorder
            border.width: 1
            radius: Theme.radiusLg
            
            ColumnLayout {
                id: databaseColumn
                
                anchors.fill: parent
                anchors.margins: Theme.spacingLg
                spacing: Theme.spacingLg
                
                Text {
                    text: "💾 Base de datos"
                    font.pixelSize: Theme.fontSizeLg
                    font.weight: Theme.fontWeightSemiBold
                    color: Theme.textPrimary
                }
                
                // Ruta de la base de datos
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingLg
                    
                    Column {
                        Layout.preferredWidth: 200
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Ubicación"
                            font.pixelSize: Theme.fontSizeMd
                            font.weight: Theme.fontWeightMedium
                            color: Theme.textPrimary
                        }
                        
                        Text {
                            text: "Ruta donde se almacenan los datos"
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }
                    }
                    
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 40
                        
                        color: Theme.background
                        border.color: Theme.border
                        border.width: 1
                        radius: Theme.radiusMd
                        
                        Text {
                            anchors.fill: parent
                            anchors.leftMargin: Theme.spacingMd
                            anchors.rightMargin: Theme.spacingMd
                            text: "/home/user/.local/share/Vento/vento.db"
                            color: Theme.textSecondary
                            font.pixelSize: Theme.fontSizeSm
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }
                
                // Botones de acción
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingMd
                    
                    Button {
                        text: "📂 Abrir carpeta"
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
                        text: "💾 Respaldo"
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
                        text: "🔄 Restaurar"
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
            }
        }
        
        //======================================================================
        // Sección: Tipo de cambio
        //======================================================================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: currencyColumn.implicitHeight + Theme.spacingXl
            
            color: Theme.surface
            border.color: Theme.cardBorder
            border.width: 1
            radius: Theme.radiusLg
            
            ColumnLayout {
                id: currencyColumn
                
                anchors.fill: parent
                anchors.margins: Theme.spacingLg
                spacing: Theme.spacingLg
                
                Text {
                    text: "💱 Tipo de cambio"
                    font.pixelSize: Theme.fontSizeLg
                    font.weight: Theme.fontWeightSemiBold
                    color: Theme.textPrimary
                }
                
                // Fuente de tasa
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingLg
                    
                    Column {
                        Layout.preferredWidth: 200
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Fuente de tasa"
                            font.pixelSize: Theme.fontSizeMd
                            font.weight: Theme.fontWeightMedium
                            color: Theme.textPrimary
                        }
                        
                        Text {
                            text: "Proveedor del tipo de cambio"
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }
                    }
                    
                    ComboBox {
                        Layout.fillWidth: true
                        model: ["BCV (Banco Central)", "Parallel", "Monitor Dolar", "Manual"]
                        currentIndex: 0
                        
                        background: Rectangle {
                            color: Theme.surface
                            border.color: Theme.border
                            border.width: 1
                            radius: Theme.radiusMd
                        }
                    }
                }
                
                // Actualización automática
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingLg
                    
                    Column {
                        Layout.preferredWidth: 200
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Actualización automática"
                            font.pixelSize: Theme.fontSizeMd
                            font.weight: Theme.fontWeightMedium
                            color: Theme.textPrimary
                        }
                        
                        Text {
                            text: "Actualizar tasa automáticamente"
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }
                    }
                    
                    Switch {
                        checked: true
                        
                        background: Rectangle {
                            color: "transparent"
                        }
                    }
                }
                
                // Frecuencia
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingLg
                    
                    Column {
                        Layout.preferredWidth: 200
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Frecuencia"
                            font.pixelSize: Theme.fontSizeMd
                            font.weight: Theme.fontWeightMedium
                            color: Theme.textPrimary
                        }
                        
                        Text {
                            text: "Cada cuánto se actualiza"
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }
                    }
                    
                    ComboBox {
                        Layout.fillWidth: true
                        model: ["Cada hora", "Cada 6 horas", "Cada 12 horas", "Diariamente"]
                        currentIndex: 1
                        
                        background: Rectangle {
                            color: Theme.surface
                            border.color: Theme.border
                            border.width: 1
                            radius: Theme.radiusMd
                        }
                    }
                }
            }
        }
        
        //======================================================================
        // Sección: Notificaciones
        //======================================================================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: notificationsColumn.implicitHeight + Theme.spacingXl
            
            color: Theme.surface
            border.color: Theme.cardBorder
            border.width: 1
            radius: Theme.radiusLg
            
            ColumnLayout {
                id: notificationsColumn
                
                anchors.fill: parent
                anchors.margins: Theme.spacingLg
                spacing: Theme.spacingLg
                
                Text {
                    text: "🔔 Notificaciones"
                    font.pixelSize: Theme.fontSizeLg
                    font.weight: Theme.fontWeightSemiBold
                    color: Theme.textPrimary
                }
                
                // Notificaciones de ventas
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingLg
                    
                    Column {
                        Layout.preferredWidth: 200
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Ventas"
                            font.pixelSize: Theme.fontSizeMd
                            font.weight: Theme.fontWeightMedium
                            color: Theme.textPrimary
                        }
                        
                        Text {
                            text: "Notificar cuando se realice una venta"
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }
                    }
                    
                    Switch {
                        checked: true
                        
                        background: Rectangle {
                            color: "transparent"
                        }
                    }
                }
                
                // Notificaciones de stock bajo
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingLg
                    
                    Column {
                        Layout.preferredWidth: 200
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Stock bajo"
                            font.pixelSize: Theme.fontSizeMd
                            font.weight: Theme.fontWeightMedium
                            color: Theme.textPrimary
                        }
                        
                        Text {
                            text: "Alertar cuando el stock sea bajo"
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }
                    }
                    
                    Switch {
                        checked: true
                        
                        background: Rectangle {
                            color: "transparent"
                        }
                    }
                }
                
                // Notificaciones del sistema
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingLg
                    
                    Column {
                        Layout.preferredWidth: 200
                        spacing: Theme.spacingXs
                        
                        Text {
                            text: "Sistema"
                            font.pixelSize: Theme.fontSizeMd
                            font.weight: Theme.fontWeightMedium
                            color: Theme.textPrimary
                        }
                        
                        Text {
                            text: "Notificaciones importantes del sistema"
                            font.pixelSize: Theme.fontSizeSm
                            color: Theme.textSecondary
                        }
                    }
                    
                    Switch {
                        checked: true
                        
                        background: Rectangle {
                            color: "transparent"
                        }
                    }
                }
            }
        }
        
        //======================================================================
        // Botones de acción
        //======================================================================
        Row {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignRight
            spacing: Theme.spacingMd
            
            Button {
                text: "Restablecer valores"
                flat: true
                
                background: Rectangle {
                    color: parent.hovered ? Theme.surfaceHover : "transparent"
                    radius: Theme.radiusMd
                }
                
                contentItem: Text {
                    text: parent.text
                    color: Theme.textSecondary
                    font.pixelSize: Theme.fontSizeMd
                }
            }
            
            Button {
                text: "Guardar cambios"
                
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
                    // Lógica para guardar configuración
                    console.log("Configuración guardada")
                }
            }
        }
    }
}
