#include "EmptyState.h"

#include "../../../../resources/IllustrationManager.h" 
#include "../../components/Label.h"

#include <QVBoxLayout>
#include <QPixmap>

using Ui::Vault::EmptyState;
EmptyState::EmptyState(QWidget *parent) : QWidget(parent) {
  setAttribute(Qt::WA_Hover, true);  
  setMouseTracking(true);             
  setAttribute(Qt::WA_TransparentForMouseEvents, false);
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setFixedSize(QSize(240, 240));

  // Illustration
  illustration = new Label(true);
  illustration->setFixedSize(QSize(140, 140));
  illustration->setScaledContents(true);

  // Text
  text = new Label("Segoe UI", 10, QFont::Normal, false, "Import files into your vault\nClick here to add files into your vault and protect them securely.");
  text->setWordWrap(true);
  text->setStyleSheet("color: black;");
   
  // Layout
  layout = new QVBoxLayout(this);
  layout->setContentsMargins(16, 16, 16, 16);
  layout->setSpacing(0);
  layout->addWidget(illustration, 0, Qt::AlignCenter);
  layout->addSpacing(4);
  layout->addWidget(text, 0, Qt::AlignCenter);

  // Initial theme
  setDarkMode(isDarkMode);
}

void EmptyState::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

  // Border
  QPen pen;
  pen.setWidth(2);
  pen.setStyle(Qt::CustomDashLine);
  pen.setDashPattern({2, 3});
  pen.setColor(QColor(isDarkMode ? "#474747" : "#dddddd"));
  painter.setPen(pen);

  // Background
  QColor bg_color;
  if (isPressed) 
    bg_color = isDarkMode ? "#242424" : "#FFFFFF";
  else if (isHover)
    bg_color = isDarkMode ? "#323232" : "#F2F2F2";
  else
    bg_color = Qt::transparent;

  painter.setBrush(QBrush(bg_color));

  // Drawing Rectangle
  painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 12, 12);
}

void EmptyState::setDarkMode(bool enable) {
  isDarkMode = enable;
  
  // Illustration Change
  illustration->setPixmap(QPixmap(enable ? IllustrationManager::illustration(Illustrations::FolderDark) :  IllustrationManager::illustration(Illustrations::FolderLight))
                        .scaled(140, 140, Qt::KeepAspectRatio, Qt::SmoothTransformation));

  // Text theme
  text->setStyleSheet(QString("color: %1;").arg(enable ? "#707070" : "#BFBFBF"));

  update();
}

void EmptyState::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    isPressed = true;
    update();    
    emit clicked();
    event->accept();
    return;
  }

  QWidget::mousePressEvent(event);
}

void EmptyState::mouseReleaseEvent(QMouseEvent *event) {
  isPressed = false;
  QWidget::mouseReleaseEvent(event);
  update();
}

void EmptyState::enterEvent(QEnterEvent *event) {
  isHover = true;
  setCursor(Qt::PointingHandCursor);

  QWidget::enterEvent(event);
  update();
}

void EmptyState::leaveEvent(QEvent *event) {
  isHover = false;
  isPressed = false;

  setCursor(Qt::ArrowCursor);

  QWidget::leaveEvent(event);
  update();
}