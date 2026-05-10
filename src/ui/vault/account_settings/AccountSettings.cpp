#include "AccountSettings.h"

#include "../../../../resources/IconManager.h"
#include "../../../core/theme/ThemeManager.h"
#include "../../../core/config/Constants.h"
#include "../../../core/session/SessionManager.h"

#include "../../components/Label.h"
#include "../../components/Button.h"
#include "../../components/Toggle.h"
#include "../../components/TextField.h"
#include "../../components/Seperator.h"
#include "../../components/ComboBox.h"
#include "../../components/ScrollBar.h"

#include "../../utils/Utils.h"
#include "../../dialogs/confirm_dialog/ConfirmDialog.h"

#include <QFileDialog>
#include <QStandardPaths>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QApplication>

using Ui::Vault::AccountSettings;
using Ui::Utils::CustomTextField;
using Er = Core::AccountSettingsService::Error;

/* ---------------------------------------------------------------------------------
                                ACCOUNT SETTINGS WINDOW 
  --------------------------------------------------------------------------------*/
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
    winTitle = new Label("Segoe UI", 10, QFont::Normal, false, tr("Account Settings"), Qt::AlignLeft);
    _titlebarLayout->addSpacing(6);
    _titlebarLayout->addWidget(winTitle, 0, Qt::AlignLeft | Qt::AlignVCenter);
    _titlebarLayout->addStretch();

   /* ---------------------------------------------------------------------------------
                                Main Header Section
    ---------------------------------------------------------------------------------*/
    // Layout 
    main_header_layout = new QVBoxLayout;
    main_header_layout->setContentsMargins(0, 0, 0, 0);
    main_header_layout->setSpacing(0);

    // Header
    mainHeader = new Label("Segoe UI", 10, QFont::Normal, false, tr("Profile Information"), Qt::AlignLeft);

    // Text under main header
    main_header_text = new Label("Segoe UI", 10, QFont::Normal, false, tr("Edit your personal information"), Qt::AlignLeft);

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
    onDeletePictureButtonClicked(); // for initial
    
    // Profile Labels Layout
    profile_pic_labels_layout = new QVBoxLayout;
    profile_pic_labels_layout->setContentsMargins(0, 0, 0, 0);
    profile_pic_labels_layout->setSpacing(0);

    // Profile Picture Title
    profile_pic_title = new Label("Segoe UI", 10, QFont::Normal, false, tr("Profile Picture"), Qt::AlignLeft);
    
    // Profile Picture Hint
    profile_pic_hint = new Label("Segoe UI", 10, QFont::Normal, false, tr("Must be PNG, JPEG, JPG under 2 MB"), Qt::AlignLeft);

    // Adding Title and Hint to Labels Layout
    profile_pic_labels_layout->addStretch();
    profile_pic_labels_layout->addWidget(profile_pic_title, 0, Qt::AlignLeft);
    profile_pic_labels_layout->addSpacing(2);
    profile_pic_labels_layout->addWidget(profile_pic_hint, 0, Qt::AlignLeft);
    profile_pic_labels_layout->addStretch();

    // Upload Picture Button
    upload_pic_btn = new Button(tr("Upload Profile Picture"));
    upload_pic_btn->setFixedSize(QSize(160, 36));
    upload_pic_btn->setDisplayMode(Button::TextOnly);
    upload_pic_btn->setLoaderButton(true);
    upload_pic_btn->setSecondary(true);
    upload_pic_btn->setFontXY(0, -1);
    connect(upload_pic_btn, &Button::clicked, this, &AccountSettings::onUploadPictureButtonClicked);

    // Delete Picture Button
    delete_pic_btn = new Button;
    delete_pic_btn->setFixedSize(QSize(36, 36));
    delete_pic_btn->setIconSize(QSize(18, 18));
    delete_pic_btn->setIconPaths(IconManager::icon(Icons::MenuDelete), IconManager::icon(Icons::MenuDelete));
    delete_pic_btn->setDisplayMode(Button::IconOnly);
    delete_pic_btn->setSecondary(true);
    delete_pic_btn->hide();
    connect(delete_pic_btn, &Button::clicked, this, &AccountSettings::onDeletePictureButtonClicked);

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
    name_header = new Label("Segoe UI", 10, QFont::Normal, false, tr("Full Name"), Qt::AlignLeft);

    // TextField
    name_field = new TextField;
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
    username_header = new Label("Segoe UI", 10, QFont::Normal, false, tr("Username"), Qt::AlignLeft);

    // TextField & Username change button layout
    field_button_layout = new QHBoxLayout;
    field_button_layout->setContentsMargins(0, 0, 0, 0);
    field_button_layout->setSpacing(0);
    
    // Text Field
    username_field = new CustomTextField(true);
    username_field->setFixedSize(QSize(360, 36));
    username_field->setPlaceholderText(tr("Enter new username"));
    username_field->setReadOnly(true);
    username_field->validityIcon()->hide();
    username_field->setContextMenu(false);
    
    // Username Validator
    username_validator = new UsernameValidator(this);
    connect(username_validator, &UsernameValidator::usernameAvailable, this, &AccountSettings::onUsernameAvailable);
    connect(username_validator, &UsernameValidator::usernameInvalid, this, [this](){
        username_field->setInvalid();
        username_field->setTooltip(
        "Invalid username.\n\n"
        "Please ensure that:\n"
        "• 3-20 characters, starting with a letter\n"
        "• Letters, numbers, . _ - only\n"
        "• Cannot start or end with . _ -\n"
        "• No excessive character repetition"
        );
        change_username_btn->hide();
    });

    usernameTimer = new QTimer(this);
    usernameTimer->setSingleShot(true);
    connect(usernameTimer, &QTimer::timeout, this, [this]() {
        QString text = username_field->text();

        if (text.isEmpty()) {
            username_field->setInvalid();
            username_field->setTooltip("Username is empty.");
            change_username_btn->hide();
            return;
        }

        if (username_validator)
            username_validator->checkUsernameValidityAndAvailability(text);
    });

    connect(username_field, &CustomTextField::textChanged, this, [this]() {
        usernameTimer->stop();

        QString text = username_field->text();
        if (text == SessionManager::instance().username()) {
            username_field->setInvalid();
            username_field->setTooltip("Same username as before");
            change_username_btn->hide();
            return;
        }

        usernameTimer->start(400);
    });

    // Email Note
    username_note = new Label("Segoe UI", 10, QFont::Normal, true, 
                             tr("Once you change your username, you will need to wait 30 days before you can change it again."), 
                             Qt::AlignLeft);

    // Enable username change Hyperlink Button
    enable_username_change_btn = new Button(tr("Change Username"));
    enable_username_change_btn->setDisplayMode(Button::TextOnly);
    enable_username_change_btn->setFixedSize(QSize(120, 20));
    enable_username_change_btn->setFontProperties("Segoe UI", 10);
    enable_username_change_btn->setHyperLink(true);
    enable_username_change_btn->hide();
    connect(enable_username_change_btn, &Button::clicked, this, &AccountSettings::onEnableChangeUsernameButtonClicked);

    // Change username button
    change_username_btn = new Button(tr("Change"));
    change_username_btn->setDisplayMode(Button::TextOnly);
    change_username_btn->setFixedSize(QSize(80, 36));
    change_username_btn->setSecondary(true);
    change_username_btn->setFontProperties("Segoe UI", 10);
    change_username_btn->setLoaderButton(true);
    change_username_btn->hide();
    connect(change_username_btn, &Button::clicked, this, &AccountSettings::onChangeUsernameButtonClicked);
    
    if (SessionManager::instance().allowedToChangeUsername())
        enable_username_change_btn->show();

    // Adding field and button to its main layout
    field_button_layout->addWidget(username_field, 0, Qt::AlignLeft);
    field_button_layout->addSpacing(20);
    field_button_layout->addWidget(enable_username_change_btn, 0, Qt::AlignLeft | Qt::AlignVCenter);
    field_button_layout->addWidget(change_username_btn, 0, Qt::AlignLeft | Qt::AlignVCenter);

    // Adding SubLayout and Button to Main Layout of Username
    username_layout->addWidget(username_header, 0, Qt::AlignLeft);
    username_layout->addSpacing(6);
    username_layout->addLayout(field_button_layout);
    username_layout->setAlignment(field_button_layout, Qt::AlignLeft);
    username_layout->addSpacing(6);
    username_layout->addWidget(username_note, 0, Qt::AlignLeft);

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
    email_header = new Label("Segoe UI", 10, QFont::Normal, false, tr("Email-Address"), Qt::AlignLeft);

    // TextField and Status Layout
    field_status_layout = new QHBoxLayout;
    field_status_layout->setContentsMargins(0, 0, 0, 0);
    field_status_layout->setSpacing(0);

    // Email TextField
    email_field = new TextField;
    email_field->setReadOnly(true);
    email_field->setFocusPolicy(Qt::NoFocus);
    email_field->setFixedSize(QSize(360, 36));
    email_field->setContextMenu(false);

    // Email Status
    email_status = new Label("Segoe UI", 10, QFont::Normal, false, tr("Verified"), Qt::AlignLeft);
    email_status->setTextColor(QColor("#28A745"));

    // Adding field and status to layout
    field_status_layout->addWidget(email_field, 0, Qt::AlignLeft);
    field_status_layout->addSpacing(20);
    field_status_layout->addWidget(email_status, 0, Qt::AlignLeft | Qt::AlignVCenter);

    // Email Note
    email_note = new Label("Segoe UI", 10, QFont::Normal, true, 
                            tr("This email address is permanent and cannot be modified."), Qt::AlignLeft);
 
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
    password_header = new Label("Segoe UI", 10, QFont::Normal, false, tr("Password"), Qt::AlignLeft);

    // Text
    password_text = new Label("Segoe UI", 10, QFont::Normal, false, tr("Modify your current password"), Qt::AlignLeft);

    // Current Password TextField
    curr_password_field = new TextField;
    curr_password_field->setFixedSize(QSize(360, 36));
    curr_password_field->setPasswordMode(true);
    curr_password_field->setPlaceholderText(tr("Enter current password"));
    curr_password_field->setContextMenu(false);
    
    // New Password TextField and Change Button
    new_password_field_change_button_layout = new QHBoxLayout;
    new_password_field_change_button_layout->setContentsMargins(0, 0, 0, 0);
    new_password_field_change_button_layout->setSpacing(0);

    // New Password TextField
    new_password_field = new CustomTextField(true);
    new_password_field->setFixedSize(QSize(360, 36));
    new_password_field->setPasswordMode(true);
    new_password_field->setPlaceholderText(tr("Enter new password"));
    new_password_field->validityIcon()->hide();
    new_password_field->setContextMenu(false);

    // New Password Validator
    password_validator = new PasswordValidator(this);
    connect(password_validator, &PasswordValidator::validationUpdated, this, &AccountSettings::onPasswordValidationUpdated);
    
    passwordTimer = new QTimer(this);
    passwordTimer->setSingleShot(true);

    connect(passwordTimer, &QTimer::timeout, this, [this]() {
        if (!password_validator)
            return;

        QString text = new_password_field->text();

        if (text.isEmpty()) {
            new_password_field->setInvalid();
            return;
        }

        password_validator->checkPasswordValidity(text);
    });

    connect(new_password_field, &CustomTextField::textChanged, this, [this]() {
        passwordTimer->stop();
        passwordTimer->start(400);
    });

    // Change password button
    change_password_btn = new Button(tr("Change"));
    change_password_btn->setDisplayMode(Button::TextOnly);
    change_password_btn->setFixedSize(QSize(80, 36));
    change_password_btn->setSecondary(true);
    change_password_btn->setFontProperties("Segoe UI", 10);
    change_password_btn->setLoaderButton(true);
    change_password_btn->hide();
    connect(change_password_btn, &Button::clicked, this, &AccountSettings::onChangePasswordButtonClicked);
    
    new_password_field_change_button_layout->addWidget(new_password_field, 0, Qt::AlignLeft);
    new_password_field_change_button_layout->addSpacing(20);
    new_password_field_change_button_layout->addWidget(change_password_btn, 0, Qt::AlignLeft | Qt::AlignVCenter);

    // Password Requirements Note
    password_note = new Label("Segoe UI", 10, 
                            QFont::Normal, 
                            true, 
                            tr("Your new password must be greater than eight characters which contain atleast:\n"
                            "• One uppercase character\n"
                            "• One lowercase character\n"
                            "• One special character\n"
                            "• One number")
                            , Qt::AlignLeft);
    password_note->setWordWrap(true);

    // Adding header, text, textfields to its layout
    password_layout->addWidget(password_header);
    password_layout->addSpacing(2);
    password_layout->addWidget(password_text);
    password_layout->addSpacing(6);
    password_layout->addWidget(curr_password_field);
    password_layout->addSpacing(4);
    password_layout->addLayout(new_password_field_change_button_layout);
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
    two_fa_header = new Label("Segoe UI", 10, QFont::Normal, false, tr("Two Factor Authentication"), Qt::AlignLeft);

    // Text
    two_fa_text = new Label("Segoe UI", 10, QFont::Normal, false, 
                            tr("Always require two-factor authentication when signing in"), Qt::AlignLeft);

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
    delete_acc_header = new Label("Segoe UI", 10, QFont::Normal, false, tr("Delete Account"), Qt::AlignLeft);

    // Text
    delete_acc_text = new Label("Segoe UI", 10, QFont::Normal, false, 
                                tr("Deleting your account is permanent and cannot be undone. All your data, settings, and activity will be permanently removed."), Qt::AlignLeft);
    delete_acc_text->setWordWrap(true);
    delete_acc_text->setMinimumWidth(400);
    delete_acc_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Adding header, text to sublayout
    delete_acc_subLayout->addWidget(delete_acc_header);
    delete_acc_subLayout->addSpacing(2);
    delete_acc_subLayout->addWidget(delete_acc_text);

    // Delete Account Button
    delete_acc_btn = new Button(tr("Delete Account"));
    delete_acc_btn->setDisplayMode(Button::TextOnly);
    delete_acc_btn->setFixedSize(QSize(120, 36));
    delete_acc_btn->setLoaderButton(true);
    delete_acc_btn->setFontProperties("Segoe UI", 10);
    delete_acc_btn->setFontXY(0, -1);
    delete_acc_btn->setBorderTransparent(true);
    delete_acc_btn->setColor(Button::PrimaryNormal,    QColor("#F53649"));
    delete_acc_btn->setColor(Button::PrimaryHover,     QColor("#D92C3E"));
    delete_acc_btn->setColor(Button::PrimaryPressed,   QColor("#B81F30"));
    delete_acc_btn->setTextColor(Button::PrimaryText,  QColor("#FFFFFF"));
    delete_acc_btn->setColor(Button::DisabledPrimary,  QColor("#F5A1A8")); 
    connect(delete_acc_btn, &Button::clicked, this, &AccountSettings::onDeleteAccountButtonClicked);

    // Adding sublayout and button to its main layout
    delete_acc_layout->addLayout(delete_acc_subLayout);
    delete_acc_layout->setAlignment(delete_acc_subLayout, Qt::AlignLeft);
    delete_acc_layout->addStretch();
    delete_acc_layout->addWidget(delete_acc_btn, Qt::AlignRight);

    // ============================ Seperator ========================================
    sep_6 = new Seperator( 1, width(), Qt::Horizontal);

    /* -----------------------------------------------------------------------------------------
                            Lock Timeout
    --------------------------------------------------------------------------------------------*/
    // Layout
    auto *lock_timeout_layout = new QHBoxLayout;
    lock_timeout_layout->setContentsMargins(0, 0, 0, 0);
    lock_timeout_layout->setSpacing(0);;

    // Sublayout
    auto *lock_timeout_sublayout = new QVBoxLayout;
    lock_timeout_sublayout->setContentsMargins(0, 0, 0, 0);
    lock_timeout_sublayout->setSpacing(0);

    // Header
    lock_timeout_header = new Label("Segoe UI", 10, QFont::Normal, false, "Auto Lock Timeout", Qt::AlignLeft);
    
    // Subtext
    lock_timeout_subText = new Label("Segoe UI", 10, QFont::Normal, false, "Choose how long Vaultorix waits before locking your vault due to inactivity", Qt::AlignLeft);
    lock_timeout_subText->setWordWrap(true);
    lock_timeout_subText->setMinimumWidth(360);

    // Adding Header and Subtext to lock_timeout_sublayout
    lock_timeout_sublayout->addWidget(lock_timeout_header, 0, Qt::AlignLeft);
    lock_timeout_sublayout->addSpacing(2);
    lock_timeout_sublayout->addWidget(lock_timeout_subText, 0, Qt::AlignLeft);

    // ComboBox
    lock_timeout_combobox = new ComboBox;
    lock_timeout_combobox->setFieldSize(QSize(160, 36));
    lock_timeout_combobox->addItem("Immediately");
    lock_timeout_combobox->addItem("5 Minutes");
    lock_timeout_combobox->addItem("15 Minutes");
    lock_timeout_combobox->addItem("30 Minutes");
    lock_timeout_combobox->setCurrentItem(0);
    lock_timeout_combobox->setDarkMode(false);

    // Adding lock_timeout_sublayout, and combo box to layout
    lock_timeout_layout->addLayout(lock_timeout_sublayout);
    lock_timeout_layout->setAlignment(lock_timeout_sublayout, Qt::AlignLeft | Qt::AlignVCenter);
    lock_timeout_layout->addStretch();
    lock_timeout_layout->addWidget(lock_timeout_combobox, 0, Qt::AlignVCenter | Qt::AlignRight);

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

    contentLayout->addLayout(lock_timeout_layout);
    contentLayout->addSpacing(8);
    contentLayout->addWidget(sep_6);
    contentLayout->addSpacing(8);

    contentLayout->addLayout(delete_acc_layout);

    // Theme
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &AccountSettings::setDarkMode);
    setDarkMode(tm.isDarkMode());

    // Setting user detials from session manager after login
    setUserDetailsFromSessionManager();
    connect(&SessionManager::instance(), &SessionManager::avatorUpdated, this, [this](){
        auto &sm = SessionManager::instance();
        if (!sm.avatar().isNull()) 
            pic->setPixmap(Ui::Utils::cropToCircle(sm.avatar(), 80));   
        else
            pic->setPixmap(Ui::Utils::cropToCircle(IconManager::icon(Icons::Avator), 36));        
    });

    // Registering Window for Error Dialog Manager;
    ErrorDialogManager::instance()->registerWindow("AccountSettings", this);

    // Registering Window for Confirm Dialog Manager;
    ConfirmDialogManager::instance()->registerWindow("AccountSettings", this);

    connect(ConfirmDialogManager::instance(), &ConfirmDialogManager::actionTriggered, this, [this](const QString &key, const QString &actionButtonKey){
        if (key == "AccountDelete" && actionButtonKey == "Accept") {
            if (delete_acc_btn) {
                delete_acc_btn->setEnabled(false);
                delete_acc_btn->setText("");
                account_settings_core->deleteAccount();
            }
        }
    });

    // Core Account Settings
    using ASCore = Core::AccountSettingsService;
    account_settings_core = new ASCore;

    connect(account_settings_core, &ASCore::failedToUpdateUsername, this, &AccountSettings::onFailedToUpdateUsername);
    connect(account_settings_core, &ASCore::usernameUpdated, this, &AccountSettings::onUsernameUpdated);
    connect(account_settings_core, &ASCore::failedToUpdateProfilePicture, this, &AccountSettings::onFailedToUpdateProfilePicture);
    connect(account_settings_core, &ASCore::profilePictureUpdated, this, &AccountSettings::onProfilePictureUpdated);
    connect(account_settings_core, &ASCore::accountDeleted, this, &AccountSettings::onAccountDeleted);
    connect(account_settings_core, &ASCore::failedToDeleteAccount, this, &AccountSettings::onFailedToDeleteAccount);
    connect(this, &AccountSettings::accountSettingsClosed, this, &AccountSettings::onAccountSettingsClosed);
    connect(account_settings_core, &ASCore::passwordUpdated, this, &AccountSettings::onPasswordUpdated);
    connect(account_settings_core, &ASCore::failedToUpdatePassword, this, &AccountSettings::onFailedToUpdatePassword);
    connect(account_settings_core, &ASCore::profilePictureDeleted, this, &AccountSettings::onProfilePictureDeleted);
    connect(account_settings_core, &ASCore::failedToDeleteProfilePicture, this, &AccountSettings::onFailedToDeleteProfilePicture);
}

