//==============================================================================
// ReportsView.qml
// Vento - Vista de reportes con tabla y exportación
//==============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs
import Vento.Reporting 1.0

Item {
    id: reportsView
    
    //----------------------------------------------------------------------
    // Propiedades
    //----------------------------------------------------------------------
    property var reportData: []
    property var summaryData: ({})
    property bool isLoading: false
    
    //----------------------------------------------------------------------
    // Conexión al servicio de reportes
    //----------------------------------------------------------------------
    ReportingService {
        id: reportingService
        
        onGeneratingChanged: {
            isLoading = isGenerating
            if (!isGenerating && lastError.length > 0) {
                errorDialog.text = lastError
                errorDialog.open()
            }
        }
        
        onReportGenerated: function(reportType, recordCount) {
            statusLabel.text = "✓ Reporte generado: %1 registros".arg(recordCount)
            clearStatusTimer.start()
        }
        
        onExportCompleted: function(fileName, format) {
            statusLabel.text = "✓ Exportado: %1 (%2)".arg(fileName).arg(format)
            clearStatusTimer.start()
        }
        
        onExportFailed: function(fileName, error) {
            statusLabel.text = "✗ Error exportando: %1".arg(error)
            clearStatusTimer.start()
        }
    }
    
    //----------------------------------------------------------------------
    // Temporizador para limpiar estado
    //----------------------------------------------------------------------
    Timer {
        id: clearStatusTimer
        interval: 5000
        onTriggered: statusLabel.text = ""
    }
    
    //----------------------------------------------------------------------
    // Diálogos
    //----------------------------------------------------------------------
    MessageDialog {
        id: errorDialog
        title: "Error"
        text: ""
        icon: StandardIcon.Critical
    }
    
    FileDialog {
        id: exportDialog
        title: "Guardar reporte como"
        nameFilters: ["Archivos CSV (*.csv)", "Archivos PDF (*.pdf)", "Todos los archivos (*)"]
        onAccepted: {
            var format = selectedFile.name.endsWith(".pdf") ? "PDF" : "CSV"
            if (format === "PDF") {
                reportingService.exportToPDF(reportData, selectedFile.toString().replace("file://", ""))
            } else {
                reportingService.exportToCSV(reportData, selectedFile.toString().replace("file://", ""))
            }
        }
    }
    
    //----------------------------------------------------------------------
    // Layout principal
    //----------------------------------------------------------------------
    ColumnLayout {
        anchors.fill: parent
        spacing: 16
        
        //------------------------------------------------------------------
        // Header de reportes
        //------------------------------------------------------------------
        RowLayout {
            Layout.fillWidth: true
            
            Label {
                text: "📈 Reportes"
                font.pixelSize: 28
                font.weight: Font.Bold
                color: appColors.text
            }
            
            Item { Layout.fillWidth: true }
            
            // Indicador de carga
            BusyIndicator {
                running: isLoading
                visible: isLoading
                Layout.preferredWidth: 24
                Layout.preferredHeight: 24
            }
            
            // Estado de operaciones
            Label {
                id: statusLabel
                font.pixelSize: 14
                color: appColors.textSecondary
            }
        }
        
        //------------------------------------------------------------------
        // Panel de filtros
        //------------------------------------------------------------------
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            radius: 8
            color: appColors.surface
            border.color: "#E0E0E0"
            border.width: 1
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12
                
                Label {
                    text: "Filtros del Reporte"
                    font.pixelSize: 16
                    font.weight: Font.Medium
                    color: appColors.text
                }
                
                RowLayout {
                    Layout.fillWidth: true
                    
                    // Fecha inicio
                    Column {
                        spacing: 4
                        Label {
                            text: "Fecha Inicio:"
                            font.pixelSize: 12
                            color: appColors.textSecondary
                        }
                        TextField {
                            id: startDateField
                            placeholderText: "DD/MM/AAAA"
                            text: new Date().toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
                        }
                    }
                    
                    // Fecha fin
                    Column {
                        spacing: 4
                        Label {
                            text: "Fecha Fin:"
                            font.pixelSize: 12
                            color: appColors.textSecondary
                        }
                        TextField {
                            id: endDateField
                            placeholderText: "DD/MM/AAAA"
                            text: new Date().toLocaleDateString(Qt.locale(), "yyyy-MM-dd")
                        }
                    }
                    
                    Item { Layout.fillWidth: true }
                    
                    // Botón generar
                    Button {
                        text: "🔄 Generar Reporte"
                        Material.background: appColors.primary
                        Material.foreground: "white"
                        onClicked: generateReport()
                        enabled: !isLoading
                    }
                    
                    // Botón exportar
                    Button {
                        text: "💾 Exportar"
                        Material.background: appColors.success
                        Material.foreground: "white"
                        enabled: reportData.length > 0 && !isLoading
                        onClicked: exportMenu.open()
                        
                        Menu {
                            id: exportMenu
                            
                            MenuItem {
                                text: "📊 Exportar a Excel (CSV)"
                                onClicked: {
                                    var fileName = reportingService.getDefaultReportFileName("sales", "csv")
                                    reportingService.exportToExcel(reportData, fileName)
                                }
                            }
                            
                            MenuItem {
                                text: "📄 Exportar a PDF"
                                onClicked: {
                                    var fileName = reportingService.getDefaultReportFileName("sales", "pdf")
                                    reportingService.exportToPDF(reportData, fileName)
                                }
                            }
                            
                            MenuItem {
                                text: "🖨️ Imprimir"
                                onClicked: reportingService.printReport(reportData)
                            }
                        }
                    }
                }
            }
        }
        
        //------------------------------------------------------------------
        // Panel de resumen
        //------------------------------------------------------------------
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            radius: 8
            color: appColors.surface
            border.color: "#E0E0E0"
            border.width: 1
            visible: Object.keys(summaryData).length > 0
            
            RowLayout {
                anchors.fill: parent
                anchors.margins: 16
                
                Repeater {
                    model: [
                        { key: "totalSales", label: "Ventas Totales", format: "d" },
                        { key: "totalRevenue", label: "Ingresos Totales", format: "f" },
                        { key: "totalItems", label: "Items Vendidos", format: "d" },
                        { key: "avgSaleAmount", label: "Venta Promedio", format: "f" }
                    ]
                    
                    delegate: Column {
                        spacing: 4
                        Layout.fillWidth: true
                        
                        Label {
                            text: modelData.label
                            font.pixelSize: 12
                            color: appColors.textSecondary
                        }
                        
                        Label {
                            text: modelData.format === "f" ? 
                                  "Bs. " + Number(summaryData[modelData.key] || 0).toFixed(2) :
                                  Number(summaryData[modelData.key] || 0).toLocaleString()
                            font.pixelSize: 18
                            font.weight: Font.Bold
                            color: appColors.primary
                        }
                    }
                }
            }
        }
        
        //------------------------------------------------------------------
        // Tabla de resultados
        //------------------------------------------------------------------
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 8
            color: appColors.surface
            border.color: "#E0E0E0"
            border.width: 1
            
            ScrollView {
                anchors.fill: parent
                anchors.margins: 1
                
                TableView {
                    id: tableView
                    anchors.fill: parent
                    
                    model: reportData
                    
                    delegate: Rectangle {
                        implicitWidth: 150
                        implicitHeight: 40
                        border.color: "#E0E0E0"
                        border.width: 1
                        
                        Text {
                            anchors.centerIn: parent
                            text: {
                                if (column === 0) return modelData.productName || ""
                                if (column === 1) return modelData.productCode || ""
                                if (column === 2) return modelData.date || ""
                                if (column === 3) return Number(modelData.quantitySold || 0).toLocaleString()
                                if (column === 4) return "Bs. " + Number(modelData.totalSales || 0).toFixed(2)
                                if (column === 5) return "Bs. " + Number(modelData.averagePrice || 0).toFixed(2)
                                return ""
                            }
                            font.pixelSize: 12
                            color: appColors.text
                        }
                    }
                    
                    // Headers
                    Rectangle {
                        z: 2
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 40
                        
                        Row {
                            anchors.fill: parent
                            
                            Repeater {
                                model: ["Producto", "Código", "Fecha", "Cantidad", "Total", "Precio Promedio"]
                                
                                delegate: Rectangle {
                                    width: 150
                                    height: 40
                                    color: appColors.primary
                                    border.color: "#E0E0E0"
                                    border.width: 1
                                    
                                    Text {
                                        anchors.centerIn: parent
                                        text: modelData
                                        color: "white"
                                        font.pixelSize: 12
                                        font.weight: Font.Bold
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            // Mensaje cuando no hay datos
            Label {
                anchors.centerIn: parent
                text: reportData.length === 0 ? "📊 No hay datos para mostrar. Genera un reporte para ver los resultados." : ""
                color: appColors.textSecondary
                font.pixelSize: 14
                visible: reportData.length === 0
            }
        }
    }
    
    //----------------------------------------------------------------------
    // Funciones
    //----------------------------------------------------------------------
    function generateReport() {
        var startDate = new Date(startDateField.text)
        var endDate = new Date(endDateField.text)
        
        if (isNaN(startDate) || isNaN(endDate)) {
            errorDialog.text = "Por favor ingrese fechas válidas"
            errorDialog.open()
            return
        }
        
        if (startDate > endDate) {
            errorDialog.text = "La fecha de inicio no puede ser posterior a la fecha de fin"
            errorDialog.open()
            return
        }
        
        // Generar reporte y resumen
        reportData = reportingService.generateSalesReport(
            startDateField.text,
            endDateField.text
        )
        
        summaryData = reportingService.getSalesSummary(
            startDateField.text,
            endDateField.text
        )
    }
}
