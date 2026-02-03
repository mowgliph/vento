//==============================================================================
// main.cpp
// Vento - Entry Point (actualizado para usar nuevo DatabaseManager)
//==============================================================================

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QIcon>
#include <QDir>
#include <QStandardPaths>

// Core
#include "core/database/DatabaseManager.hpp"
#include "core/logger/Logger.hpp"
#include "core/utils/Types.hpp"

// Features
#include "features/currency/CurrencyService.hpp"
#include "features/inventory/InventoryService.hpp"
#include "features/sales/SalesService.hpp"
#include "features/reporting/ReportingService.hpp"

//==============================================================================
// Configuración
//==============================================================================
namespace Config {
    constexpr auto APP_NAME = "Vento";
    constexpr auto ORG_NAME = "VentoApp";
    constexpr auto ORG_DOMAIN = "vento.app";
    constexpr auto APP_VERSION = "1.0.0";
}

//==============================================================================
// Obtener ruta de datos de la aplicación
//==============================================================================
QString getAppDataPath() {
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    return dataPath;
}

//==============================================================================
// Configuración del estilo visual
//==============================================================================
void setupApplicationStyle() {
    QQuickStyle::setStyle("Material");
    qputenv("QT_QUICK_CONTROLS_MATERIAL_VARIANT", "Dense");
    qputenv("QT_QUICK_CONTROLS_MATERIAL_THEME", "Light");
    qputenv("QT_QUICK_CONTROLS_MATERIAL_ACCENT", "#0078D4");
    qputenv("QT_QUICK_CONTROLS_MATERIAL_PRIMARY", "#005A9E");
}

//==============================================================================
// Inicialización de servicios
//==============================================================================
bool initializeServices() {
    auto& logger = Vento::Logger::instance();
    auto& db = Vento::DatabaseManager::instance();
    
    // Configurar logger
    logger.setLogLevel(Vento::LogLevel::Debug);
    
    QString logPath = getAppDataPath() + "/logs/vento.log";
    logger.setLogToFile(logPath);
    
    logger.info("═══════════════════════════════════════════════════════");
    logger.info(QString("  Vento v%1 iniciando...").arg(Config::APP_VERSION));
    logger.info("═══════════════════════════════════════════════════════");
    
    // Inicializar base de datos
    QString dbPath = getAppDataPath() + "/vento.db";
    
    Vento::DatabaseConfig dbConfig;
    dbConfig.path = dbPath;
    dbConfig.enableForeignKeys = true;
    dbConfig.enableWAL = true;
    
    auto dbResult = db.initialize(dbConfig);
    if (!dbResult) {
        logger.error(QString("Error de base de datos: %1").arg(dbResult.errorMessage));
        return false;
    }
    
    // Mostrar estadísticas
    auto stats = db.getStatistics();
    logger.info(QString("Base de datos: %1").arg(stats["path"].toString()));
    logger.info(QString("  Productos: %1").arg(stats["productsCount"].toInt()));
    logger.info(QString("  Ventas: %1").arg(stats["salesCount"].toInt()));
    logger.info(QString("  Tamaño: %1 MB").arg(stats["fileSizeMB"].toString()));
    
    // Inicializar features
    Vento::CurrencyService::instance().initialize();
    Vento::InventoryService::instance().initialize();
    Vento::SalesService::instance().initialize();
    Vento::ReportingService::instance().initialize();
    
    logger.info("✓ Todos los servicios inicializados correctamente");
    return true;
}

//==============================================================================
// Registro de tipos QML
//==============================================================================
void registerQmlTypes() {
    // DatabaseManager expuesto para estadísticas
    qmlRegisterSingletonType<Vento::DatabaseManager>(
        "Vento.Core", 1, 0, "Database",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return &Vento::DatabaseManager::instance();
        }
    );
    
    // Currency
    qmlRegisterSingletonType<Vento::CurrencyService>(
        "Vento.Currency", 1, 0, "CurrencyService",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return &Vento::CurrencyService::instance();
        }
    );
    
    // Inventory
    qmlRegisterSingletonType<Vento::InventoryService>(
        "Vento.Inventory", 1, 0, "InventoryService",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return &Vento::InventoryService::instance();
        }
    );
    
    // Sales
    qmlRegisterSingletonType<Vento::SalesService>(
        "Vento.Sales", 1, 0, "SalesService",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return &Vento::SalesService::instance();
        }
    );
    
    // Reporting
    qmlRegisterSingletonType<Vento::ReportingService>(
        "Vento.Reporting", 1, 0, "ReportingService",
        [](QQmlEngine*, QJSEngine*) -> QObject* {
            return &Vento::ReportingService::instance();
        }
    );
}

//==============================================================================
// Main
//==============================================================================
int main(int argc, char *argv[]) {
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough
    );
    
    QGuiApplication app(argc, argv);
    
    app.setApplicationName(Config::APP_NAME);
    app.setOrganizationName(Config::ORG_NAME);
    app.setOrganizationDomain(Config::ORG_DOMAIN);
    app.setApplicationVersion(Config::APP_VERSION);
    app.setWindowIcon(QIcon(":/icons/app_icon.png"));
    
    setupApplicationStyle();
    registerQmlTypes();
    
    if (!initializeServices()) {
        qCritical() << "Error crítico: No se pudieron inicializar los servicios";
        return -1;
    }
    
    QQmlApplicationEngine engine;
    
    engine.addImportPath(":/");
    engine.rootContext()->setContextProperty("appVersion", Config::APP_VERSION);
    engine.rootContext()->setContextProperty("appName", Config::APP_NAME);
    
    const QUrl mainQml(QStringLiteral("qrc:/qml/Main.qml"));
    
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection
    );
    
    engine.load(mainQml);
    
    if (engine.rootObjects().isEmpty()) {
        return -1;
    }
    
    Vento::Logger::instance().info("╔═══════════════════════════════════════╗");
    Vento::Logger::instance().info("║     🌊 Vento iniciado correctamente   ║");
    Vento::Logger::instance().info("╚═══════════════════════════════════════╝");
    
    return app.exec();
}
