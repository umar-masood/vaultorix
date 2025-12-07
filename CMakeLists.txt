cmake_minimum_required(VERSION 3.16)
project(Vaultorix VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED Core Gui Widgets Svg Network)


add_executable(Vaultorix
    # User Interface

    # Windows
    src/ui/windows/subWindow/SubWindow.cpp
    src/ui/windows/subWindow/resources.qrc

    # Account Window
    src/ui/accountWindow/AccountWindow.cpp
    
    # Account Create
    src/ui/accountCreate/AccountCreate.cpp
    
    # Account OTP
    src/ui/accountOTP/AccountOTP.cpp
    src/ui/accountOTP/OTPWidget.cpp

    # Account Sign In
    src/ui/accountSignIn/AccountSignIn.cpp

    # Terms & Conditions
    src/ui/dialogs/termsConditions/TermsConditions.cpp

    # Error Dialogs
    src/ui/dialogs/error/Error.cpp

    # Dialogs Utils
    src/ui/dialogs/dialogsUtils.cpp

    # Pwd Validator
    src/ui/pwdRulesWidget/pwdRulesWidget.cpp
    
    # UI Components
    src/ui/components/Button.cpp
    src/ui/components/CheckBox.cpp
    src/ui/components/ComboBox.cpp
    src/ui/components/Delegate.cpp
    src/ui/components/Dialog.cpp
    src/ui/components/LineProgress.cpp
    src/ui/components/Menu.cpp
    src/ui/components/Rectangle.cpp
    src/ui/components/RoundedBox.cpp
    src/ui/components/ScrollBar.cpp
    src/ui/components/SmoothOpacity.cpp
    src/ui/components/SmoothShadow.cpp
    src/ui/components/SpinnerProgress.cpp
    src/ui/components/Text.cpp
    src/ui/components/TextField.cpp
    src/ui/components/Toggle.cpp
    src/ui/components/ToolTip.cpp
    src/ui/components/Seperator.cpp

    # Core
    src/core/emailValidator/emailValidator.cpp
    src/core/pwdValidator/pwdValidator.cpp
    src/core/OTPValidator/OTPValidator.cpp
    src/core/usernameValidator/usernameValidator.cpp
    src/core/validatorUtils/validatorUtils.cpp

    # Entry Point of App
    src/main.cpp

    # Resources
    src/resources/resources.qrc
)

target_link_libraries(Vaultorix PRIVATE Qt6::Core Qt6::Network Qt6::Gui Qt6::Svg Qt6::Widgets dwmapi user32)
set_target_properties(Vaultorix PROPERTIES WIN32_EXECUTABLE TRUE)