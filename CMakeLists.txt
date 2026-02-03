cmake_minimum_required(VERSION 3.16)
project(Vaultorix VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED Core Gui Widgets Svg Network)

add_executable(Vaultorix
    # User Interface

    # Windows
    src/ui/windows/window/Window.cpp
    src/ui/windows/subWindow/SubWindow.cpp

    # Vault Window
    src/ui/vault/vaultWindow/VaultWindow.cpp
    ## Toolbar
    src/ui/vault/toolbar/Toolbar.cpp
    ## User
    src/ui/vault/user/User.cpp
    ## Empty State Widget
    src/ui/vault/emptyStateWidget/EmptyStateWidget.cpp
    ## View 
    src/ui/vault/view/View.cpp
    ## Statusbar
    src/ui/vault/statusbar/Statusbar.cpp

    # Account Window
    src/ui/account/accountWindow/AccountWindow.cpp
    
    # Account Create
    src/ui/account/accountCreate/AccountCreate.cpp
    
    # Account OTP
    src/ui/account/accountOTP/AccountOTP.cpp

    # Account Sign In
    src/ui/account/accountSignIn/AccountSignIn.cpp

    # Terms & Conditions
    src/ui/dialogs/termsConditions/TermsConditions.cpp

    # Error Dialogs
    src/ui/dialogs/errorDialog/ErrorDialog.cpp

    # Pwd Validator
    src/ui/account/pwdRulesWidget/pwdRulesWidget.cpp
    
    # UI Components
    src/ui/components/Button.cpp
    src/ui/components/CheckBox.cpp
    src/ui/components/ComboBox.cpp
    src/ui/components/Delegate.cpp
    src/ui/components/Dialog.cpp
    src/ui/components/LineProgress.cpp
    src/ui/components/Menu.cpp
    src/ui/components/RoundedBox.cpp
    src/ui/components/ScrollBar.cpp
    src/ui/components/SmoothOpacity.cpp
    src/ui/components/SmoothShadow.cpp
    src/ui/components/SpinnerProgress.cpp
    src/ui/components/TextField.cpp
    src/ui/components/Toggle.cpp
    src/ui/components/ToolTip.cpp
    src/ui/components/Seperator.cpp
    src/ui/components/Label.cpp
    src/ui/components/Text.cpp
    src/ui/components/Popup.cpp
    src/ui/components/ButtonMenu.cpp
    src/ui/components/ViewModeToggle.cpp

    # Core
    # Validators
    src/core/account/validators/emailValidator/EmailValidator.cpp
    src/core/account/validators/pwdValidator/PwdValidator.cpp
    src/core/account/validators/OTPValidator/OTPValidator.cpp
    src/core/account/validators/usernameValidator/UsernameValidator.cpp
    src/core/account/validators/validatorUtils/ValidatorUtils.cpp
    src/core/account/validators/nameValidator/NameValidator.cpp

    ## Coordinator
    src/core/account/accountAuthCoordinator/AccountAuthCoordinator.cpp

    ## Managers
    src/core/account/accountCreateManager/AccountCreateManager.cpp
    src/core/account/accountSignInManager/AccountSignInManager.cpp
    
    ## Device Info
    src/core/account/deviceInfo/DeviceInfo.cpp

    # Entry Point of App
    src/main.cpp

    # Resources
    src/resources/Icons.h
    src/resources/IconManager.cpp
    src/resources/Illustrations.h
    src/resources/IllustrationManager.cpp
    src/resources/resources.qrc
)

target_link_libraries(Vaultorix PRIVATE Qt6::Core Qt6::Network Qt6::Gui Qt6::Svg Qt6::Widgets dwmapi user32)
set_target_properties(Vaultorix PROPERTIES WIN32_EXECUTABLE TRUE)