void AccountSettings::closeEvent(QCloseEvent *event) {
    emit accountSettingsClosed();
    SubWindow::closeEvent(event);
}

void AccountSettings::onAccountSettingsClosed() {
    account_settings_core->setNew2FAStatus(two_fa_toggle->isChecked());
}

// Delete Account
void AccountSettings::onDeleteAccountButtonClicked() {
    ConfirmDialogManager::instance()->show("AccountDelete", "AccountSettings");
}

void AccountSettings::onFailedToDeleteAccount(Er failure) {
    if (failure != Er::NoInternet)
        ErrorDialogManager::instance()->show(tr("SomethingWentWrong"), tr("AccountSettings"));
    
    if (!delete_acc_btn)
        return;

    delete_acc_btn->setEnabled(true);
    delete_acc_btn->setText(tr("Delete Account"));
}

void AccountSettings::onAccountDeleted() {
    qApp->quit();
}

// Username Change
void AccountSettings::onUsernameUpdated() {
    username_field->setReadOnly(true);
    username_field->validityIcon()->hide();
    username_field->setFocus();

    change_username_btn->hide();
    change_username_btn->deleteLater();
}

void AccountSettings::onChangeUsernameButtonClicked() {
    change_username_btn->setText("");
    change_username_btn->setEnabled(false);

    username_field->setReadOnly(true);
    username_field->setFocus();

    if (account_settings_core)
        account_settings_core->setUsername(username_field->text());
}

