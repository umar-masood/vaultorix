#include "Preferences.h"
#include "../../../../resources/IconManager.h"
#include "../../../core/theme/ThemeManager.h"
#include "../../components/Label.h"
#include "../../components/Toggle.h"
#include "../../components/Seperator.h"
#include "../../components/ComboBox.h"

using Ui::Vault::Preferences;

Preferences::Preferences(QWidget *parent) : SubWindow(QSize(600, 600), parent)
{
    setFocusPolicy(Qt::StrongFocus);
    setModal(true);

    // --------------------------- Window Titlebar Layout ---------------------------
    QHBoxLayout *_titlebarLayout = this->titlebarLayout();

    // --------------------------- Window Title ---------------------------
    winTitle = new Label("Segoe UI", 10, QFont::Normal, false, "Preferences", Qt::AlignLeft);
    _titlebarLayout->addSpacing(6);
    _titlebarLayout->addWidget(winTitle, 0, Qt::AlignLeft | Qt::AlignVCenter);
    _titlebarLayout->addStretch();

    // ---------------- Window Content Area Layout -------------------------------
    auto *win_content_area_layout = new QVBoxLayout(contentArea());
    win_content_area_layout->setSpacing(0);
    win_content_area_layout->setContentsMargins(14, 34, 14, 14);

    // ============================ Seperator ========================================
    sep_1 = new Seperator( 1, width() - 2*14, Qt::Horizontal);

    /* -----------------------------------------------------------------------------------------
                            OPTION 2 : Encrypt Files on Import
    --------------------------------------------------------------------------------------------*/
    // Layout
    auto *encrypt_import_layout = new QHBoxLayout;
    encrypt_import_layout->setContentsMargins(0, 0, 0, 0);
    encrypt_import_layout->setSpacing(0);

    // Icon
    encrypt_import_icon = new Label(true);
    encrypt_import_icon->setFixedSize(QSize(30, 30));
    encrypt_import_icon->setPixmap(QPixmap(IconManager::icon(Icons::AppIcon)).scaled(QSize(30, 30), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Sublayout
    auto *encrypt_import_sublayout = new QVBoxLayout;
    encrypt_import_sublayout->setContentsMargins(0, 0, 0, 0);
    encrypt_import_sublayout->setSpacing(0);

    // Header
    encrypt_import_header = new Label("Segoe UI", 10, QFont::Normal, false, "Encrypt Files on Import", Qt::AlignLeft);

    // Subtext
    encrypt_import_subText = new Label("Segoe UI", 10, QFont::Normal, false, "Automatically encrypt files when adding them to your vault for enhanced security", Qt::AlignLeft);
    encrypt_import_subText->setWordWrap(true);
    encrypt_import_subText->setMinimumWidth(360);

    // Adding Header and Subtext to encrypt_import_sublayout
    encrypt_import_sublayout->addWidget(encrypt_import_header, 0, Qt::AlignLeft);
    encrypt_import_sublayout->addSpacing(2);
    encrypt_import_sublayout->addWidget(encrypt_import_subText, 0, Qt::AlignLeft);

    // Toggle
    encrypt_import_toggle = new Toggle;

    // Adding icon, encrypt_import_sublayout, and combo box to layout
    encrypt_import_layout->addWidget(encrypt_import_icon, 0, Qt::AlignVCenter | Qt::AlignLeft);
    encrypt_import_layout->addSpacing(15);
    encrypt_import_layout->addLayout(encrypt_import_sublayout);
    encrypt_import_layout->setAlignment(encrypt_import_sublayout, Qt::AlignLeft | Qt::AlignVCenter);
    encrypt_import_layout->addStretch();
    encrypt_import_layout->addWidget(encrypt_import_toggle, 0, Qt::AlignVCenter | Qt::AlignRight);

    // Adding layout to main layout
    win_content_area_layout->addSpacing(8);
    win_content_area_layout->addWidget(sep_1, 0, Qt::AlignHCenter);
    win_content_area_layout->addSpacing(8);
    win_content_area_layout->addLayout(encrypt_import_layout);

    // ================================= Seperator ==============================================
    sep_2 = new Seperator( 1, width() - 2*14, Qt::Horizontal);

    /* -----------------------------------------------------------------------------------------
                            OPTION 4 : Automatic Updates
    --------------------------------------------------------------------------------------------*/
    // Layout
    auto *auto_updates_layout = new QHBoxLayout;
    auto_updates_layout->setContentsMargins(0, 0, 0, 0);
    auto_updates_layout->setSpacing(0);

    // Icon
    auto_updates_icon = new Label(true);
    auto_updates_icon->setFixedSize(QSize(30, 30));
    auto_updates_icon->setPixmap(QPixmap(IconManager::icon(Icons::AppIcon)).scaled(QSize(30, 30), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Sublayout
    auto *auto_updates_sublayout = new QVBoxLayout;
    auto_updates_sublayout->setContentsMargins(0, 0, 0, 0);
    auto_updates_sublayout->setSpacing(0);

    // Header
    auto_updates_header = new Label("Segoe UI", 10, QFont::Normal, false, "Automatic Updates", Qt::AlignLeft);

    // Subtext
    auto_updates_subText = new Label("Segoe UI", 10, QFont::Normal, false, "Download and install updates automatically to keep Vaultorix secure and up to date.", Qt::AlignLeft);
    auto_updates_subText->setWordWrap(true);
    auto_updates_subText->setMinimumWidth(360);

    // Adding Header and Subtext to auto_updates_sublayout
    auto_updates_sublayout->addWidget(auto_updates_header, 0, Qt::AlignLeft);
    auto_updates_sublayout->addSpacing(2);
    auto_updates_sublayout->addWidget(auto_updates_subText, 0, Qt::AlignLeft);

    // Toggle
    auto_updates_toggle = new Toggle;

    // Adding icon, auto_updates_sublayout, and combo box to layout
    auto_updates_layout->addWidget(auto_updates_icon, 0, Qt::AlignVCenter | Qt::AlignLeft);
    auto_updates_layout->addSpacing(15);
    auto_updates_layout->addLayout(auto_updates_sublayout);
    auto_updates_layout->setAlignment(auto_updates_sublayout, Qt::AlignLeft | Qt::AlignVCenter);
    auto_updates_layout->addStretch();
    auto_updates_layout->addWidget(auto_updates_toggle, 0, Qt::AlignVCenter | Qt::AlignRight);

    // Adding layout to main layout
    win_content_area_layout->addSpacing(8);
    win_content_area_layout->addWidget(sep_2, 0, Qt::AlignHCenter);
    win_content_area_layout->addSpacing(8);
    win_content_area_layout->addLayout(auto_updates_layout);

    // ============================ Seperator ========================================
    sep_3 = new Seperator( 1, width() - 2*14, Qt::Horizontal);

    /* -----------------------------------------------------------------------------------------
                            OPTION 5: App Language
    --------------------------------------------------------------------------------------------*/
    // Layout
    auto *app_lang_layout = new QHBoxLayout;
    app_lang_layout->setContentsMargins(0, 0, 0, 0);
    app_lang_layout->setSpacing(0);

    // Icon
    app_lang_icon = new Label(true);
    app_lang_icon->setFixedSize(QSize(30, 30));
    app_lang_icon->setPixmap(QPixmap(IconManager::icon(Icons::AppIcon)).scaled(QSize(30, 30), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Sublayout
    auto *app_lang_sublayout = new QVBoxLayout;
    app_lang_sublayout->setContentsMargins(0, 0, 0, 0);
    app_lang_sublayout->setSpacing(0);

    // Header
    app_lang_header = new Label("Segoe UI", 10, QFont::Normal, false, "App Language", Qt::AlignLeft);

    // Subtext
    app_lang_subText = new Label("Segoe UI", 10, QFont::Normal, false, "Choose the language used throughout the application interface.", Qt::AlignLeft);
    app_lang_subText->setWordWrap(true);
    app_lang_subText->setMinimumWidth(360);

    // Adding Header and Subtext to app_lang_sublayout
    app_lang_sublayout->addWidget(app_lang_header, 0, Qt::AlignLeft);
    app_lang_sublayout->addSpacing(2);
    app_lang_sublayout->addWidget(app_lang_subText, 0, Qt::AlignLeft);

    // ComboBox
    static const QStringList languages = {
        "English",
        "Español",
        "Português",
        "Français",
        "Deutsch",
        "Italiano",
        "Nederlands",
        "Polski",
        "Русский",
        "Türkçe",
        "Urdu",
        "العربية"
    };

    app_lang_combobox = new ComboBox;
    app_lang_combobox->setMaxVisibleItems(12);
    app_lang_combobox->setFieldSize(QSize(160, 36));

    for (auto l : languages)
        app_lang_combobox->addItem(l);

    app_lang_combobox->setCurrentItem(0);

    // Adding icon, app_lang_sublayout, and combo box to layout
    app_lang_layout->addWidget(app_lang_icon, 0, Qt::AlignVCenter | Qt::AlignLeft);
    app_lang_layout->addSpacing(15);
    app_lang_layout->addLayout(app_lang_sublayout);
    app_lang_layout->setAlignment(app_lang_sublayout, Qt::AlignLeft | Qt::AlignVCenter);
    app_lang_layout->addStretch();
    app_lang_layout->addWidget(app_lang_combobox, 0, Qt::AlignVCenter | Qt::AlignRight);

    // Adding layout to main layout
    win_content_area_layout->addSpacing(8);
    win_content_area_layout->addWidget(sep_3, 0, Qt::AlignHCenter);
    win_content_area_layout->addSpacing(8);
    win_content_area_layout->addLayout(app_lang_layout);
    win_content_area_layout->addStretch();

    // Theme
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &Preferences::setDarkMode);
    setDarkMode(tm.isDarkMode());
}

void Preferences::setDarkMode(bool isDarkMode) {
    // -------------------- Icons --------------------
    QHash<Label *, QPair<Icons, Icons>> iconMap = {
        {encrypt_import_icon, {Icons::FolderLockLight, Icons::FolderLockDark}},
        {auto_updates_icon, {Icons::UpdateLight, Icons::UpdateDark}},
        {app_lang_icon, {Icons::LanguagesLight, Icons::LanguagesDark}}};

    for (auto it = iconMap.constBegin(); it != iconMap.constEnd(); ++it) {

        Label *label = it.key();
        if (!label)
            continue;

        Icons iconType = isDarkMode ? it.value().second : it.value().first;

        label->setPixmap(IconManager::renderSvg(IconManager::icon(iconType), QSize(24, 24)));
    }

    // -------------------- Headers --------------------
    for (auto* label : { winTitle, encrypt_import_header, auto_updates_header, app_lang_header })
        if (label)
            label->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");

    // -------------------- Subtexts --------------------
    for (Label* label : { encrypt_import_subText, auto_updates_subText, app_lang_subText })
        if (label)
            label->setTextColor(isDarkMode ? "#94A3B8" : "#6B7280");

    // -------------------- ComboBoxes --------------------
    for (ComboBox* box : { app_lang_combobox })
        if (box)
            box->setDarkMode(isDarkMode);

    // -------------------- Toggles --------------------
    for (Toggle* toggle : { encrypt_import_toggle, auto_updates_toggle })
        if (toggle)
            toggle->setDarkMode(isDarkMode);

    // -------------------- Seperators --------------------
    for (Seperator* sep : { sep_1, sep_2, sep_3})
        if (sep)
            sep->setColor(isDarkMode ? "#334155" : "#E5E7EB");

    // Base class method
    SubWindow::setDarkMode(isDarkMode);
}

// -------------------- GETTERS --------------------
ComboBox* Preferences::languageComboBox() const { return app_lang_combobox; }
Toggle* Preferences::encryptImportToggle() const { return encrypt_import_toggle; }
Toggle* Preferences::autoUpdatesToggle() const { return auto_updates_toggle; }
