//==================================================================
// Main.qml
// Vento - Ventana principal modular
//==================================================================

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import Vento.UI
import Vento.Core
import Vento.Currency
import Vento.Inventory
import Vento.Sales

ApplicationWindow {
    id: mainWindow
    
    //==========================================================================
    // Propiedades de la ventana
    //==========================================================================
    width: 1280
    height: 720
    minimumWidth: 800
    minimumHeight: 600
    visible: true
    title: qsTr("Vento - Sistema de Gestión")
    
    //==========================================================================
    // Propiedades personalizadas
    //==========================================================================
    property string currentView: "dashboard"
    property bool sidebarCollapsed: false
    
    //==========================================================================
    // Fondo de la aplicación
    //==========================================================================
    color: Theme.background
    
    //==========================================================================
    // Header de la aplicación
    //==========================================================================
    header: AppHeader {
        id: appHeader
        
        title: {
            switch(currentView) {
                case "dashboard": return "Dashboard"
                case "currency": return "Tipo de Cambio"
                case "inventory": return "Inventario"
                case "sales": return "Ventas"
                case "reports": return "Reportes"
                case "settings": return "Configuración"
                default: return "Vento"
            }
        }
        
        sidebarCollapsed: mainWindow.sidebarCollapsed
        
        onToggleSidebar: {
            mainWindow.sidebarCollapsed = !mainWindow.sidebarCollapsed
        }
        
        onSearchChanged: function(text) {
            console.log("Buscando:", text)
            // Implementar lógica de búsqueda global
        }
        
        onNotificationsClicked: {
            console.log("Notificaciones clickeadas")
            // Implementar panel de notificaciones
        }
        
        onSettingsClicked: {
            currentView = "settings"
        }
    }
    
    //==========================================================================
    // Contenido principal con sidebar
    //==========================================================================
    RowLayout {
        anchors.fill: parent
        spacing: 0
        
        //======================================================================
        // Sidebar de navegación
        //======================================================================
        Sidebar {
            id: sidebar
            
            Layout.preferredHeight: parent.height
            collapsed: mainWindow.sidebarCollapsed
            currentView: mainWindow.currentView
            
            onViewChanged: function(viewName) {
                mainWindow.currentView = viewName
            }
        }
        
        //======================================================================
        // Área principal de contenido
        //======================================================================
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Theme.background
            
            //==================================================================
            // StackLayout para las diferentes vistas
            //==================================================================
            StackLayout {
                anchors.fill: parent
                currentIndex: {
                    switch(currentView) {
                        case "dashboard": return 0
                        case "currency": return 1
                        case "inventory": return 2
                        case "sales": return 3
                        case "reports": return 4
                        case "settings": return 5
                        default: return 0
                    }
                }
                
                // Dashboard View
                DashboardView {
                    id: dashboardView
                }
                
                // Currency View (placeholder)
                Rectangle {
                    color: Theme.background
                    
                    Column {
                        anchors.centerIn: parent
                        spacing: Theme.spacingLg
                        
                        Text {
                            text: "💱"
                            font.pixelSize: 64
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        
                        Text {
                            text: "Vista de Tipo de Cambio"
                            font.pixelSize: Theme.fontSizeXl
                            font.weight: Theme.fontWeightSemiBold
                            color: Theme.textPrimary
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        
                        Text {
                            text: "Gestión de tasas de cambio próximamente"
                            font.pixelSize: Theme.fontSizeMd
                            color: Theme.textSecondary
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
                
                // Inventory View
                InventoryView {
                    id: inventoryView
                }
                
                // Sales View
                SalesView {
                    id: salesView
                }
                
                // Reports View (placeholder)
                Rectangle {
                    color: Theme.background
                    
                    Column {
                        anchors.centerIn: parent
                        spacing: Theme.spacingLg
                        
                        Text {
                            text: "📈"
                            font.pixelSize: 64
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        
                        Text {
                            text: "Vista de Reportes"
                            font.pixelSize: Theme.fontSizeXl
                            font.weight: Theme.fontWeightSemiBold
                            color: Theme.textPrimary
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                        
                        Text {
                            text: "Reportes y análisis próximamente"
                            font.pixelSize: Theme.fontSizeMd
                            color: Theme.textSecondary
                            anchors.horizontalCenter: parent.horizontalCenter
                        }
                    }
                }
                
                // Settings View
                SettingsView {
                    id: settingsView
                }
            }
            
            //==================================================================
            // Overlay para notificaciones (Toast)
            //==================================================================
            Column {
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: Theme.spacingLg
                spacing: Theme.spacingSm
                
                // Toast container
                Repeater {
                    model: []
                    
                    delegate: Toast {
                        width: 300
                        message: model.message || "Notificación de prueba"
                        type: model.type || "info"
                        
                        Component.onCompleted: show()
                    }
                }
            }
        }
    }
    
    //==========================================================================
    // Funciones de utilidad
    //==========================================================================
    function showToast(message, type) {
        // Implementar sistema de notificaciones toast
        console.log("Toast:", message, type)
    }
    
    function showNotification(title, message) {
        // Implementar sistema de notificaciones del sistema
        console.log("Notification:", title, message)
    }
    
    //==========================================================================
    // Conexiones con servicios C++
    //==========================================================================
    Connections {
        target: CurrencyService
        
        function onExchangeRateUpdated(rate) {
            // Actualizar tasa de cambio en el header
            console.log("Tasa actualizada:", rate)
        }
        
        function onErrorOccurred(error) {
            showToast("Error en servicio de divisas: " + error, "error")
        }
    }
    
    Connections {
        target: InventoryService
        
        function onProductAdded(product) {
            showToast("Producto agregado: " + product.name, "success")
        }
        
        function onStockLow(productId, stock) {
            showToast("Stock bajo para producto ID: " + productId, "warning")
        }
        
        function onErrorOccurred(error) {
            showToast("Error en inventario: " + error, "error")
        }
    }
    
    Connections {
        target: SalesService
        
        function onSaleCompleted(sale) {
            showToast("Venta realizada: Bs. " + sale.total.toFixed(2), "success")
        }
        
        function onErrorOccurred(error) {
            showToast("Error en ventas: " + error, "error")
        }
    }
    
    //==========================================================================
    // Inicialización
    //==========================================================================
    Component.onCompleted: {
        console.log("Vento UI inicializado correctamente")
        
        // Cargar configuración inicial
        // Conectar servicios
        // Inicializar datos
    }
}