void AccountSettings::onEnableChangeUsernameButtonClicked() {
    username_field->setReadOnly(false);
    username_field->validityIcon()->show();
    username_field->setFocus();

    enable_username_change_btn->hide();
    enable_username_change_btn->deleteLater();
}

void AccountSettings::onUsernameAvailable(bool isAvailable) {
    if (isAvailable) {
        username_field->setValid();
        username_field->setTooltip("Username is available.");
    } else {
        username_field->setInvalid();
        username_field->setTooltip("Username is already taken.\nPlease use another.");
    }

    change_username_btn->setVisible(isAvailable);
}

void AccountSettings::onFailedToUpdateUsername(Er failure) {
    if (failure != Er::NoInternet)
        ErrorDialogManager::instance()->show("SomethingWentWrong", "AccountSettings");
        
    username_field->setReadOnly(false);
    username_field->setFocus();

    change_username_btn->setEnabled(true);
    change_username_btn->setText(tr("Change"));
}

// Profile Picture 
void AccountSettings::onProfilePictureUpdated() {
    if (upload_pic_btn) {
        upload_pic_btn->setText(tr("Change Profile Picture"));
        upload_pic_btn->setEnabled(true);
    }

    if (delete_pic_btn)
        delete_pic_btn->show();
}

void AccountSettings::onUploadPictureButtonClicked() {
    _profilePicturePath = QFileDialog::getOpenFileName(
        this,
        tr("Select a profile picture"),
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
        "Image Files (*.png *.jpeg *.jpg)"
    );
    
    QFileInfo info(_profilePicturePath);
            
    if (info.size() > PROFILE_PICTURE_MAX_SIZE) {
        WARN_HERE("Profile picture size is too large.");
        return;
    }

    if (pic) 
        if (!_profilePicturePath.isEmpty()) {
           
            pic->setPixmap(Ui::Utils::cropToCircle(_profilePicturePath, 80));
        
            if (!upload_pic_btn)
                return;

            if (!delete_pic_btn)
                return;

            upload_pic_btn->setText("");
            upload_pic_btn->setEnabled(false);

            account_settings_core->setProfilePicturePath(_profilePicturePath);
        }
}

