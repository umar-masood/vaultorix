#include "Dialog.h"

Overlay::Overlay(QWidget *parent) : QWidget(parent) {
   setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
   setAttribute(Qt::WA_TranslucentBackground);
   setAttribute(Qt::WA_TransparentForMouseEvents, true);
   setMouseTracking(false);
}

void Overlay::paintEvent(QPaintEvent *event) {
   Q_UNUSED(event);

   // Colors
   QColor BG = QColor(0,0,0,80);

   QPainter painter(this);
   painter.setRenderHints(QPainter::Antialiasing);
   painter.setBrush(BG);
   painter.setPen(Qt::NoPen);

   QPainterPath path;
   path.addRoundedRect(rect().adjusted(1.5, 1.5, -1.5, -1.5), 6, 6);
   painter.drawPath(path);
}

Dialog::Dialog(QWidget *centralWidget, QWidget *parent, bool hasCloseButton) : SubWindow(centralWidget->size(), parent, hasCloseButton, false), contentWidget(centralWidget) {
   setFocusPolicy(Qt::StrongFocus);
   setModal(true);
}

void Dialog::setDarkMode(bool value) {
   if (isDarkMode == value) 
      return;
   
   isDarkMode = value;
   SubWindow::setDarkMode(isDarkMode);
}

void Dialog::centerInParent() {
   if (parentWidget()) {
      QScreen *screen = QApplication::screenAt(QCursor::pos());
      if (!screen)
         screen = QApplication::primaryScreen();
         QRect parentRect = parentWidget()->geometry();
         int x = parentRect.x() + (parentRect.width() - width()) / 2;
         int y = parentRect.y() + (parentRect.height() - height()) / 2;
         move(x, y);
   }
}

void Dialog::showEvent(QShowEvent *event) {
   if (!setupDone) {
      setup();
      setupDone = true;
   }

   centerInParent();
   raise();
   SubWindow::showEvent(event);
}

void Dialog::setup() {
   auto* layout = new QVBoxLayout(this->contentArea());
   layout->setSpacing(0);
   layout->setContentsMargins(0, 0, 0, 0);
   
   if (contentWidget)
      layout->addWidget(contentWidget, 0, Qt::AlignCenter);
}