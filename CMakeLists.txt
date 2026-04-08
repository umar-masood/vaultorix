cmake_minimum_required(VERSION 3.16)
project(Vaultorix VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED Core Gui Widgets Svg Network)

add_executable(Vaultorix
    # ------------------------------------------------------------
    #                     USER INTERFACE
    # ------------------------------------------------------------

    # Windows
    src/ui/windows/window/Window.cpp
    src/ui/windows/subWindow/SubWindow.cpp

    # ----------------- MAIN APP VAULT ---------------------------
    # Vault Window
    src/ui/vault/vault_window/VaultWindow.cpp

    ## Toolbar
    src/ui/vault/toolbar/Toolbar.cpp

    ## User
    src/ui/vault/user/User.cpp
    ### User Menu
    src/ui/vault/userMenu/UserMenu.cpp
    #### Account Settings
    src/ui/vault/account_settings/AccountSettings.cpp

    ## Empty State Widget
    src/ui/vault/empty_state/EmptyState.cpp

    ## View 
    src/ui/vault/view/View.cpp
    ### View Delegate
    src/ui/vault/view/ViewDelegate.cpp

    ## Statusbar
    src/ui/vault/statusbar/Statusbar.cpp

    ## About
    src/ui/vault/about/About.cpp
    
    ## Update
    src/ui/vault/update/Update.cpp
    
    ## App Preferences
    src/ui/vault/preferences/Preferences.cpp
    
    ## Report a Bug
    ./src/ui/vault/report_bug/ReportBug.cpp

    # ------------ AUTH (SIGN IN, SIGN UP, OTP) -----------
    # Account Window
    src/ui/auth/auth_window/AuthWindow.cpp
    
    ## Account Signup
    src/ui/auth/signup/Signup.cpp
    
    ## Account OTP
    src/ui/auth/otp/Otp.cpp

    ## Account Signin
    src/ui/auth/signin/Signin.cpp

    ## Auth Coordinator
    src/ui/auth/auth_coordinator/AuthCoordinator.cpp

    # ----------------- Dialogs ---------------------------
    # Error Dialogs
    src/ui/dialogs/error_dialog/ErrorDialog.cpp

    ## Terms & Conditions
    src/ui/dialogs/terms_conditions/TermsConditions.cpp

    # ----------------- Compoenents ---------------------------
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
    src/ui/components/ButtonMenu.cpp
    src/ui/components/ViewModeToggle.cpp
    src/ui/components/Popup.cpp
    src/ui/components/TextEdit.cpp

    # ------------------------------------------------------------
    #                        CORE
    # ------------------------------------------------------------

    # Validators
    ## Auth
    src/core/validators/auth/EmailValidator.cpp
    src/core/validators/auth/PasswordValidator.cpp
    src/core/validators/auth/UsernameValidator.cpp
    src/core/validators/auth/NameValidator.cpp

    # Utils
    src/core/utils/Utils.cpp

    ## Services
    ### Auth
    src/core/services/auth/SignupService.cpp
    src/core/services/auth/SigninService.cpp
    src/core/services/auth/OTPService.cpp
    src/core/services/auth/TokenManager.cpp

    ## Device Info
    src/core/system/DeviceInfo.cpp

    ## Config
    src/core/config/APIConfig.h

    ## Theme Manager
    src/core/theme/ThemeManager.cpp
    # ------------------------------------------------------------
    #                   ENTRY POINT & RESOURCES
    # ------------------------------------------------------------

    # Entry Point of App
    src/main.cpp

    # Resources
    resources/Icons.h
    resources/IconManager.cpp
    resources/Illustrations.h
    resources/IllustrationManager.cpp
    resources/resources.qrc
)

target_link_libraries(Vaultorix PRIVATE Qt6::Core Qt6::Network Qt6::Gui Qt6::Svg Qt6::Widgets dwmapi user32)
set_target_properties(Vaultorix PROPERTIES WIN32_EXECUTABLE TRUE)

# ------------------------------------------------------------
#                   EXTERNAL DEPENDENCIES
# ------------------------------------------------------------
add_subdirectory(${PROJECT_SOURCE_DIR}/dependencies/qtkeychain)
target_link_libraries(Vaultorix PRIVATE Qt6::Core QtKeychain)