void AccountSettings::onDeletePictureButtonClicked() {
    if (account_settings_core)
        account_settings_core->deleteProfilePicture();
    
    if (delete_pic_btn)
        delete_pic_btn->setEnabled(false);

    if (upload_pic_btn)
        upload_pic_btn->setEnabled(false);
}

void AccountSettings::onFailedToUpdateProfilePicture(Er failure) {
    if (failure != Er::NoInternet)
        ErrorDialogManager::instance()->show("SomethingWentWrong", "AccountSettings");

    if (pic)
        pic->setPixmap(Ui::Utils::cropToCircle(IconManager::icon(Icons::Avator), 80));
    
    if (upload_pic_btn) {
        upload_pic_btn->setEnabled(true);
        upload_pic_btn->setText(tr("Upload Profile Picture"));
    }
}

void AccountSettings::onProfilePictureDeleted() {
    _profilePicturePath = "";

    if (pic)
        pic->setPixmap(Ui::Utils::cropToCircle(IconManager::icon(Icons::Avator), 80));
    
    if (upload_pic_btn) {
        upload_pic_btn->setText(tr("Upload Profile Picture"));
        upload_pic_btn->setEnabled(true);
    }

    if (delete_pic_btn) {
        delete_pic_btn->hide();
        delete_pic_btn->setEnabled(true);
    }
}

