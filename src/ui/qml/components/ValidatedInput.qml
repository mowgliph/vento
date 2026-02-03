//==============================================================================
// ValidatedInput.qml
// Vento - Componente de entrada con validación integrada
//==============================================================================

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../theme/Theme.qml" as Theme

FocusScope {
    id: root
    
    //==========================================================================
    // Propiedades públicas
    //==========================================================================
    property alias text: inputField.text
    property alias placeholderText: inputField.placeholderText
    property alias validator: inputField.validator
    property alias inputMethodHints: inputField.inputMethodHints
    
    property bool isValid: true
    property string errorMessage: ""
    property bool showError: false
    
    property bool required: false
    property string inputType: "text"  // "text", "number", "email", "date"
    property double minValue: -Infinity
    property double maxValue: Infinity
    property int maxLength: 0
    
    //==========================================================================
    // Señales
    //==========================================================================
    signal accepted()
    signal textChanged(string text)
    signal validationChanged(bool isValid, string errorMessage)
    
    //==========================================================================
    // Layout principal
    //==========================================================================
    ColumnLayout {
        anchors.fill: parent
        spacing: Theme.spacingXs
        
        //======================================================================
        // Campo de entrada
        //======================================================================
        TextField {
            id: inputField
            Layout.fillWidth: true
            Layout.preferredHeight: Theme.spacingXl * 2
            
            // Estilo usando Theme
            color: Theme.textPrimary
            font.pixelSize: Theme.fontSizeMd
            font.family: Theme.fontFamily
            
            background: Rectangle {
                radius: Theme.radiusMd
                color: Theme.surface
                border.color: {
                    if (root.showError && !root.isValid) {
                        return Theme.error
                    } else if (inputField.activeFocus) {
                        return Theme.borderFocus
                    } else {
                        return Theme.border
                    }
                }
                border.width: inputField.activeFocus ? 2 : 1
                
                // Efecto de focus
                Behavior on border.color {
                    ColorAnimation { duration: Theme.animationFast }
                }
            }
            
            // Configurar validador según tipo
            Component.onCompleted: setupValidator()
            
            onTextChanged: {
                root.text = text
                validateInput()
                root.textChanged(text)
            }
            
            onAccepted: root.accepted()
            
            // Placeholder styling
            placeholderTextColor: Theme.textSecondary
        }
        
        //======================================================================
        // Mensaje de error
        //======================================================================
        Label {
            id: errorLabel
            Layout.fillWidth: true
            visible: root.showError && !root.isValid && root.errorMessage.length > 0
            
            text: root.errorMessage
            font.pixelSize: Theme.fontSizeXs
            color: Theme.error
            
            // Animación de aparición
            opacity: visible ? 1.0 : 0.0
            Behavior on opacity {
                NumberAnimation { duration: Theme.animationFast }
            }
            
            Layout.preferredHeight: visible ? implicitHeight : 0
            Behavior on Layout.preferredHeight {
                NumberAnimation { duration: Theme.animationFast }
            }
        }
    }
    
    //==========================================================================
    // Funciones de validación
    //==========================================================================
    function setupValidator() {
        switch (root.inputType) {
            case "number":
                inputField.validator = DoubleValidator {
                    bottom: root.minValue
                    top: root.maxValue
                    notation: DoubleValidator.StandardNotation
                }
                inputField.inputMethodHints = Qt.ImhDigitsOnly
                break
                
            case "integer":
                inputField.validator = IntValidator {
                    bottom: root.minValue
                    top: root.maxValue
                }
                inputField.inputMethodHints = Qt.ImhDigitsOnly
                break
                
            case "email":
                // Validación regex para email
                inputField.validator = RegExpValidator {
                    regExp: /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/
                }
                inputField.inputMethodHints = Qt.ImhEmailCharactersOnly
                break
                
            case "date":
                // Validación regex para fecha YYYY-MM-DD
                inputField.validator = RegExpValidator {
                    regExp: /^\d{4}-\d{2}-\d{2}$/
                }
                inputField.inputMethodHints = Qt.ImhDigitsOnly
                inputField.placeholderText = "YYYY-MM-DD"
                break
                
            default:
                // Validación de longitud máxima
                if (root.maxLength > 0) {
                    inputField.validator = RegExpValidator {
                        regExp: new RegExp(`^.{0,${root.maxLength}}$`)
                    }
                }
                break
        }
    }
    
    function validateInput() {
        var wasValid = root.isValid
        var wasError = root.showError
        
        // Resetear estado
        root.isValid = true
        root.errorMessage = ""
        
        // Validación de campo requerido
        if (root.required && inputField.text.trim().length === 0) {
            root.isValid = false
            root.errorMessage = "Este campo es obligatorio"
            root.showError = true
        }
        // Validación de longitud
        else if (root.maxLength > 0 && inputField.text.length > root.maxLength) {
            root.isValid = false
            root.errorMessage = `Máximo ${root.maxLength} caracteres`
            root.showError = true
        }
        // Validación de rango numérico
        else if (root.inputType === "number" || root.inputType === "integer") {
            var numValue = parseFloat(inputField.text)
            if (!isNaN(numValue)) {
                if (numValue < root.minValue) {
                    root.isValid = false
                    root.errorMessage = `Valor mínimo: ${root.minValue}`
                    root.showError = true
                } else if (numValue > root.maxValue) {
                    root.isValid = false
                    root.errorMessage = `Valor máximo: ${root.maxValue}`
                    root.showError = true
                }
            }
        }
        // Validación de email
        else if (root.inputType === "email" && inputField.text.length > 0) {
            var emailRegex = /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/
            if (!emailRegex.test(inputField.text)) {
                root.isValid = false
                root.errorMessage = "Formato de email inválido"
                root.showError = true
            }
        }
        // Validación de fecha
        else if (root.inputType === "date" && inputField.text.length > 0) {
            var dateRegex = /^\d{4}-\d{2}-\d{2}$/
            if (!dateRegex.test(inputField.text)) {
                root.isValid = false
                root.errorMessage = "Formato de fecha inválido (YYYY-MM-DD)"
                root.showError = true
            } else {
                // Validar que la fecha sea válida
                var dateParts = inputField.text.split('-')
                var year = parseInt(dateParts[0])
                var month = parseInt(dateParts[1])
                var day = parseInt(dateParts[2])
                
                var date = new Date(year, month - 1, day)
                if (date.getFullYear() !== year || date.getMonth() !== month - 1 || date.getDate() !== day) {
                    root.isValid = false
                    root.errorMessage = "Fecha inválida"
                    root.showError = true
                }
            }
        }
        
        // Ocultar error si el campo está vacío y no es requerido
        if (!root.required && inputField.text.trim().length === 0) {
            root.showError = false
        }
        
        // Emitir señal de cambio de validación
        if (wasValid !== root.isValid || wasError !== root.showError) {
            root.validationChanged(root.isValid, root.errorMessage)
        }
    }
    
    //==========================================================================
    // Métodos públicos
    //==========================================================================
    function clear() {
        inputField.text = ""
        root.isValid = true
        root.errorMessage = ""
        root.showError = false
    }
    
    function focus() {
        inputField.forceActiveFocus()
    }
    
    function isValidInput() {
        validateInput()
        return root.isValid
    }
}
