#===============================================================================
# src/features/currency/CMakeLists.txt
# Vento - Feature Currency
#===============================================================================

set(CURRENCY_SOURCES
    CurrencyRepository.cpp
    CurrencyService.cpp
)

set(CURRENCY_HEADERS
    CurrencyTypes.hpp
    CurrencyModel.hpp
    CurrencyRepository.hpp
    CurrencyService.hpp
)

#===============================================================================
# Biblioteca
#===============================================================================
add_library(VentoCurrency STATIC
    ${CURRENCY_SOURCES}
    ${CURRENCY_HEADERS}
)

add_library(Vento::Currency ALIAS VentoCurrency)

#===============================================================================
# Configuración
#===============================================================================
target_include_directories(VentoCurrency
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(VentoCurrency
    PUBLIC
        Qt6::Core
        Qt6::Qml
        Vento::Core
)

target_compile_features(VentoCurrency PUBLIC cxx_std_20)
