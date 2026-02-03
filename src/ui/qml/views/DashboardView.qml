//==================================================================
// DashboardView.qml
// Vento - Vista principal del dashboard
//==================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Vento.UI
import Vento.Core

Flickable {
    id: dashboardView
    
    //==========================================================================
    // Propiedades
    //==========================================================================
    property bool loading: false
    
    contentWidth: width
    contentHeight: mainColumn.implicitHeight
    clip: true
    
    ScrollBar.vertical: ScrollBar {
        active: dashboardView.moving
        policy: ScrollBar.AsNeeded
    }
    
    //==========================================================================
    // Contenido principal
    //==========================================================================
    ColumnLayout {
        id: mainColumn
        
        anchors.fill: parent
        anchors.margins: Theme.spacingXl
        spacing: Theme.spacingXl
        
        //======================================================================
        // Encabezado del dashboard
        //======================================================================
        Row {
            Layout.fillWidth: true
            spacing: Theme.spacingLg
            
            Column {
                spacing: Theme.spacingXs
                
                Text {
                    text: "📊 Dashboard"
                    font.pixelSize: Theme.fontSizeDisplay
                    font.weight: Theme.fontWeightBold
                    color: Theme.textPrimary
                }
                
                Text {
                    text: "Bienvenido de nuevo! Aquí está el resumen de tu negocio."
                    font.pixelSize: Theme.fontSizeMd
                    color: Theme.textSecondary
                }
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: "🔄 Actualizar"
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
                
                onClicked: refreshData()
            }
        }
        
        //======================================================================
        // Tarjetas de estadísticas
        //======================================================================
        GridLayout {
            Layout.fillWidth: true
            columns: 4
            columnSpacing: Theme.spacingLg
            rowSpacing: Theme.spacingLg
            
            StatCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 140
                
                title: "Ventas Hoy"
                value: "Bs. 2,450"
                icon: "💰"
                iconColor: Theme.success
                valueColor: Theme.success
                loading: dashboardView.loading
            }
            
            StatCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 140
                
                title: "Productos"
                value: "156"
                icon: "📦"
                iconColor: Theme.primary
                loading: dashboardView.loading
            }
            
            StatCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 140
                
                title: "Pedidos"
                value: "23"
                icon: "🛒"
                iconColor: Theme.warning
                loading: dashboardView.loading
            }
            
            StatCard {
                Layout.fillWidth: true
                Layout.preferredHeight: 140
                
                title: "Ingresos Mes"
                value: "Bs. 45,200"
                icon: "📈"
                iconColor: Theme.primaryDark
                loading: dashboardView.loading
            }
        }
        
        //======================================================================
        // Sección de gráficos y actividades
        //======================================================================
        RowLayout {
            Layout.fillWidth: true
            spacing: Theme.spacingXl
            
            //==================================================================
            // Gráfico de ventas
            //==================================================================
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 300
                Layout.minimumWidth: 400
                
                color: Theme.surface
                border.color: Theme.cardBorder
                border.width: 1
                radius: Theme.radiusLg
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingLg
                    spacing: Theme.spacingMd
                    
                    Text {
                        text: "📈 Ventas de la semana"
                        font.pixelSize: Theme.fontSizeLg
                        font.weight: Theme.fontWeightSemiBold
                        color: Theme.textPrimary
                    }
                    
                    // Placeholder para gráfico real
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: Theme.background
                        radius: Theme.radiusMd
                        
                        Text {
                            anchors.centerIn: parent
                            text: "📊 Gráfico de ventas próximamente"
                            color: Theme.textSecondary
                            font.pixelSize: Theme.fontSizeMd
                        }
                    }
                }
            }
            
            //==================================================================
            // Actividad reciente
            //==================================================================
            Rectangle {
                Layout.preferredWidth: 350
                Layout.fillHeight: true
                
                color: Theme.surface
                border.color: Theme.cardBorder
                border.width: 1
                radius: Theme.radiusLg
                
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: Theme.spacingLg
                    spacing: Theme.spacingMd
                    
                    Text {
                        text: "🕐 Actividad Reciente"
                        font.pixelSize: Theme.fontSizeLg
                        font.weight: Theme.fontWeightSemiBold
                        color: Theme.textPrimary
                    }
                    
                    // Lista de actividades
                    ListView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: Theme.spacingSm
                        
                        model: ListModel {
                            ListElement { icon: "💰"; text: "Venta realizada"; time: "Hace 5 min"; color: "#107C10" }
                            ListElement { icon: "📦"; text: "Producto agregado"; time: "Hace 15 min"; color: "#0078D4" }
                            ListElement { icon: "🛒"; text: "Nuevo pedido"; time: "Hace 1 hora"; color: "#FF8C00" }
                            ListElement { icon: "💱"; text: "Tasa actualizada"; time: "Hace 2 horas"; color: "#6B6B6B" }
                            ListElement { icon: "📊"; text: "Reporte generado"; time: "Hace 3 horas"; color: "#0078D4" }
                        }
                        
                        delegate: Rectangle {
                            width: ListView.view.width
                            height: 60
                            color: "transparent"
                            
                            Row {
                                anchors.fill: parent
                                anchors.leftMargin: Theme.spacingSm
                                spacing: Theme.spacingMd
                                
                                Text {
                                    text: model.icon
                                    font.pixelSize: Theme.fontSizeLg
                                    anchors.verticalCenter: parent.verticalCenter
                                }
                                
                                Column {
                                    anchors.verticalCenter: parent.verticalCenter
                                    spacing: 2
                                    
                                    Text {
                                        text: model.text
                                        font.pixelSize: Theme.fontSizeSm
                                        font.weight: Theme.fontWeightMedium
                                        color: Theme.textPrimary
                                    }
                                    
                                    Text {
                                        text: model.time
                                        font.pixelSize: Theme.fontSizeXs
                                        color: Theme.textSecondary
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        //======================================================================
        // Productos más vendidos
        //======================================================================
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 250
            
            color: Theme.surface
            border.color: Theme.cardBorder
            border.width: 1
            radius: Theme.radiusLg
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: Theme.spacingLg
                spacing: Theme.spacingMd
                
                Row {
                    Layout.fillWidth: true
                    
                    Text {
                        text: "🏆 Productos Más Vendidos"
                        font.pixelSize: Theme.fontSizeLg
                        font.weight: Theme.fontWeightSemiBold
                        color: Theme.textPrimary
                    }
                    
                    Item { width: Theme.spacingLg }
                    
                    Button {
                        text: "Ver todos"
                        flat: true
                        
                        background: Rectangle {
                            color: parent.hovered ? Theme.primaryContainer : "transparent"
                            radius: Theme.radiusSm
                        }
                        
                        contentItem: Text {
                            text: parent.text
                            color: Theme.primary
                            font.pixelSize: Theme.fontSizeSm
                        }
                    }
                }
                
                // Grid de productos populares
                Row {
                    Layout.fillWidth: true
                    spacing: Theme.spacingLg
                    
                    Repeater {
                        model: 4
                        
                        delegate: Rectangle {
                            width: 150
                            height: 120
                            color: Theme.background
                            radius: Theme.radiusMd
                            
                            Column {
                                anchors.centerIn: parent
                                spacing: Theme.spacingXs
                                
                                Text {
                                    text: "📦"
                                    font.pixelSize: 32
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                                
                                Text {
                                    text: "Producto " + (index + 1)
                                    font.pixelSize: Theme.fontSizeSm
                                    font.weight: Theme.fontWeightMedium
                                    color: Theme.textPrimary
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                                
                                Text {
                                    text: "Bs. " + (25.50 + index * 5).toFixed(2)
                                    font.pixelSize: Theme.fontSizeSm
                                    color: Theme.primary
                                    font.weight: Theme.fontWeightBold
                                    anchors.horizontalCenter: parent.horizontalCenter
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    //==========================================================================
    // Funciones
    //==========================================================================
    function refreshData() {
        loading = true
        // Simular carga de datos
        refreshTimer.restart()
    }
    
    Timer {
        id: refreshTimer
        interval: 1000
        onTriggered: loading = false
    }
}
