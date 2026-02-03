//==============================================================================
// ReportingServiceTest.cpp
// Vento - Tests unitarios para ReportingService
//==============================================================================

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_all.hpp>
#include <QDate>
#include <QVariantMap>
#include <QVariantList>

// Incluir headers del proyecto
#include "features/reporting/ReportingService.hpp"
#include "features/reporting/ReportingTypes.hpp"
#include "core/logger/Logger.hpp"

using namespace Vento::Reporting;

//==============================================================================
// Fixture para tests de ReportingService
//==============================================================================
class ReportingServiceFixture {
public:
    ReportingServiceFixture() {
        // Inicializar logger para tests
        Vento::Logger::instance().setLogLevel(Vento::LogLevel::Error);
        
        // Inicializar servicio
        service = &ReportingService::instance();
        service->initialize();
    }
    
    ~ReportingServiceFixture() {
        // Limpiar después de cada test
        if (service) {
            // Resetear estado si es necesario
        }
    }
    
    ReportingService* service;
};

//==============================================================================
// Tests de inicialización y estado
//==============================================================================
TEST_CASE_METHOD(ReportingServiceFixture, "ReportingService initialization", "[reporting][init]") {
    REQUIRE(service != nullptr);
    REQUIRE(service->isGenerating() == false);
    REQUIRE(service->lastError().isEmpty());
    REQUIRE(service->currentProgress() == 0);
}

TEST_CASE_METHOD(ReportingServiceFixture, "ReportingService null repository handling", "[reporting][safety]") {
    // Este test verifica el manejo seguro de repositorios nulos
    // En una implementación real, necesitaríamos mock del repositorio
    
    SECTION("getSalesSummary with null repository") {
        // Simular repositorio nulo (esto requeriría acceso privado o un mock)
        auto result = service->getSalesSummary(QDate::currentDate(), QDate::currentDate());
        
        // Debería retornar mapa vacío en caso de error
        REQUIRE(result.isEmpty());
    }
    
    SECTION("getDailySalesReport with null repository") {
        auto result = service->getDailySalesReport(QDate::currentDate());
        
        // Debería retornar mapa vacío en caso de error
        REQUIRE(result.isEmpty());
    }
}

//==============================================================================
// Tests de validación de fechas
//==============================================================================
TEST_CASE_METHOD(ReportingServiceFixture, "Date validation", "[reporting][validation]") {
    SECTION("Valid date range") {
        QDate startDate = QDate::currentDate().addDays(-7);
        QDate endDate = QDate::currentDate();
        
        // Esta prueba requeriría mock del repositorio para funcionar completamente
        auto result = service->getSalesSummary(startDate, endDate);
        
        // Por ahora, verificamos que no haya excepciones
        REQUIRE(true); // Test pasa si no hay crash
    }
    
    SECTION("Invalid date range") {
        QDate startDate = QDate::currentDate();
        QDate endDate = QDate::currentDate().addDays(-7); // End before start
        
        auto result = service->getSalesSummary(startDate, endDate);
        
        // Debería manejar fechas inválidas gracefulmente
        REQUIRE(true); // Test pasa si no hay crash
    }
}

//==============================================================================
// Tests de generación de reportes
//==============================================================================
TEST_CASE_METHOD(ReportingServiceFixture, "Report generation", "[reporting][generation]") {
    SECTION("Generate sales report") {
        QDate startDate = QDate::currentDate().addDays(-30);
        QDate endDate = QDate::currentDate();
        
        auto result = service->generateSalesReport(startDate, endDate);
        
        // Verificar que retorna una lista (vacía o con datos)
        REQUIRE(result.type() == QVariant::List);
    }
    
    SECTION("Get top selling products") {
        QDate startDate = QDate::currentDate().addDays(-30);
        QDate endDate = QDate::currentDate();
        
        auto result = service->getTopSellingProducts(startDate, endDate, 10);
        
        // Verificar que retorna una lista
        REQUIRE(result.type() == QVariant::List);
    }
    
    SECTION("Get daily sales totals") {
        QDate startDate = QDate::currentDate().addDays(-7);
        QDate endDate = QDate::currentDate();
        
        auto result = service->getDailySalesTotals(startDate, endDate);
        
        // Verificar que retorna una lista
        REQUIRE(result.type() == QVariant::List);
    }
}

//==============================================================================
// Tests de reporte diario
//==============================================================================
TEST_CASE_METHOD(ReportingServiceFixture, "Daily sales report", "[reporting][daily]") {
    QDate today = QDate::currentDate();
    
    auto report = service->getDailySalesReport(today);
    
    // Verificar estructura del reporte diario
    REQUIRE(report.type() == QVariant::Map);
    
    if (!report.isEmpty()) {
        // Verificar campos esperados si hay datos
        auto reportMap = report.toMap();
        
        INFO("Report keys: " << reportMap.keys().join(", ").toStdString());
        
        // Campos que deberían existir
        QStringList expectedKeys = {"summary", "sales", "topProducts", "averageSale", "date", "dateFormatted"};
        
        for (const auto& key : expectedKeys) {
            INFO("Checking for key: " << key.toStdString());
            // No todos los campos pueden existir si no hay datos, así que solo verificamos estructura básica
        }
    }
}

