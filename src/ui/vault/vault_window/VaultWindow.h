#pragma once

#include "../../windows/window/Window.h"

class Seperator;
class Label;
class QHBoxLayout;
class QVBoxLayout;

namespace Ui::Vault {
    class Statusbar;
    class View;
    class User;
    class Toolbar;
    class Update;
    class About;
    class ReportBug;
}

namespace Ui::Vault { 
    /* --------------------- Vault Main Window --------------------------- */
    class VaultWindow : public Window {
        Q_OBJECT

        public:
        static VaultWindow* instance(QWidget *parent = nullptr);
        Statusbar* statusbar() const;

        private:    
        explicit VaultWindow(QWidget *parent = nullptr);
        Button* createButton(const QString &iconPathLight, const QString &iconPathDark);
        void setDarkMode(bool isDarkMode);
        void onAboutBtnClicked();
        void onPreferencesBtnClicked();
        void onAppUpdateBtnClicked();
        void onReportBugBtnClicked();

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
        // Ui::Vault::Preferences *pref = nullptr;

        // Updates Download Button
        Button *updates_download_btn = nullptr;
           
        // Updates Download Button
        Button *about_btn = nullptr;

        // App Updates 
        Ui::Vault::Update *appUpdateWidget = nullptr;
            
        // App About
        Ui::Vault::About *about = nullptr;

        // Report a bug Button
        Button *report_bug_btn = nullptr;

        // Report Bug
        Ui::Vault::ReportBug *report_bug = nullptr;

        // Content Area
        // Main Layout
        QVBoxLayout *content_layout = nullptr;

        // Toolbar
        Ui::Vault::Toolbar *toolbar = nullptr;

        // View
        Ui::Vault::View *view = nullptr;

        // Statusbar
        Ui::Vault::Statusbar *_statusbar = nullptr;
    };
};