#pragma once

#include "../../windows/window/Window.h"
#include "../../components/Seperator.h"
#include "../../components/ToolTip.h"
#include "../../components/Label.h"

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
    static VaultWindow* instance(QWidget *parent = nullptr);
    Statusbar* statusbar() const;

    private:    
    explicit VaultWindow(QWidget *parent = nullptr);
    Button *createButton(const QString &iconPathLight, const QString &iconPathDark);

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
    QString AppIcon, DarkModeIcon, LightModeIcon;
    
    // Flag
    bool isDarkMode = false;
 
    // Sign out Button
    Button *sign_out_btn = nullptr;
    // ToolTip
    ToolTip *sign_out_btn_tip = nullptr;

    // Settings button
    Button *preferences_btn  = nullptr;
    // ToolTip
    ToolTip *preferences_btn_tip = nullptr;

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