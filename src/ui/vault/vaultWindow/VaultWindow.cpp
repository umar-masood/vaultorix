#include "VaultWindow.h"
#include "../../../resources/IconManager.h"

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
  setMinimumSize(QSize(1000,600));

  // Icons
  AppIcon       = IconManager::icon(Icons::AppIcon);
  DarkModeIcon  = IconManager::icon(Icons::DarkMode);
  LightModeIcon = IconManager::icon(Icons::LightMode);

  // Title Bar
  // App Icon
  app_icon = new Label(true);
  app_icon->setFixedSize(QSize(26, 24));
  app_icon->setPixmap(QPixmap(AppIcon).scaled(26, 24, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

  // App Name
  app_name = new Label("Segoe UI", 11, QFont::Normal, false, "Vaultorix");
   
  // Seperator
  seperator = new Seperator(nullptr, 18, 1, Qt::Vertical);
  seperator->raise();

  // Theme Mode Button
  theme_mode_btn = createButton(IconManager::icon(Icons::LightMode), IconManager::icon(Icons::LightMode));
  theme_mode_btn->setToolTip("Change theme mode");
  setInteractiveTitleBarWidget(theme_mode_btn);

  // Theme Mode Button Signal Slot
  connect(theme_mode_btn, &Button::clicked, this, [this](){
    isDarkMode = !isDarkMode;
    emit themeModeChanged(isDarkMode);
  });

  connect(this, &VaultWindow::themeModeChanged, this, &VaultWindow::onthemeModeChanged);   

  // Sign out Button
  sign_out_btn = new Button("Sign out");
  sign_out_btn->setCursor(Qt::PointingHandCursor);
  sign_out_btn->setFontXY(0, -1);
  sign_out_btn->setGradientColors("#bc0101", "#ff1100", "#FF3727");
  sign_out_btn->setFontProperties("Segoe UI", 10);
  sign_out_btn->setIconSize(QSize(18, 18));
  sign_out_btn->setDisplayMode(Button::IconText);
  sign_out_btn->setFixedSize(QSize(100, 26));
  sign_out_btn->setPrimaryButtonIcon(IconManager::icon(Icons::SignOut));
  sign_out_btn->setToolTip("Sign out of your account");
  setInteractiveTitleBarWidget(sign_out_btn);

  // Preferences Button
  preferences_btn = createButton(IconManager::icon(Icons::PreferencesLight), IconManager::icon(Icons::PreferencesDark));
  preferences_btn->setToolTip("Preferences");

  connect(preferences_btn, &Button::clicked, this, [this](){
    if (!pref) {
      pref = new Preferences(this);
      pref->setAttribute(Qt::WA_DeleteOnClose);
      pref->setDarkMode(isDarkMode);

      connect(pref, &QObject::destroyed, this, [this](){
        pref = nullptr;
        VaultWindow::instance()->updateGeometry(); 
      });
    }

    pref->show();
    pref->raise();
  });

  // Title Bar Layout
  titlebar_layout = new QHBoxLayout(titleBar());
  titlebar_layout->setSpacing(0);
  titlebar_layout->setContentsMargins(0, 0, 0, 0);

  titlebar_layout->addSpacing(7);
  titlebar_layout->addWidget(app_icon, 0, Qt::AlignLeft);
  titlebar_layout->addSpacing(10);
  titlebar_layout->addWidget(app_name, 0, Qt::AlignLeft);

  titlebar_layout->addStretch();

  titlebar_layout->addWidget(sign_out_btn, 0, Qt::AlignRight);
  titlebar_layout->addSpacing(10);
  titlebar_layout->addWidget(preferences_btn, 0, Qt::AlignRight);
  titlebar_layout->addSpacing(10);
  titlebar_layout->addWidget(theme_mode_btn, 0, Qt::AlignRight);
  titlebar_layout->addSpacing(10);
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

  // Inital Theme
  emit themeModeChanged(false);
}

Button* VaultWindow::createButton(const QString &iconPathLight, const QString &iconPathDark) {
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

void VaultWindow::onthemeModeChanged(bool enable) {
  // Calling Base class member function of theme
  setDarkMode(enable);

  // App name
  enable ? app_name->setStyleSheet("color: white;") : app_name->setStyleSheet("color: black;");

  // Theme Mode Button
  theme_mode_btn->setDarkMode(enable);
  
  enable ? theme_mode_btn->setIconPaths(LightModeIcon, LightModeIcon) : theme_mode_btn->setIconPaths(DarkModeIcon, DarkModeIcon);

  // Preferences Button
  preferences_btn->setDarkMode(enable);

  // Toolbar theme
  toolbar->setDarkMode(enable);
  
  // View theme
  view->setDarkMode(enable);
  
  // Statusbar theme
  _statusbar->setDarkMode(enable);
}

Statusbar *VaultWindow::statusbar() const { return _statusbar; }
