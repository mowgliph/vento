//==============================================================================
// Theme.qml
// Vento - Sistema de diseño / Design Tokens
// Estilo: Windows 11 / Fluent Design
//==============================================================================
pragma Singleton
import QtQuick

QtObject {
    id: theme
    
    //==========================================================================
    // Colores - Paleta Principal
    //==========================================================================
    readonly property color primary: "#0078D4"           // Azul Windows 11
    readonly property color primaryLight: "#47A7F5"
    readonly property color primaryDark: "#005A9E"
    readonly property color primaryContainer: "#E6F2FB"
    
    readonly property color secondary: "#6B6B6B"
    readonly property color secondaryLight: "#9E9E9E"
    readonly property color secondaryDark: "#424242"
    
    //==========================================================================
    // Colores - Estados
    //==========================================================================
    readonly property color success: "#107C10"
    readonly property color successLight: "#DFF6DD"
    readonly property color successDark: "#0B5C0B"
    
    readonly property color warning: "#FF8C00"
    readonly property color warningLight: "#FFF4CE"
    readonly property color warningDark: "#CC7000"
    
    readonly property color error: "#D13438"
    readonly property color errorLight: "#FDE7E9"
    readonly property color errorDark: "#A52A2D"
    
    readonly property color info: "#0078D4"
    readonly property color infoLight: "#E6F2FB"
    
    //==========================================================================
    // Colores - Superficies
    //==========================================================================
    readonly property color background: "#F3F3F3"
    readonly property color surface: "#FFFFFF"
    readonly property color surfaceVariant: "#FAFAFA"
    readonly property color surfaceHover: "#F5F5F5"
    readonly property color surfacePressed: "#EBEBEB"
    
    readonly property color sidebar: "#202020"
    readonly property color sidebarHover: "#2D2D2D"
    readonly property color sidebarActive: "#0078D4"
    
    readonly property color card: "#FFFFFF"
    readonly property color cardHover: "#F8F9FA"
    readonly property color cardBorder: "#E5E5E5"
    
    //==========================================================================
    // Colores - Texto
    //==========================================================================
    readonly property color textPrimary: "#1A1A1A"
    readonly property color textSecondary: "#6B6B6B"
    readonly property color textDisabled: "#A0A0A0"
    readonly property color textOnPrimary: "#FFFFFF"
    readonly property color textOnSidebar: "#FFFFFF"
    readonly property color textLink: "#0078D4"
    
    //==========================================================================
    // Colores - Bordes y Divisores
    //==========================================================================
    readonly property color border: "#E5E5E5"
    readonly property color borderLight: "#F0F0F0"
    readonly property color borderFocus: "#0078D4"
    readonly property color divider: "#E5E5E5"
    
    //==========================================================================
    // Tipografía
    //==========================================================================
    readonly property string fontFamily: "Segoe UI, system-ui, sans-serif"
    
    readonly property int fontSizeXs: 10
    readonly property int fontSizeSm: 12
    readonly property int fontSizeMd: 14
    readonly property int fontSizeLg: 16
    readonly property int fontSizeXl: 20
    readonly property int fontSizeXxl: 24
    readonly property int fontSizeDisplay: 32
    
    readonly property int fontWeightNormal: Font.Normal
    readonly property int fontWeightMedium: Font.Medium
    readonly property int fontWeightSemiBold: Font.DemiBold
    readonly property int fontWeightBold: Font.Bold
    
    //==========================================================================
    // Espaciado
    //==========================================================================
    readonly property int spacingXs: 4
    readonly property int spacingSm: 8
    readonly property int spacingMd: 12
    readonly property int spacingLg: 16
    readonly property int spacingXl: 24
    readonly property int spacingXxl: 32
    
    //==========================================================================
    // Bordes Redondeados
    //==========================================================================
    readonly property int radiusXs: 4
    readonly property int radiusSm: 6
    readonly property int radiusMd: 8
    readonly property int radiusLg: 12
    readonly property int radiusXl: 16
    readonly property int radiusFull: 9999
    
    //==========================================================================
    // Sombras
    //==========================================================================
    readonly property color shadowColor: "#20000000"
    readonly property int shadowRadius: 8
    readonly property int shadowOffset: 2
    
    //==========================================================================
    // Animaciones
    //==========================================================================
    readonly property int animationFast: 100
    readonly property int animationNormal: 200
    readonly property int animationSlow: 300
    
    //==========================================================================
    // Dimensiones
    //==========================================================================
    readonly property int sidebarWidth: 240
    readonly property int sidebarCollapsedWidth: 60
    readonly property int headerHeight: 56
    readonly property int footerHeight: 32
    readonly property int fabSize: 56
    readonly property int iconSizeSm: 16
    readonly property int iconSizeMd: 20
    readonly property int iconSizeLg: 24
    
    //==========================================================================
    // Funciones de ayuda
    //==========================================================================
    function alpha(color, alpha) {
        return Qt.rgba(color.r, color.g, color.b, alpha)
    }
    
    function darken(color, factor) {
        return Qt.darker(color, 1 + factor)
    }
    
    function lighten(color, factor) {
        return Qt.lighter(color, 1 + factor)
    }
}