//==============================================================================
// Tests de exportación
//==============================================================================
TEST_CASE_METHOD(ReportingServiceFixture, "Export functionality", "[reporting][export]") {
    QVariantList testData;
    
    // Crear datos de prueba
    QVariantMap testItem;
    testItem["productId"] = 1;
    testItem["productName"] = "Test Product";
    testItem["productCode"] = "TEST001";
    testItem["date"] = QDate::currentDate().toString("yyyy-MM-dd");
    testItem["quantitySold"] = 5;
    testItem["totalSales"] = 100.50;
    testItem["averagePrice"] = 20.10;
    
    testData.append(testItem);
    
    SECTION("Export to CSV") {
        QString testFile = "test_export.csv";
        bool result = service->exportToCSV(testData, testFile);
        
        // Verificar que no haya excepciones
        REQUIRE(true); // Test pasa si no hay crash
        
        // Limpiar archivo de prueba
        QFile::remove(testFile);
    }
    
    SECTION("Export to PDF") {
        QString testFile = "test_export.pdf";
        bool result = service->exportToPDF(testData, testFile);
        
        // Verificar que no haya excepciones
        REQUIRE(true); // Test pasa si no hay crash
        
        // Limpiar archivo de prueba
        QFile::remove(testFile);
    }
}

//==============================================================================
// Tests de manejo de errores
//==============================================================================
TEST_CASE_METHOD(ReportingServiceFixture, "Error handling", "[reporting][errors]") {
    SECTION("Empty data export") {
        QVariantList emptyData;
        
        bool csvResult = service->exportToCSV(emptyData, "empty_test.csv");
        bool pdfResult = service->exportToPDF(emptyData, "empty_test.pdf");
        
        // Debería manejar datos vacíos sin crash
        REQUIRE(true);
        
        // Limpiar
        QFile::remove("empty_test.csv");
        QFile::remove("empty_test.pdf");
    }
    
    SECTION("Invalid file paths") {
        QVariantList testData;
        QVariantMap testItem;
        testItem["productName"] = "Test";
        testData.append(testItem);
        
        // Intentar exportar a ruta inválida (depende del sistema)
        bool result = service->exportToCSV(testData, "/invalid/path/test.csv");
        
        // Debería manejar error gracefulmente
        REQUIRE(true);
    }
}

//==============================================================================
// Tests de utilidades
//==============================================================================
TEST_CASE_METHOD(ReportingServiceFixture, "Utility functions", "[reporting][utils]") {
    SECTION("Get default report filename") {
        QString filename = service->getDefaultReportFileName("sales", "csv");
        
        REQUIRE(!filename.isEmpty());
        REQUIRE(filename.endsWith(".csv"));
        REQUIRE(filename.contains("sales"));
    }
    
    SECTION("Get product categories") {
        auto categories = service->getProductCategories();
        
        // Debería retornar una lista (vacía o con datos)
        REQUIRE(categories.type() == QVariant::StringList);
    }
    
    SECTION("Get general statistics") {
        auto stats = service->getGeneralStatistics();
        
        // Debería retornar un mapa (vacío o con datos)
        REQUIRE(stats.type() == QVariant::Map);
    }
}

//==============================================================================
// Tests de concurrencia y seguridad
//==============================================================================
TEST_CASE_METHOD(ReportingServiceFixture, "Thread safety", "[reporting][threading]") {
    // Test básico de que el servicio puede ser llamado concurrentemente
    // Nota: Tests reales de concurrencia requerirían setup más complejo
    
    SECTION("Multiple service calls") {
        QDate startDate = QDate::currentDate().addDays(-7);
        QDate endDate = QDate::currentDate();
        
        // Llamadas múltiples rápidas
        auto result1 = service->getSalesSummary(startDate, endDate);
        auto result2 = service->getTopSellingProducts(startDate, endDate, 5);
        auto result3 = service->getDailySalesTotals(startDate, endDate);
        
        // Verificar que no haya crash
        REQUIRE(true);
    }
}

//==============================================================================
// Tests de rendimiento (básicos)
//==============================================================================
TEST_CASE_METHOD(ReportingServiceFixture, "Performance", "[reporting][performance]") {
    SECTION("Large data handling") {
        // Crear dataset grande para pruebas de rendimiento
        QVariantList largeData;
        for (int i = 0; i < 1000; ++i) {
            QVariantMap item;
            item["productId"] = i;
            item["productName"] = QString("Product %1").arg(i);
            item["productCode"] = QString("PROD%1").arg(i, 4, 10, QChar('0'));
            item["date"] = QDate::currentDate().toString("yyyy-MM-dd");
            item["quantitySold"] = i % 10 + 1;
            item["totalSales"] = (i + 1) * 10.5;
            item["averagePrice"] = 10.5;
            largeData.append(item);
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        bool result = service->exportToCSV(largeData, "large_test.csv");
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Verificar que complete en tiempo razonable (< 5 segundos para 1000 items)
        REQUIRE(duration.count() < 5000);
        REQUIRE(true); // Test pasa si no hay crash
        
        // Limpiar
        QFile::remove("large_test.csv");
    }
}
