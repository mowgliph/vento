#===============================================================================
# CMakeLists.txt - Vento Tests
#===============================================================================

# Buscar framework de testing
find_package(Catch2 3 QUIET)

if(NOT Catch2_FOUND)
    # Si no está instalado, usar FetchContent para descargarlo
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.5.0
    )
    FetchContent_MakeAvailable(Catch2)
endif()

#===============================================================================
# Tests Unitarios
#===============================================================================

# Test de ReportingService
add_executable(ReportingServiceTest 
    unit/ReportingServiceTest.cpp
    ../src/features/reporting/ReportingService.cpp
    ../src/features/reporting/ReportingRepository.cpp
    ../src/core/logger/Logger.cpp
    ../src/core/utils/FileHandler.cpp
)

target_link_libraries(ReportingServiceTest 
    PRIVATE 
    Catch2::Catch2WithMain
    Qt6::Core
    Qt6::Sql
    Qt6::Test
)

target_include_directories(ReportingServiceTest PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(ReportingServiceTest PRIVATE 
    VENTO_TESTING=1
)

# Registrar test
catch_discover_tests(ReportingServiceTest)

#===============================================================================
# Test de InventoryService
#===============================================================================
add_executable(InventoryServiceTest 
    unit/InventoryServiceTest.cpp
    ../src/features/inventory/InventoryService.cpp
    ../src/features/inventory/ProductRepository.cpp
    ../src/core/logger/Logger.cpp
)

target_link_libraries(InventoryServiceTest 
    PRIVATE 
    Catch2::Catch2WithMain
    Qt6::Core
    Qt6::Sql
    Qt6::Test
)

target_include_directories(InventoryServiceTest PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(InventoryServiceTest PRIVATE 
    VENTO_TESTING=1
)

catch_discover_tests(InventoryServiceTest)

#===============================================================================
# Test de CurrencyService
#===============================================================================
add_executable(CurrencyServiceTest 
    unit/CurrencyServiceTest.cpp
    ../src/features/currency/CurrencyService.cpp
    ../src/features/currency/CurrencyRepository.cpp
    ../src/core/logger/Logger.cpp
)

target_link_libraries(CurrencyServiceTest 
    PRIVATE 
    Catch2::Catch2WithMain
    Qt6::Core
    Qt6::Sql
    Qt6::Test
)

target_include_directories(CurrencyServiceTest PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(CurrencyServiceTest PRIVATE 
    VENTO_TESTING=1
)

catch_discover_tests(CurrencyServiceTest)

#===============================================================================
# Test de Integración
#===============================================================================
add_executable(IntegrationTest 
    integration/DatabaseIntegrationTest.cpp
    ../src/core/database/DatabaseManager.cpp
    ../src/core/logger/Logger.cpp
)

target_link_libraries(IntegrationTest 
    PRIVATE 
    Catch2::Catch2WithMain
    Qt6::Core
    Qt6::Sql
    Qt6::Test
)

target_include_directories(IntegrationTest PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(IntegrationTest PRIVATE 
    VENTO_TESTING=1
)

catch_discover_tests(IntegrationTest)

#===============================================================================
# Configuración de tests
#===============================================================================

# Habilitar testing en CTest
enable_testing()

# Agrupar tests
set_tests_properties(ReportingServiceTest PROPERTIES LABELS "unit")
set_tests_properties(InventoryServiceTest PROPERTIES LABELS "unit")
set_tests_properties(CurrencyServiceTest PROPERTIES LABELS "unit")
set_tests_properties(IntegrationTest PROPERTIES LABELS "integration")

# Configurar timeout para tests (5 minutos)
set_tests_properties(ReportingServiceTest PROPERTIES TIMEOUT 300)
set_tests_properties(InventoryServiceTest PROPERTIES TIMEOUT 300)
set_tests_properties(CurrencyServiceTest PROPERTIES TIMEOUT 300)
set_tests_properties(IntegrationTest PROPERTIES TIMEOUT 300)

# Variable para base de datos de tests
set(TEST_DATABASE_DIR "${CMAKE_BINARY_DIR}/test_data")
file(MAKE_DIRECTORY ${TEST_DATABASE_DIR})

message(STATUS "Tests configurados correctamente")
message(STATUS "  - Unit tests: ReportingService, InventoryService, CurrencyService")
message(STATUS "  - Integration tests: Database")
message(STATUS "  - Test database: ${TEST_DATABASE_DIR}")
