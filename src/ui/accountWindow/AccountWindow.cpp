#include "AccountWindow.h"

/* ---------------- Branding ---------------- */
Branding::Branding(QWidget *parent) : QWidget(parent) {
   setFixedSize(QSize(220, 60));
   setAttribute(Qt::WA_TranslucentBackground);
   logo = QPixmap(":/icons/AppBranding/app-icon.png").scaled(54, 50, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
}

void Branding::paintEvent(QPaintEvent *) {
   QPainter painter(this);
   painter.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
   painter.drawPixmap(2, 2, logo);

   QFont font;
   font.setPixelSize(30);
   font.setWeight(QFont::Bold);
   font.setFamily("Inter");

   painter.setFont(font);
   painter.setPen(QColor("white"));
   painter.drawText(logo.width() + 13, 30, "Vaultorix");

   font.setPixelSize(12);
   font.setWeight(QFont::Medium);
   font.setFamily("Segoe UI");

   painter.setFont(font);
   painter.setPen(QColor("white"));
   painter.drawText(logo.width() + 14, 46, "Simple  Powerful  Secure");
}

/* ---------------- Bullet Point ---------------- */
BulletPoint::BulletPoint(const QString &text, const QString &iconPath, QWidget *parent) : QLabel(parent), text(text) {
   setAttribute(Qt::WA_TranslucentBackground);
   icon = QPixmap(iconPath).scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation);
   setFixedSize(22 + 12 + QFontMetrics(font()).horizontalAdvance(text), 22);
}

void BulletPoint::paintEvent(QPaintEvent *) {
   QPainter painter(this);
   painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

   QRect rec(0, 1, 20, 20);
   if (!icon.isNull()) {
      int xPos = rec.x() + (rec.width() - icon.width()) / 2;
      int yPos = rec.y() + (rec.height() - icon.height()) / 2;
      painter.drawPixmap(xPos, yPos, icon);
   }

   painter.setFont(font());
   painter.setPen(QPen(QColor("white")));

   QRect textRec(rec.right() + 13, 0, width() - rec.width() - 12, height());
   painter.drawText(textRec, Qt::AlignVCenter | Qt::AlignLeft, text);
}

QFont BulletPoint::font() const {
   QFont fnt;
   fnt.setPixelSize(16);
   fnt.setFamily("Segoe UI");
   fnt.setWeight(QFont::DemiBold);
   return fnt;
}

/* ---------------- Account Window ---------------- */
AccountWindow::AccountWindow(QWidget *rightWidget, QObject *parent, const QVector<QWidget *> &subWidgets) : QObject(parent), rightWidget(rightWidget) {
   init();
}

void AccountWindow::setDarkMode(bool value) {
   isDarkMode = value;
   emit themeModeChanged(isDarkMode);
}

SubWindow *AccountWindow::subWindow() const { return w; }

void AccountWindow::setRightWidget(QWidget *widget) {
   if (!widget || !right)
      return;

   // Hide and Delete if there's already existed right widget
   if (this->rightWidget) {
      entireLayoutRight->removeWidget(this->rightWidget);
      this->rightWidget->hide();
      this->rightWidget->setParent(nullptr);
   }

   this->rightWidget = widget;
   this->rightWidget->setParent(this->right);

   // Smooth Opacity Effect
   SmoothOpacity *effect = new SmoothOpacity;
   effect->setOpacity(0.0);
   rightWidget->setGraphicsEffect(effect);

   // Adding Right Widget into the layout
   entireLayoutRight->addWidget(rightWidget, 0, Qt::AlignCenter);

   // Opacity Effect Animation
   QPropertyAnimation *ani = new QPropertyAnimation(effect, "opacity", rightWidget);
   ani->setEasingCurve(QEasingCurve::InOutQuad);
   ani->setDuration(600);
   ani->setStartValue(0.0);
   ani->setEndValue(1.0);
   ani->start(QAbstractAnimation::DeleteWhenStopped);
   connect(ani, &QPropertyAnimation::finished, [=](){
      ani->deleteLater();
      rightWidget->setGraphicsEffect(nullptr); 
   });

   QMetaObject::invokeMethod(widget, "setDarkMode", Qt::DirectConnection, Q_ARG(bool, isDarkMode));
}

