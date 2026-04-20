#include "AccountSettings.h"
#include "../../../../resources/IconManager.h"
#include "../../auth/signup/Signup.h"
#include "../../../core/theme/ThemeManager.h"

#include "../../components/Label.h"
#include "../../components/Button.h"
#include "../../components/Toggle.h"
#include "../../components/TextField.h"
#include "../../components/Seperator.h"

using Ui::Vault::AccountSettings;
/* --------------------------------------------------------------------------
                        ACCOUNT SETTINGS WINDOW 
  -------------------------------------------------------------------------*/
AccountSettings::AccountSettings(QWidget *parent) : SubWindow(QSize(600, 600), parent)
{
    // Window Properties
    setFocusPolicy(Qt::StrongFocus);
    setModal(true);

    // Window Content Area Layout
    auto *win_content_area_layout = new QVBoxLayout(contentArea());
    win_content_area_layout->setSpacing(0);
    win_content_area_layout->setContentsMargins(14, 34, 8, 14); // 8 : for scrollbar placement

    // Main Widget
    auto *mainWidget = new QWidget;
    mainWidget->setAttribute(Qt::WA_TranslucentBackground);
    mainWidget->setFixedWidth(566); // fixed to prevent horizontal scrolling

    // ScrollArea
    auto *scrollArea = new QScrollArea;
    scrollArea->setWidget(mainWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setAttribute(Qt::WA_TranslucentBackground);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ScrollBar
    scrollbar = new ScrollBar(Qt::Vertical);
    scrollArea->setVerticalScrollBar(scrollbar);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Adding scroll area to window content area layout
    win_content_area_layout->addWidget(scrollArea);

    // Content Main Layout
    auto *contentLayout = new QVBoxLayout(mainWidget);
    contentLayout->setSpacing(8);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    // --------------------------- Window Titlebar Layout ---------------------------
    QHBoxLayout *_titlebarLayout = this->titlebarLayout();

    // --------------------------- Window Title ---------------------------
    winTitle = new Label("Segoe UI", 10, QFont::Normal, false, "Account Settings", Qt::AlignLeft);
    _titlebarLayout->addSpacing(6);
    _titlebarLayout->addWidget(winTitle, 0, Qt::AlignLeft | Qt::AlignVCenter);
    _titlebarLayout->addStretch();

    // --------------------------- Save Button ---------------------------
    save_btn = new Button("Save");
    save_btn->setDisplayMode(Button::TextOnly);
    save_btn->setCursor(Qt::PointingHandCursor);
    save_btn->setFixedSize(QSize(50, 26));
    save_btn->setFontXY(0, -1);
    save_btn->setColor(Button::DisabledPrimary, QColor("#6c9cbb"));
    save_btn->setEnabled(false); 

    // Seperator 
    titlebar_sep = new Seperator( 18, 1, Qt::Vertical);

    // Adding Save button and seperator to titlebar layout
    _titlebarLayout->addWidget(save_btn, 0, Qt::AlignRight | Qt::AlignVCenter);
    _titlebarLayout->addSpacing(10);
    _titlebarLayout->addWidget(titlebar_sep, 0, Qt::AlignRight | Qt::AlignVCenter);
    _titlebarLayout->addSpacing(10);

   /* ---------------------------------------------------------------------------------
                                Main Header Section
    ---------------------------------------------------------------------------------*/
    // Layout 
    main_header_layout = new QVBoxLayout;
    main_header_layout->setContentsMargins(0, 0, 0, 0);
    main_header_layout->setSpacing(0);

    // Header
    mainHeader = new Label("Segoe UI", 10, QFont::Normal, false, "Profile Information", Qt::AlignLeft);

    // Text under main header
    main_header_text = new Label("Segoe UI", 10, QFont::Normal, false, "Edit your personal information", Qt::AlignLeft);

    // Adding header, text to its layout
    main_header_layout->addStretch();
    main_header_layout->addWidget(mainHeader, 0, Qt::AlignLeft);
    main_header_layout->addSpacing(2);
    main_header_layout->addWidget(main_header_text, 0, Qt::AlignLeft);
    main_header_layout->addStretch();

    /* ---------------------------------------------------------------------------------
                                Profile Picture Section
    ---------------------------------------------------------------------------------*/
    // Layout
    profile_pic_layout = new QHBoxLayout();
    profile_pic_layout->setContentsMargins(0, 0, 0, 0);
    profile_pic_layout->setSpacing(0);

    // Profile Picture
    pic = new Label(true);
    pic->setFixedSize(QSize(80, 80));
    pic->setPixmap(QPixmap(IconManager::icon(Icons::Avator)).scaled(QSize(80, 80), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Profile Labels Layout
    profile_pic_labels_layout = new QVBoxLayout;
    profile_pic_labels_layout->setContentsMargins(0, 0, 0, 0);
    profile_pic_labels_layout->setSpacing(0);

    // Profile Picture Title
    profile_pic_title = new Label("Segoe UI", 10, QFont::Normal, false, "Profile Picture", Qt::AlignLeft);
    
    // Profile Picture Hint
    profile_pic_hint = new Label("Segoe UI", 10, QFont::Normal, false, "PNG, JPEG, under 1 MB", Qt::AlignLeft);

    // Adding Title and Hint to Labels Layout
    profile_pic_labels_layout->addStretch();
    profile_pic_labels_layout->addWidget(profile_pic_title, 0, Qt::AlignLeft);
    profile_pic_labels_layout->addSpacing(2);
    profile_pic_labels_layout->addWidget(profile_pic_hint, 0, Qt::AlignLeft);
    profile_pic_labels_layout->addStretch();

    // Upload Picture Button
    upload_pic_btn = new Button("Upload Profile Picture");
    upload_pic_btn->setDisplayMode(Button::TextOnly);
    upload_pic_btn->setSecondary(true);
    upload_pic_btn->setFontXY(0, -1);

    // Delete Picture Button
    delete_pic_btn = new Button;
    delete_pic_btn->setFixedSize(QSize(36, 36));
    delete_pic_btn->setIconSize(QSize(18, 18));
    delete_pic_btn->setIconPaths(IconManager::icon(Icons::MenuDelete), IconManager::icon(Icons::MenuDelete));
    delete_pic_btn->setDisplayMode(Button::IconOnly);
    delete_pic_btn->setSecondary(true);

    // Adding Upload and Delete Buttons to Profile Pic Main Layout
    profile_pic_layout->addWidget(pic, 0, Qt::AlignLeft);
    profile_pic_layout->addSpacing(20);
    profile_pic_layout->addLayout(profile_pic_labels_layout);
    profile_pic_layout->setAlignment(profile_pic_labels_layout, Qt::AlignLeft);
    profile_pic_layout->addStretch();
    profile_pic_layout->addWidget(upload_pic_btn, 0, Qt::AlignRight);
    profile_pic_layout->addSpacing(5);
    profile_pic_layout->addWidget(delete_pic_btn, 0, Qt::AlignRight);

    /* ----------------------------------------------------------------------------
                                    Name Section
    -------------------------------------------------------------------------------*/
    // Name Layout
    name_layout = new QVBoxLayout;
    name_layout->setContentsMargins(0, 0, 0, 0);
    name_layout->setSpacing(0);

    // Header
    name_header = new Label("Segoe UI", 10, QFont::Normal, false, "Full Name", Qt::AlignLeft);

    // TextField
    name_field = new TextField("Umar Masood");
    name_field->setReadOnly(true);
    name_field->setFixedSize(QSize(360, 36));

    // Adding Header and TextField to Main Name Layout
    name_layout->addWidget(name_header, 0, Qt::AlignLeft);
    name_layout->addSpacing(6);
    name_layout->addWidget(name_field, 0, Qt::AlignLeft);

    // ============================ Seperator ========================================
    sep_1 = new Seperator( 1, width(), Qt::Horizontal);

    /* ---------------------------------------------------------------------------------
                                    Username Section
     --------------------------------------------------------------------------------*/
    // Username Main Layout
    username_layout = new QVBoxLayout;
    username_layout->setContentsMargins(0, 0, 0, 0);
    username_layout->setSpacing(0);

    // Header
    username_header = new Label("Segoe UI", 10, QFont::Normal, false, "Username", Qt::AlignLeft);

    // TextField & Username change button layout
    field_button_layout = new QHBoxLayout;
    field_button_layout->setContentsMargins(0, 0, 0, 0);
    field_button_layout->setSpacing(0);
    
    // Text Field
    username_field = new CustomTextField(true);
    username_field->setText("umar123");
    username_field->setFixedSize(QSize(360, 36));
    username_field->setPlaceholderText("Enter new username");
    username_field->setReadOnly(true);
    username_field->validityIcon()->hide();
    username_field->setContextMenu(false);

    // Change Username Hyperlink Button
    change_username_btn = new Button("Change Username");
    change_username_btn->setDisplayMode(Button::TextOnly);
    change_username_btn->setFixedSize(QSize(120, 20));
    change_username_btn->setFontProperties("Segoe UI", 10);
    change_username_btn->setHyperLink(true);

    // Signal Slot
    connect(change_username_btn, &Button::clicked, this, [this](){
        username_field->setReadOnly(false);
        username_field->validityIcon()->show();
        username_field->setText("");
        username_field->setFocus();
        change_username_btn->hide();
    });

    // Adding field and button to its main layout
    field_button_layout->addWidget(username_field, 0, Qt::AlignLeft);
    field_button_layout->addSpacing(20);
    field_button_layout->addWidget(change_username_btn, 0, Qt::AlignLeft | Qt::AlignVCenter);

    // Adding SubLayout and Button to Main Layout of Username
    username_layout->addWidget(username_header, 0, Qt::AlignLeft);
    username_layout->addSpacing(6);
    username_layout->addLayout(field_button_layout);
    username_layout->setAlignment(field_button_layout, Qt::AlignLeft);

    // ============================ Seperator ========================================
    sep_2 = new Seperator( 1, width(), Qt::Horizontal);

    /* ---------------------------------------------------------------------------------
                                    Email  Section
     --------------------------------------------------------------------------------*/
    // Email Layout
    email_layout = new QVBoxLayout;
    email_layout->setContentsMargins(0, 0, 0, 0);
    email_layout->setSpacing(0);

    // Email Header
    email_header = new Label("Segoe UI", 10, QFont::Normal, false, "Email-Address", Qt::AlignLeft);

    // TextField and Status Layout
    field_status_layout = new QHBoxLayout;
    field_status_layout->setContentsMargins(0, 0, 0, 0);
    field_status_layout->setSpacing(0);

    // Email TextField
    email_field = new TextField("umarmasood8546@gmail.com");
    email_field->setReadOnly(true);
    email_field->setFocusPolicy(Qt::NoFocus);
    email_field->setFixedSize(QSize(360, 36));
    email_field->setContextMenu(false);

    // Email Status
    email_status = new Label("Segoe UI", 10, QFont::Normal, false, "Verified", Qt::AlignLeft);
    email_status->setTextColor(QColor("#28A745"));

    // Adding field and status to layout
    field_status_layout->addWidget(email_field, 0, Qt::AlignLeft);
    field_status_layout->addSpacing(20);
    field_status_layout->addWidget(email_status, 0, Qt::AlignLeft | Qt::AlignVCenter);

    // Email Note
    email_note = new Label("Segoe UI", 10, QFont::Normal, false, 
                            "Note: This email address is permanent and cannot be modified.", Qt::AlignLeft);
 
    // Adding sublayout and status to its main layout
    email_layout->addWidget(email_header, Qt::AlignLeft);
    email_layout->addSpacing(6);
    email_layout->addLayout(field_status_layout, Qt::AlignLeft);
    email_layout->addSpacing(6);
    email_layout->addWidget(email_note, 0, Qt::AlignLeft);

    // ============================ Seperator ========================================
    sep_3 = new Seperator( 1, width(), Qt::Horizontal);

    /* ---------------------------------------------------------------------------------
                                Password  Section
     --------------------------------------------------------------------------------*/
    // Password Layout
    password_layout = new QVBoxLayout;
    password_layout->setContentsMargins(0, 0, 0, 0);
    password_layout->setSpacing(0);

    // Header
    password_header = new Label("Segoe UI", 10, QFont::Normal, false, "Password", Qt::AlignLeft);

    // Text
    password_text = new Label("Segoe UI", 10, QFont::Normal, false, "Modify your current password", Qt::AlignLeft);

    // Current Password TextField
    curr_password_field = new TextField;
    curr_password_field->setFixedSize(QSize(360, 36));
    curr_password_field->setPasswordMode(true);
    curr_password_field->setPlaceholderText("Enter current password");
    curr_password_field->setContextMenu(false);

    // New Password TextField
    new_password_field = new TextField;
    new_password_field->setFixedSize(QSize(360, 36));
    new_password_field->setPasswordMode(true);
    new_password_field->setPlaceholderText("Enter new password");
    new_password_field->setContextMenu(false);

    // Password Requirements Note
    password_note = new Label("Segoe UI", 10, QFont::Normal, false, 
                            "Note: Password must include at least one uppercase letter, one lowercase letter, one number, and one special character.", Qt::AlignLeft);
    password_note->setWordWrap(true);

    // Adding header, text, textfields to its layout
    password_layout->addWidget(password_header);
    password_layout->addSpacing(2);
    password_layout->addWidget(password_text);
    password_layout->addSpacing(6);
    password_layout->addWidget(curr_password_field);
    password_layout->addSpacing(4);
    password_layout->addWidget(new_password_field);
    password_layout->addSpacing(6);
    password_layout->addWidget(password_note);

    // ============================ Seperator ========================================
    sep_4 = new Seperator( 1, width(), Qt::Horizontal);

    /* ---------------------------------------------------------------------------------
                                2FA  Section
     --------------------------------------------------------------------------------*/
    // 2FA Layout
    two_fa_layout = new QHBoxLayout;
    two_fa_layout->setContentsMargins(0, 0, 0, 0);
    two_fa_layout->setSpacing(0);

    // Sublayout
    two_fa_sublayout = new QVBoxLayout;
    two_fa_sublayout->setContentsMargins(0, 0, 0, 0);
    two_fa_sublayout->setSpacing(0);

    // Header
    two_fa_header = new Label("Segoe UI", 10, QFont::Normal, false, "Two Factor Authentication", Qt::AlignLeft);

    // Text
    two_fa_text = new Label("Segoe UI", 10, QFont::Normal, false, 
                            "Always require two-factor authentication when signing in", Qt::AlignLeft);

    // Adding header and text to sublayout
    two_fa_sublayout->addWidget(two_fa_header);
    two_fa_sublayout->addSpacing(2);
    two_fa_sublayout->addWidget(two_fa_text);

    // Toggle
    two_fa_toggle = new Toggle;

    // Adding Toggle and sublayout to its main layout
    two_fa_layout->addLayout(two_fa_sublayout);
    two_fa_layout->setAlignment(two_fa_sublayout, Qt::AlignLeft);
    two_fa_layout->addStretch();
    two_fa_layout->addWidget(two_fa_toggle, Qt::AlignRight);

    // ============================ Seperator ========================================
    sep_5 = new Seperator( 1, width(), Qt::Horizontal);

    /* ---------------------------------------------------------------------------------
                                Delete Account Section
     --------------------------------------------------------------------------------*/
    // Delete Account Layout
    delete_acc_layout = new QHBoxLayout;
    delete_acc_layout->setContentsMargins(0, 0, 0, 0);
    delete_acc_layout->setSpacing(0);

    // SubLayout
    delete_acc_subLayout = new QVBoxLayout;
    delete_acc_subLayout->setContentsMargins(0, 0, 0, 0);
    delete_acc_subLayout->setSpacing(0);

    // Header
    delete_acc_header = new Label("Segoe UI", 10, QFont::Normal, false, "Delete Account", Qt::AlignLeft);

    // Text
    delete_acc_text = new Label("Segoe UI", 10, QFont::Normal, false, 
                                "Deleting your account is permanent and cannot be undone. All your data, settings, and activity will be permanently removed.", Qt::AlignLeft);
    delete_acc_text->setWordWrap(true);
    delete_acc_text->setMinimumWidth(400);
    delete_acc_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Adding header, text to sublayout
    delete_acc_subLayout->addWidget(delete_acc_header);
    delete_acc_subLayout->addSpacing(2);
    delete_acc_subLayout->addWidget(delete_acc_text);

    // Delete Account Button
    delete_acc_btn = new Button("Delete Account");
    delete_acc_btn->setDisplayMode(Button::TextOnly);
    delete_acc_btn->setFontProperties("Segoe UI", 10);
    delete_acc_btn->setFontXY(0, -1);
    delete_acc_btn->setBorderTransparent(true);
    delete_acc_btn->setColor(Button::PrimaryNormal,    QColor("#F53649"));
    delete_acc_btn->setColor(Button::PrimaryHover,     QColor("#D92C3E"));
    delete_acc_btn->setColor(Button::PrimaryPressed,   QColor("#B81F30"));
    delete_acc_btn->setTextColor(Button::PrimaryText,  QColor("#FFFFFF"));

    // Adding sublayout and button to its main layout
    delete_acc_layout->addLayout(delete_acc_subLayout);
    delete_acc_layout->setAlignment(delete_acc_subLayout, Qt::AlignLeft);
    delete_acc_layout->addStretch();
    delete_acc_layout->addWidget(delete_acc_btn, Qt::AlignRight);

    /* -----------------------------------------------------------------------
                        Adding Layouts to contentLayout
       -----------------------------------------------------------------------*/
    contentLayout->addLayout(main_header_layout);
    contentLayout->addSpacing(15);

    contentLayout->addLayout(profile_pic_layout);
    contentLayout->addSpacing(15);

    contentLayout->addLayout(name_layout);
    contentLayout->addSpacing(8);
    contentLayout->addWidget(sep_1);
    contentLayout->addSpacing(8);

    contentLayout->addLayout(username_layout);
    contentLayout->addSpacing(8);
    contentLayout->addWidget(sep_2);
    contentLayout->addSpacing(8);

    contentLayout->addLayout(email_layout);
    contentLayout->addSpacing(8);
    contentLayout->addWidget(sep_3);
    contentLayout->addSpacing(8);

    contentLayout->addLayout(password_layout);
    contentLayout->addSpacing(8);
    contentLayout->addWidget(sep_4);
    contentLayout->addSpacing(8);

    contentLayout->addLayout(two_fa_layout);
    contentLayout->addSpacing(8);
    contentLayout->addWidget(sep_5);
    contentLayout->addSpacing(8);

    contentLayout->addLayout(delete_acc_layout);

    // Theme
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &AccountSettings::setDarkMode);
    setDarkMode(tm.isDarkMode());
}

void AccountSettings::setDarkMode(bool isDarkMode) {
    // Scrollbar
    scrollbar->setDarkMode(isDarkMode);

    // -------------------- Subtexts --------------------
    for (Label* label : { main_header_text, profile_pic_hint, password_text, two_fa_text, delete_acc_text, email_note, password_note })
        if (label)
            label->setTextColor(isDarkMode ? "#94A3B8" : "#6B7280");

    // -------------------- Headers --------------------
    for (Label* label : {winTitle, mainHeader, name_header, username_header, email_header, password_header, two_fa_header, delete_acc_header, profile_pic_title })
        if (label)
            label->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");

    // -------------------- TextFields --------------------
    for (TextField* field : { name_field, email_field, curr_password_field, new_password_field })
        if (field)
            field->setDarkMode(isDarkMode);

    if (username_field)
        username_field->setDarkMode(isDarkMode);

    // -------------------- Buttons --------------------
    for (Button* btn : { save_btn, upload_pic_btn, delete_pic_btn, change_username_btn})
        if (btn)
            btn->setDarkMode(isDarkMode);

    // -------------------- Toggle --------------------
    if (two_fa_toggle) two_fa_toggle->setDarkMode(isDarkMode);

    // -------------------- Seperators --------------------
    for (Seperator* sep : { sep_1, sep_2, sep_3, sep_4, sep_5 })
        if (sep)
            sep->setColor(isDarkMode ? "#334155" : "#E5E7EB");

    // Base class method
    SubWindow::setDarkMode(isDarkMode);
}

/* --------------------------------------------------------------------------
                                   GETTERS 
  -------------------------------------------------------------------------*/
TextField* AccountSettings::nameField() const { return name_field; }
CustomTextField* AccountSettings::usernameField() const { return username_field; }
TextField* AccountSettings::emailField() const { return email_field; }
TextField* AccountSettings::currentPasswordField() const { return curr_password_field; }
TextField* AccountSettings::newPasswordField() const { return new_password_field; }
Toggle* AccountSettings::twoFAToggle() const { return two_fa_toggle; }
Label* AccountSettings::emailStatus() const { return email_status; }
Button* AccountSettings::saveButton() const { return save_btn; }
Button* AccountSettings::deleteAccountButton() const { return delete_acc_btn; }
Button* AccountSettings::uploadPictureButton() const { return upload_pic_btn; }
Button* AccountSettings::deletePictureButton() const { return delete_pic_btn; }
Button* AccountSettings::changeUsernameButton() const { return change_username_btn; }