void AccountSettings::onFailedToDeleteProfilePicture(Er failure) {
    if (failure != Er::NoInternet)
        ErrorDialogManager::instance()->show("SomethingWentWrong", "AccountSettings");
    
    if (upload_pic_btn) 
        upload_pic_btn->setEnabled(true);

    if (delete_pic_btn)
        delete_pic_btn->setEnabled(true);
}

// Password Validation
void AccountSettings::onPasswordValidationUpdated(const PasswordValidator::PasswordValidationResult &result) {
    if (!change_password_btn || !curr_password_field)
        return;

    const bool canChange = result.isStrong && curr_password_field->text().length() >= 8;
    change_password_btn->setVisible(canChange);
}

void AccountSettings::onChangePasswordButtonClicked() {
    account_settings_core->setPassword(curr_password_field->text(), new_password_field->text());

    curr_password_field->setReadOnly(true);
    new_password_field->setReadOnly(true);

    change_password_btn->setText("");
    change_password_btn->setEnabled(false);
}

void AccountSettings::onPasswordUpdated() {
    change_password_btn->hide();
    change_password_btn->setText("");
    change_password_btn->setEnabled(true);

    curr_password_field->setText("");
    new_password_field->setText("");

    curr_password_field->setReadOnly(false);
    new_password_field->setReadOnly(false);
}