void AccountWindow::show() { if (w) w->show(); }
void AccountWindow::init() {
   if (w) return;

   // SubWindow Creation
   w = new SubWindow(QSize(1000, 720), nullptr, true, true);
   w->setWindowTitle("Vaultorix");
   w->setWindowIcon(QIcon(":/icons/AppBranding/app-icon.png"));

   // Main Layout
   mainLayout = new QHBoxLayout(w->contentArea());
   mainLayout->setContentsMargins(0, 0, 0, 0);
   mainLayout->setSpacing(0);

   // TitleBar Area
   QWidget *titleBar = w->titleBarArea();

   // Theme Mode Button
   themeMode = new Button(titleBar);
   themeMode->setSecondary(true);
   themeMode->setIconSize(QSize(16,16));
   themeMode->setDisplayMode(Button::IconOnly);
   themeMode->setFixedSize(QSize(26, 26));
   themeMode->setIconPaths(lightModeIcon, lightModeIcon);
   connect(themeMode, &Button::clicked, this, [this](){
      isDarkMode = !isDarkMode;
      emit themeModeChanged(isDarkMode);
   });

   // Theme Button Tooltip
   themeButtonTip = new ToolTip(themeMode);
   themeButtonTip->setText("Change theme mode");

   // Seperator 
   seperator = new Seperator(titleBar, 18, 1, Qt::Vertical);
   int x = titleBar->width() - 26*2 - 5*3 - 4;
   int y = (titleBar->height() - 18) / 2;
   seperator->move(x, y);
   seperator->raise();

   // Theme Button Positioning
   y = (titleBar->height() - 26) / 2;
   themeMode->move(x - 26 - 8, y );
   themeMode->raise();

   // Left Panel
   left = new QWidget;
   left->setStyleSheet("background-color: #003F66;");

   // Branding of App
   brand = new Branding;

   // Tagline
   tagline = new QLabel("Protect What Matters Most");
   tagline->setStyleSheet("color: white;");
   tagline->setAlignment(Qt::AlignLeft);

   // Setting Font on Tagline
   QFont font;
   font.setPixelSize(24);
   font.setFamily("Segoe UI");
   font.setWeight(QFont::DemiBold);
   tagline->setFont(font);

   // Features Bullet Points
   point1 = new BulletPoint("AES-256 encryption trusted worldwide", ":/icons/AppBranding/lock.svg");
   point2 = new BulletPoint("Quick file encryption & decryption", ":/icons/AppBranding/folder.svg");
   point3 = new BulletPoint("Two-factor authentication by default ", ":/icons/AppBranding/2FA.svg");
   point4 = new BulletPoint("Auto-Lock for maximum privacy", ":/icons/AppBranding/timer.svg");
   point5 = new BulletPoint("Backup & recovery on the way", ":/icons/AppBranding/backup.svg");
   points = {point1, point2, point3, point4, point5};

   // left Side Layout
   entireLayoutLeft = new QVBoxLayout(left);
   entireLayoutLeft->setContentsMargins(30, 30, 24, 30);
   entireLayoutLeft->addWidget(brand, 0);
   entireLayoutLeft->addSpacing(20);
   entireLayoutLeft->addWidget(tagline, 0);
   entireLayoutLeft->addSpacing(16);
   for (int i = 0; i < points.size(); ++i) {
      entireLayoutLeft->addWidget(points[i], 0);
      if (i < points.size() - 1)
         entireLayoutLeft->addSpacing(16);
   }

   // Illustration
   illustration = new QLabel;
   illustration->setAttribute(Qt::WA_TranslucentBackground);
   illustration->setFixedSize(QSize(340, 320));
   illustration->setAlignment(Qt::AlignCenter);
   illustration->setPixmap(QPixmap(":/icons/AppBranding/illustration.png").scaled(QSize(340, 320), Qt::KeepAspectRatio, Qt::SmoothTransformation));

   entireLayoutLeft->addSpacing(16);
   entireLayoutLeft->addWidget(illustration, 0, Qt::AlignCenter);
   entireLayoutLeft->addStretch();

   // Right Panel
   right = new QWidget;

   // Right Side Layout
   entireLayoutRight = new QVBoxLayout(right);
   entireLayoutRight->setContentsMargins(30, 30, 30, 30);
   entireLayoutRight->setSpacing(0);
   if (rightWidget)
      entireLayoutRight->addWidget(rightWidget, 0, Qt::AlignCenter);

   // Adding Left and Right Panels into main layout
   mainLayout->addWidget(left);
   mainLayout->addWidget(right);

   // Theme Connection
   connect(this, &AccountWindow::themeModeChanged, this, &AccountWindow::onthemeModeChanged);
}

void AccountWindow::onthemeModeChanged(bool enable) {
   if (w) w->setDarkMode(enable);

   if (themeMode) {
      themeMode->setDarkMode(enable);
      themeButtonTip->setDarkMode(enable);
      enable ? themeMode->setIconPaths(darkModeIcon, darkModeIcon) : themeMode->setIconPaths(lightModeIcon, lightModeIcon);
   }

   if (rightWidget)
      QMetaObject::invokeMethod(rightWidget, "setDarkMode", Q_ARG(bool, enable));

   for (auto *subWidget : subWidgets) 
      QMetaObject::invokeMethod(subWidget, "setDarkMode", Q_ARG(bool, enable));
}

void AccountWindow::setSubWidgets(const QVector<QWidget *> subWidgets) { 
   for (QWidget *w : subWidgets) 
      if (!this->subWidgets.contains(w)) 
         this->subWidgets.append(w);
}