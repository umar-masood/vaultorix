#pragma once

#include <QCloseEvent>
#include <QPointer>

#include "../../windows/subwindow/Subwindow.h"
#include "../../../core/services/account_settings/AccountSettingsService.h"
#include "../../../core/validators/auth/UsernameValidator.h"
#include "../../../core/validators/auth/PasswordValidator.h"

class Label;
class Button;
class Toggle;
class Seperator;
class TextField;
class ScrollBar;
class ComboBox;
class ScrollArea;
class QVBoxLayout;
class QHBoxLayout;
class QTimer;

namespace Ui::Utils { 
    class CustomTextField; 
};

namespace Ui::Vault {
    class AccountSettings : public SubWindow {
        Q_OBJECT

        public:
        explicit AccountSettings(QWidget *parent = nullptr);
            
        protected:
        void closeEvent(QCloseEvent *event) override;

        private slots:
        void onUsernameUpdated();
        void onPasswordUpdated();
        void onAccountDeleted();

        void onUploadPictureButtonClicked();
        void onDeletePictureButtonClicked();
        void onDeleteAccountButtonClicked();
        void onChangeUsernameButtonClicked();
        void onEnableChangeUsernameButtonClicked();
        void onChangePasswordButtonClicked();
        void onAccountSettingsClosed();
        void onProfilePictureDeleted();

        void onUsernameAvailable(bool isAvailable);
        void onPasswordValidationUpdated(const PasswordValidator::PasswordValidationResult &result);
        
        void onStartLockTimeoutTimer(int msec);

        void onFailedToUpdatePassword(Core::AccountSettingsService::Error failure);
        void onFailedToDeleteAccount(Core::AccountSettingsService::Error failure);
        void onFailedToUpdateProfilePicture(Core::AccountSettingsService::Error failure);
        void onFailedToDeleteProfilePicture(Core::AccountSettingsService::Error failure);
        void onFailedToUpdateUsername(Core::AccountSettingsService::Error failure);

        signals:
        void accountSettingsClosed();
        void startLockTimeoutTimer(int msec);

        private:
        QString _profilePicturePath;
        
        void setDarkMode(bool isDarkMode);        
        void setUserDetailsFromSessionManager();
        void onProfilePictureUpdated();

        Core::AccountSettingsService *account_settings_core = nullptr;
        QTimer *lockTimer;
        // Window Title Label
        Label *winTitle = nullptr;

        // Scrollbar
        QPointer<ScrollBar> scrollbar = nullptr;

        // Main Header Layout
        QVBoxLayout *main_header_layout = nullptr;
        // Header
        Label *mainHeader = nullptr;
        // Text Main Header
        Label *main_header_text = nullptr;

        // Profile Picture HLayout
        QHBoxLayout *profile_pic_layout = nullptr;
            // Avator
            Label *pic = nullptr;
            // Labels VLayout
            QVBoxLayout *profile_pic_labels_layout = nullptr;
                Label *profile_pic_title = nullptr;
                Label *profile_pic_hint = nullptr;
            // Upload Profile Image Button
            Button *upload_pic_btn = nullptr;
            Button *delete_pic_btn = nullptr;

        // Name VLayout
        QVBoxLayout *name_layout = nullptr;
        Label *name_header = nullptr;
        TextField *name_field = nullptr;

        // ------- Seperator ---------------
        Seperator *sep_1 = nullptr;

        // Username HLayout
        QVBoxLayout *username_layout = nullptr;
                // Username Header
                Label *username_header = nullptr;
                // Field & Button Layout
                QHBoxLayout *field_button_layout = nullptr;
                    // Username TextField
                    Ui::Utils::CustomTextField *username_field = nullptr;
                    // Enable change username hyperlink button
                    Button *enable_username_change_btn = nullptr;
                    // Change button
                    Button *change_username_btn = nullptr;
                    // Username Note
                    Label *username_note = nullptr;
                    // Username Validator
                    UsernameValidator *username_validator = nullptr;
                    // Timer
                    QTimer *usernameTimer = nullptr;
                    
        // ------- Seperator ---------------
        Seperator *sep_2 = nullptr;

        // Email-Address HLayout
        QVBoxLayout *email_layout = nullptr;
            // Email Header
            Label *email_header = nullptr;
                // Field and Status Layout
                QHBoxLayout *field_status_layout = nullptr;
                    // Email TextField
                    TextField *email_field = nullptr;   
                    // Email Verification Status Label
                    Label *email_status = nullptr;
            // Email Note
            Label *email_note = nullptr;

        // ------- Seperator ---------------
        Seperator *sep_3 = nullptr;

        // Password VLayout
        QVBoxLayout *password_layout = nullptr;
            // Password Header
            Label *password_header = nullptr;
            // Password Text
            Label *password_text = nullptr;
            // Current Password TextField
            TextField *curr_password_field = nullptr;
            // New Password + Change Button Layout
            QHBoxLayout *new_password_field_change_button_layout = nullptr;
                // New Password TextField
                Ui::Utils::CustomTextField *new_password_field = nullptr;
                // Change Password Button
                Button *change_password_btn = nullptr;
                // Password Validator
                PasswordValidator *password_validator = nullptr;
                // Timer
                QTimer *passwordTimer = nullptr;
            // Password Note
            Label *password_note = nullptr;
    
        // ------- Seperator ---------------
        Seperator *sep_4 = nullptr;

        // Two Factor Authentication HLayout
        QHBoxLayout *two_fa_layout = nullptr;
            // Labels VLayout
            QVBoxLayout *two_fa_sublayout = nullptr;
                // Header
                Label *two_fa_header = nullptr;
                // Text
                Label *two_fa_text = nullptr;
            // Toggle
            Toggle *two_fa_toggle = nullptr;

        // ------- Seperator ---------------
        Seperator *sep_5 = nullptr;

        // Delete Account HLayout
        QHBoxLayout *delete_acc_layout = nullptr;
            // Labels VLayout
            QVBoxLayout *delete_acc_subLayout = nullptr;
                // Header 
                Label *delete_acc_header = nullptr;
                //Text
                Label *delete_acc_text = nullptr;
            // Delete Account Button
            Button *delete_acc_btn = nullptr;
        
        // ------- Seperator ---------------
        Seperator *sep_6 = nullptr;

        // Lock Timeout Option
        // Icon
        Label *lock_timeout_icon = nullptr;
            // Header
            Label *lock_timeout_header = nullptr;
            // Subtext
            Label *lock_timeout_subText = nullptr;
        // Combo Box
        ComboBox *lock_timeout_combobox = nullptr;
    };  
};