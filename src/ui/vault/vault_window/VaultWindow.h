#pragma once

#include "../../windows/window/Window.h"
#include "../../components/Seperator.h"
#include "../../components/Label.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

/* ------------------- Forward Declarations ---------------------------*/
class Statusbar;
class Preferences;
class View;
class User;
class Toolbar;
class AppUpdates;
class About;
class ReportBug;

/* --------------------- Vault Main Window --------------------------- */
class VaultWindow : public Window {
    Q_OBJECT

    public:
    static VaultWindow* instance(QWidget *parent = nullptr);
    void setDarkMode(bool isDarkMode);
    Statusbar* statusbar() const;

    private:    
    explicit VaultWindow(QWidget *parent = nullptr);
    Button* createButton(const QString &iconPathLight, const QString &iconPathDark);

    // Title Bar
    // Layout
    QHBoxLayout *titlebar_layout = nullptr;

    // App name & icon
    Label *app_icon = nullptr;
    Label *app_name = nullptr;

    // Seperator for titlebar controls
    Seperator *seperator = nullptr;

    // Theme Mode Button   
    Button *theme_mode_btn = nullptr;

    // Icons   
    QString AppIcon, DarkModeIcon, LightModeIcon;

    // Settings Button
    Button *preferences_btn  = nullptr;

    // Preferences Settings
    Preferences *pref = nullptr;
    
    // Updates Download Button
    Button *updates_download_btn = nullptr;

    // Updates Download Button
    Button *about_btn = nullptr;

    // App Updates 
    AppUpdates *au = nullptr;

    // App About
    About *about = nullptr;

    // Report a bug Button
    Button *report_bug_btn = nullptr;

    // Report Bug
    ReportBug *report_bug = nullptr;

    // Content Area
    // Main Layout
    QVBoxLayout *content_layout = nullptr;
    
    // Toolbar
    Toolbar *toolbar = nullptr;

    // View
    View *view = nullptr;

    // Statusbar
    Statusbar *_statusbar = nullptr;
};