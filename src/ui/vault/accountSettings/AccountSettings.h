#pragma once

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>

#include "../../windows/subWindow/SubWindow.h"
#include "../../components/Label.h"
#include "../../components/Button.h"
#include "../../components/Toggle.h"
#include "../../components/TextField.h"
#include "../../components/Seperator.h"

class AccountSettingsWindow : public SubWindow {
    Q_OBJECT

    public:
    explicit AccountSettingsWindow(QWidget *parent = nullptr);
    void setDarkMode(bool enable);

    TextField* nameField() const;
    TextField* usernameField() const;
    TextField* emailField() const;
    TextField* currentPasswordField() const;
    TextField* newPasswordField() const;
    
    Toggle* twoFAToggle() const;
    
    Label* emailStatus() const;
    
    Button* saveButton() const;
    Button* deleteAccountButton() const;
    Button* uploadPictureButton() const;
    Button* deletePictureButton() const;
    Button* changeUsernameButton() const;

    private:
    // Theme Mode
    bool isDarkMode = false;

    // Save Button
    Button *save_btn = nullptr;

    // Window Title Label
    Label *winTitle = nullptr;

    // Seperator
    Seperator *titlebar_sep = nullptr;

    // Scrollbar
    ScrollBar *scrollbar = nullptr;

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
                TextField *username_field = nullptr;
                // Change username hyperlink button
                Button *change_username_btn = nullptr;
    
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
        // New Password TextField
        TextField *new_password_field = nullptr;

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
};  