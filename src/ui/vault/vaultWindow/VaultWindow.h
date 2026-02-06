#pragma once

#include "../../windows/window/Window.h"
#include "../../components/Label.h"
#include "../../components/Seperator.h"
#include "../../components/ToolTip.h"
#include "../../../resources/IconManager.h"

#include "../statusbar/Statusbar.h"
#include "../view/View.h"
#include "../user/User.h"
#include "../toolbar/Toolbar.h"
#include <QHBoxLayout>
#include <QVBoxLayout>


/* --------------------- Vault Main Window --------------------------- */
class VaultWindow : public Window {
    Q_OBJECT

    public:
    explicit VaultWindow(QWidget *parent = nullptr);
    Statusbar* statusbar() const;

    private:
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
    // ToolTip
    ToolTip *theme_mode_btn_tip = nullptr;

    // Icons   
    const QString AppIcon       = IconManager::icon(Icons::AppIcon);
    const QString DarkModeIcon  = IconManager::icon(Icons::DarkMode);
    const QString LightModeIcon = IconManager::icon(Icons::LightMode);
    
    // Flag
    bool isDarkMode = false;
 
    // Sign out Button
    Button *sign_out_btn = nullptr;
    // ToolTip
    ToolTip *sign_out_btn_tip = nullptr;

    // Content Area
    // Main Layout
    QVBoxLayout *content_layout = nullptr;
    
    // Toolbar
    Toolbar *toolbar = nullptr;

    // View
    View *view = nullptr;

    // Statusbar
    Statusbar *_statusbar = nullptr;

    signals:
    void themeModeChanged(bool enable);

    private slots:
    void onthemeModeChanged(bool enable);
};