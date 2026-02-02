#include "VaultWindow.h"

VaultWindow::VaultWindow(QWidget *parent) : Window(parent) {
  // Window Properties
  setWindowTitle("Vaultorix");
  setWindowIcon(QIcon(appIcon));
  setMinimumSize(QSize(1000,600));

  // Title Bar
  // App Icon
  app_icon = new Label(true);
  app_icon->setFixedSize(QSize(26, 24));
  app_icon->setPixmap(QPixmap(appIcon).scaled(24, 22, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

  // App Name
  app_name = new Label(false, "Segoe UI", 11, QFont::Normal, false, "Vaultorix");
   
  // Seperator
  seperator = new Seperator(nullptr, 18, 1, Qt::Vertical);
  seperator->raise();

  // Theme Mode Button
  theme_mode_btn = new Button;
  theme_mode_btn->setCursor(Qt::PointingHandCursor);
  theme_mode_btn->setSecondary(true);
  theme_mode_btn->setIconSize(QSize(16,16));
  theme_mode_btn->setDisplayMode(Button::IconOnly);
  theme_mode_btn->setFixedSize(QSize(26, 26));
  setInteractiveTitleBarWidget(theme_mode_btn);

  // Tooltip of theme mode button
  theme_mode_btn_tip = new ToolTip(theme_mode_btn);
  theme_mode_btn_tip->setText("Change theme mode");

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
  sign_out_btn->setGradientColor(true, "#FF59CD", "#FF3727");
  sign_out_btn->setHoverGradientColor("#FF3727");
  sign_out_btn->setFontProperties("Segoe UI", 10);
  sign_out_btn->setIconSize(QSize(16,16));
  sign_out_btn->setDisplayMode(Button::IconText);
  sign_out_btn->setFixedSize(QSize(100, 26));
  sign_out_btn->setPrimaryButtonIcon(signOutIcon);
  setInteractiveTitleBarWidget(sign_out_btn);

  // ToolTip of sign out button
  sign_out_btn_tip = new ToolTip(sign_out_btn);
  sign_out_btn_tip->setText("Sign out of your account");

  // Title Bar Layout
  titlebar_layout = new QHBoxLayout(titleBar());
  titlebar_layout->setSpacing(0);
  titlebar_layout->setContentsMargins(0, 0, 0, 0);

  titlebar_layout->addSpacing(13);
  titlebar_layout->addWidget(app_icon, 0, Qt::AlignLeft);
  titlebar_layout->addSpacing(10);
  titlebar_layout->addWidget(app_name, 0, Qt::AlignLeft);

  titlebar_layout->addStretch();

  titlebar_layout->addWidget(sign_out_btn, 0, Qt::AlignRight);
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
  statusbar = new Statusbar;

  // Layout
  content_layout = new QVBoxLayout(contentArea());
  content_layout->setContentsMargins(12, 0, 12, 10);
  content_layout->setSpacing(0);
  content_layout->addSpacing(4);
  content_layout->addWidget(toolbar, 0, Qt::AlignTop);
  content_layout->addSpacing(4);
  content_layout->addWidget(view);
  content_layout->addSpacing(4);
  content_layout->addWidget(statusbar);

  // Inital Theme
  emit themeModeChanged(false);
}

void VaultWindow::onthemeModeChanged(bool enable) {
  // Calling Base class member function of theme
  setDarkMode(enable);

  // App name
  enable ? app_name->setStyleSheet("color: white;") : app_name->setStyleSheet("color: black;");

  // Theme Mode Button
  theme_mode_btn->setDarkMode(enable);
  enable ? theme_mode_btn->setIconPaths(lightModeIcon, lightModeIcon) : theme_mode_btn->setIconPaths(darkModeIcon, darkModeIcon);
  theme_mode_btn_tip->setDarkMode(enable);

  // Toolbar theme
  toolbar->setDarkMode(enable);
  
  // View theme
  view->setDarkMode(enable);
  
  // Statusbar theme
  statusbar->setDarkMode(enable);

  // Sign out Button
  sign_out_btn_tip->setDarkMode(enable);
}
