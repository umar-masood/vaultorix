#include "AccountWindow.h"
/* ---------------- Stacked Widget ---------------- */

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

   if (this->rightWidget) {
      entireLayoutRight->removeWidget(this->rightWidget);
      this->rightWidget->hide();
      this->rightWidget->setParent(nullptr);
   }

   this->rightWidget = widget;
   entireLayoutRight->addWidget(rightWidget, 0, Qt::AlignCenter);

   QMetaObject::invokeMethod(
      widget,
      "setDarkMode",
      Qt::DirectConnection,
      Q_ARG(bool, isDarkMode)
   );
}

void AccountWindow::show() { if (w) w->show(); }
void AccountWindow::init() {
   if (w) return;

   // SubWindow Creation
   w = new SubWindow(QSize(1000, 720), nullptr, true, true);
   w->setWindowTitle("Vaultorix");
   w->setWindowIcon(QIcon(":/icons/AppBranding/app-icon.png"));

   // TitleBar Area
   QWidget *titleBar = w->titleBarArea();

   // Theme Mode Button
   themeMode = new Button(titleBar);
   themeMode->setSecondary(true);
   themeMode->setIconSize(QSize(16,16));
   themeMode->setDisplayMode(Button::IconOnly);
   themeMode->setFixedSize(QSize(26, 26));
   themeMode->setIconPaths(lightModeIcon, lightModeIcon);

   // Seperator 
   seperator = new Seperator(titleBar, 18, 1, Qt::Vertical);

   int x = titleBar->width() - 26*2 - 5*3 - 4;
   int y = (titleBar->height() - 18) / 2;

   seperator->move(x, y);
   seperator->raise();

   y = (titleBar->height() - 26) / 2;
   themeMode->move(x - 26 - 8, y );
   themeMode->raise();

   connect(themeMode, &Button::clicked, this, [this](){
      isDarkMode = !isDarkMode;
      emit themeModeChanged(isDarkMode);
   });

   // Left Panel
   left = new QWidget(w->contentArea());
   left->setGeometry(0, 0, w->width() / 2, w->height());
   left->setStyleSheet("background-color: #003F66;");

   brand = new Branding;
   mainPoint = new QLabel("Protect What Matters Most");
   mainPoint->setStyleSheet("color: white;");
   mainPoint->setAlignment(Qt::AlignLeft);

   QFont font;
   font.setPixelSize(24);
   font.setFamily("Segoe UI");
   font.setWeight(QFont::DemiBold);
   mainPoint->setFont(font);

   point1 = new BulletPoint("AES-256 encryption trusted worldwide", ":/icons/AppBranding/lock.svg");
   point2 = new BulletPoint("Quick file encryption & decryption", ":/icons/AppBranding/folder.svg");
   point3 = new BulletPoint("Two-factor authentication by default ", ":/icons/AppBranding/2FA.svg");
   point4 = new BulletPoint("Auto-Lock for maximum privacy", ":/icons/AppBranding/timer.svg");
   point5 = new BulletPoint("Backup & recovery on the way", ":/icons/AppBranding/backup.svg");

   points = {point1, point2, point3, point4, point5};

   entireLayoutLeft = new QVBoxLayout;
   entireLayoutLeft->setContentsMargins(30, 30, 24, 30);
   entireLayoutLeft->addWidget(brand, 0);
   entireLayoutLeft->addSpacing(20);
   entireLayoutLeft->addWidget(mainPoint, 0);
   entireLayoutLeft->addSpacing(16);

   for (int i = 0; i < points.size(); ++i) {
      entireLayoutLeft->addWidget(points[i], 0);
      if (i < points.size() - 1)
         entireLayoutLeft->addSpacing(16);
   }

   illustration = new QLabel;
   illustration->setAttribute(Qt::WA_TranslucentBackground);
   illustration->setFixedSize(QSize(340, 320));
   illustration->setAlignment(Qt::AlignCenter);
   illustration->setPixmap(QPixmap(":/icons/AppBranding/illustration.png").scaled(QSize(340, 320), Qt::KeepAspectRatio, Qt::SmoothTransformation));

   entireLayoutLeft->addSpacing(16);
   entireLayoutLeft->addWidget(illustration, 0, Qt::AlignCenter);
   entireLayoutLeft->addStretch();
   left->setLayout(entireLayoutLeft);

   // Right Panel
   right = new QWidget(w->contentArea());
   right->setGeometry(left->width(), 0, w->width() / 2, w->height());
   right->setAttribute(Qt::WA_TranslucentBackground);

   entireLayoutRight = new QVBoxLayout(right);
   entireLayoutRight->setContentsMargins(30, 30, 30, 30);
   entireLayoutRight->setSpacing(0);
   
   if (rightWidget)
      entireLayoutRight->addWidget(rightWidget, 0, Qt::AlignCenter);

   // Theme Connection
   connect(this, &AccountWindow::themeModeChanged, this, &AccountWindow::onthemeModeChanged);
}

void AccountWindow::onthemeModeChanged(bool enable) {
   if (w) w->setDarkMode(enable);

   if (themeMode) {
      themeMode->setDarkMode(enable);
      enable ? themeMode->setIconPaths(darkModeIcon, darkModeIcon) : themeMode->setIconPaths(lightModeIcon, lightModeIcon);
   }

   if (rightWidget)
      QMetaObject::invokeMethod(rightWidget, "setDarkMode", Q_ARG(bool, enable));

   for (auto *subWidget : subWidgets) 
      QMetaObject::invokeMethod(subWidget, "setDarkMode", Q_ARG(bool, enable));
}

void AccountWindow::setSubWidgets(const QVector<QWidget *> subWidgets) { this->subWidgets = subWidgets; }