void AccountSettings::onFailedToUpdatePassword(Er failure) {
    if (failure != Er::NoInternet)
        ErrorDialogManager::instance()->show("SomethingWentWrong", "AccountSettings");

    curr_password_field->setReadOnly(false);
    new_password_field->setReadOnly(false);
    new_password_field->setFocus();

    change_password_btn->setEnabled(true);
    change_password_btn->setText(tr("Change"));
}

void AccountSettings::setUserDetailsFromSessionManager() {
    auto &sm = SessionManager::instance();

    name_field->setText(sm.fullName());
    username_field->setText(sm.username());
    email_field->setText(sm.email());
    two_fa_toggle->setChecked(sm.twoFAStatus());

    if (!sm.avatar().isNull()) {
        pic->setPixmap(Ui::Utils::cropToCircle(sm.avatar(), 80));
        upload_pic_btn->setText(tr("Change Profile Picture"));
        delete_pic_btn->show();
    } else
        pic->setPixmap(Ui::Utils::cropToCircle(IconManager::icon(Icons::Avator), 80));
}

void AccountSettings::setDarkMode(bool isDarkMode) {
    // Scrollbar
    scrollbar->setDarkMode(isDarkMode);

    // -------------------- Subtexts --------------------
    for (Label* label : { main_header_text, profile_pic_hint, password_text, two_fa_text, delete_acc_text, email_note, password_note, username_note, lock_timeout_subText })
        if (label)
            label->setTextColor(isDarkMode ? "#94A3B8" : "#6B7280");

    // -------------------- Headers --------------------
    for (Label* label : {winTitle, mainHeader, name_header, username_header, email_header, password_header, two_fa_header, delete_acc_header, profile_pic_title, lock_timeout_header })
        if (label)
            label->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");

    // -------------------- TextFields --------------------
    for (TextField* field : { name_field, email_field, curr_password_field })
        if (field)
            field->setDarkMode(isDarkMode);

    if (username_field)
        username_field->setDarkMode(isDarkMode);
    
    if (new_password_field)
        new_password_field->setDarkMode(isDarkMode);

    // -------------------- Buttons --------------------
    for (Button* btn : {upload_pic_btn, delete_pic_btn, enable_username_change_btn, change_username_btn, change_password_btn})
        if (btn)
            btn->setDarkMode(isDarkMode);

    // -------------------- Toggle --------------------
    if (two_fa_toggle) two_fa_toggle->setDarkMode(isDarkMode);

    // -------------------- Seperators --------------------
    for (Seperator* sep : {sep_1, sep_2, sep_3, sep_4, sep_5, sep_6 })
        if (sep)
            sep->setColor(isDarkMode ? "#334155" : "#E5E7EB");

    // ---------------- Combo Box -------------------
    if (lock_timeout_combobox)
        lock_timeout_combobox->setDarkMode(isDarkMode);

    // Base class method
    SubWindow::setDarkMode(isDarkMode);
}