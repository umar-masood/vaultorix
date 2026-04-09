#include "VaultWindow.h"

#include "../statusbar/Statusbar.h"
#include "../preferences/Preferences.h"
#include "../view/View.h"
#include "../user/User.h"
#include "../toolbar/Toolbar.h"
#include "../update/Update.h"
#include "../about/About.h"
#include "../report_bug/ReportBug.h"
#include "../../../../resources/IconManager.h"
#include "../../../core/theme/ThemeManager.h"

VaultWindow *VaultWindow::instance(QWidget *parent) {
    static VaultWindow *vw = nullptr;

    if (!vw)
        vw = new VaultWindow(parent);

    return vw;
}

VaultWindow::VaultWindow(QWidget *parent) : Window(parent) {
    // Window Properties
    setWindowTitle("Vaultorix");
    setWindowIcon(QIcon(AppIcon));
    setMinimumSize(QSize(1000, 600));

    // Icons
    AppIcon = IconManager::icon(Icons::AppIcon);
    DarkModeIcon = IconManager::icon(Icons::DarkMode);
    LightModeIcon = IconManager::icon(Icons::LightMode);

    // Title Bar
    // App Icon
    app_icon = new Label(true);
    app_icon->setFixedSize(QSize(26, 24));
    app_icon->setPixmap(QPixmap(AppIcon).scaled(26, 24, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    // App Name
    app_name = new Label("Segoe UI", 11, QFont::Normal, false, "Vaultorix");

    // Seperator
    seperator = new Seperator(18, 1, Qt::Vertical);
    seperator->raise();

    // Theme Mode Button
    theme_mode_btn = createButton(IconManager::icon(Icons::LightMode), IconManager::icon(Icons::LightMode));
    theme_mode_btn->setToolTip("Change theme mode");
    setInteractiveTitleBarWidget(theme_mode_btn);

    // Theme Mode Button Signal Slot
    connect(theme_mode_btn, &Button::clicked, this, [this]() {
       auto &tm = ThemeManager::instance();
       tm.setDarkMode(!tm.isDarkMode()); 
    });

    // Update Download Button
    updates_download_btn = createButton(IconManager::icon(Icons::DownloadUpdateLight), IconManager::icon(Icons::DownloadUpdateDark));
    updates_download_btn->setToolTip("Vaultorix Updates");
    connect(updates_download_btn, &Button::clicked, this, &VaultWindow::onUpdatesBtnClicked);

    // Preferences Button
    preferences_btn = createButton(IconManager::icon(Icons::PreferencesLight), IconManager::icon(Icons::PreferencesDark));
    preferences_btn->setToolTip("Preferences");
    connect(preferences_btn, &Button::clicked, this, &VaultWindow::onPreferencesBtnClicked);

    // About Button
    about_btn = createButton(IconManager::icon(Icons::AboutLight), IconManager::icon(Icons::AboutDark));
    about_btn->setToolTip("About Vaultorix");
    connect(about_btn, &Button::clicked, this, &VaultWindow::onAboutBtnClicked);

    // Report Bug Button
    report_bug_btn = createButton(IconManager::icon(Icons::BugLight), IconManager::icon(Icons::BugDark));
    report_bug_btn->setToolTip("Report a bug");
    connect(report_bug_btn, &Button::clicked, this, &VaultWindow::onReportBugBtnClicked);

    // Title Bar Layout
    titlebar_layout = new QHBoxLayout(titleBar());
    titlebar_layout->setSpacing(0);
    titlebar_layout->setContentsMargins(0, 0, 0, 0);

    titlebar_layout->addSpacing(7);
    titlebar_layout->addWidget(app_icon, 0, Qt::AlignLeft);
    titlebar_layout->addSpacing(10);
    titlebar_layout->addWidget(app_name, 0, Qt::AlignLeft);

    titlebar_layout->addStretch();

    for (auto *btn : {report_bug_btn, about_btn, updates_download_btn, preferences_btn, theme_mode_btn})
    {
        titlebar_layout->addWidget(btn, 0, Qt::AlignRight);
        titlebar_layout->addSpacing(10);
    }

    titlebar_layout->addWidget(seperator, 0, Qt::AlignRight);
    titlebar_layout->addSpacing(10);

    // Content Area
    // Toolbar
    toolbar = new Toolbar;

    // View
    view = new View;

    // Statusbar
    _statusbar = new Statusbar;

    // Layout
    content_layout = new QVBoxLayout(contentArea());
    content_layout->setContentsMargins(6, 0, 6, 4);
    content_layout->setSpacing(0);
    content_layout->addSpacing(2);
    content_layout->addWidget(toolbar, 0, Qt::AlignTop);
    content_layout->addSpacing(4);
    content_layout->addWidget(view);
    content_layout->addSpacing(4);
    content_layout->addWidget(_statusbar);

    // Theme Mode
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &VaultWindow::setDarkMode);
    setDarkMode((tm.isDarkMode()));
}

Button *VaultWindow::createButton(const QString &iconPathLight, const QString &iconPathDark) {
    auto *btn = new Button;
    btn->setDisplayMode(Button::IconOnly);
    btn->setFixedSize(QSize(26, 26));
    btn->setSecondary(true);
    btn->setNormalBackgroundTransparent(true);
    btn->setBorderTransparent(true);
    btn->setIconSize(QSize(18, 18));
    btn->setIconPaths(iconPathLight, iconPathDark);
    btn->setCursor(Qt::PointingHandCursor);
    return btn;
}

void VaultWindow::setDarkMode(bool isDarkMode) {
    // Calling Base class member function of theme
    Window::setDarkMode(isDarkMode);

    // App name
    isDarkMode ? app_name->setStyleSheet("color: white;") : app_name->setStyleSheet("color: black;");

    // Buttons
    for (auto *btn : {theme_mode_btn, preferences_btn, updates_download_btn, about_btn, report_bug_btn})
        btn->setDarkMode(isDarkMode);

    isDarkMode ? theme_mode_btn->setIconPaths(LightModeIcon, LightModeIcon) 
               : theme_mode_btn->setIconPaths(DarkModeIcon, DarkModeIcon);
}

void VaultWindow::onAboutBtnClicked() {
    if (!about) {
        about = new About(this);
        about->setAttribute(Qt::WA_DeleteOnClose);

        connect(about, &QObject::destroyed, this, [this](){
            about = nullptr;
            VaultWindow::instance()->updateGeometry(); 
        });
    }

    about->show();
    about->raise();
}

void VaultWindow::onPreferencesBtnClicked() {
    if (!pref) {
        pref = new Preferences(this);
        pref->setAttribute(Qt::WA_DeleteOnClose);   

        connect(pref, &QObject::destroyed, this, [this](){
            pref = nullptr;
            VaultWindow::instance()->updateGeometry(); 
        });
    }

    pref->show();
    pref->raise();
}

void VaultWindow::onUpdatesBtnClicked() {
    if (!au) {
        au = new AppUpdates(this);
        au->setAttribute(Qt::WA_DeleteOnClose);

        connect(au, &QObject::destroyed, this, [this](){
            au = nullptr;
            VaultWindow::instance()->updateGeometry(); 
        });

        // Au must exist while timer is running
        QTimer::singleShot(5000, au, [=]() {
            if (au->updateInfoWidget()) {
                au->updateInfoWidget()->setUpdateAvailable(true);
            }
        });

        au->show();
        au->raise();
    } 
}

void VaultWindow::onReportBugBtnClicked() {
    if (!report_bug) {
        report_bug = new ReportBug(this);
        report_bug->setAttribute(Qt::WA_DeleteOnClose);

        connect(report_bug, &QObject::destroyed, this, [this]() {
            report_bug = nullptr;
            VaultWindow::instance()->updateGeometry(); 
        });
    }

    report_bug->show();
    report_bug->raise();
}

Statusbar *VaultWindow::statusbar() const { return _statusbar; }