#include "User.h"

User::User(QWidget *parent) : QWidget(parent) {
  setAttribute(Qt::WA_Hover);
  setAvator(QPixmap(IconManager::icon(Icons::Avator)), 36);
}

void User::setDarkMode(bool enable) { 
  isDarkMode = enable; 
  update(); 
}

void User::setName(const QString &name) { 
  _name = name;
  adjustWidgetSize();
}

void User::adjustWidgetSize() {
  QFontMetrics fm(font());
  int textW = fm.horizontalAdvance(_name);

  const int spacing = 12;
  int totalW = 12 + avator.width() + 12 + textW + 12;

  setFixedSize(totalW, 58);
}

QFont User::font() {
  QFont fnt;
  fnt.setFamily("Segoe UI");
  fnt.setPixelSize(14);
  fnt.setWeight(QFont::Medium);
  return fnt;
}

void User::setAvator(const QPixmap &pixmap, int size) {
  if (pixmap.isNull()) {
    qWarning() << "\nError: setAvator() :: Loading avator";
    return;
  }
   
  // Getting minimum dimension
  int s = qMin(pixmap.width(), pixmap.height());

  // Define the Square 
  QRect square((pixmap.width() - s) / 2, (pixmap.height() - s) / 2, s, s);

  // Extracting the defined square from whole image
  QPixmap image = pixmap.copy(square).scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  // Transparent Pixmap of 'size'
  QPixmap circle(size, size);
  circle.fill(Qt::transparent);

  // Painting on Transparent Pixmap
  QPainter painter(&circle);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

  // Defining circular path for clipping on Transparent Pixmap
  QPainterPath path;
  path.addRoundedRect(0, 0, size, size, size / 2, size / 2);

  // Clip circle on transparent pixmap
  painter.setClipPath(path);

  // Draw squared image inside this circle
  painter.drawPixmap(0, 0, image);

  // Inserting into class member for paint event
  this->avator = circle;
}

void User::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

  // Background
  QColor brushColor;

  if (isPressed)
    brushColor = isDarkMode ? "#242424" : "#FFFFFF";
  else if (isHover)
    brushColor = isDarkMode ? "#323232" : "#F0F0F0";
  else if (!isEnabled())
    brushColor = isDarkMode ? "#555555" : "#E0E0E0";
  else
    brushColor = Qt::transparent;

  painter.setBrush(brushColor);
  painter.setPen(Qt::NoPen);
  painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 6, 6);

  // Avator
  int avatarY = (height() - avator.height()) / 2;
  painter.drawPixmap(12, avatarY, avator.width(), avator.height(), avator);

  // Name Text
  painter.setFont(font());
  painter.setPen(isDarkMode ? Qt::white : Qt::black);

  int textX = 12 + avator.width() + 12;
  int textW = width() - textX - 12;

  QRect textRect(textX, 0, textW, height());
  painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, _name);

}

void User::mousePressEvent(QMouseEvent *event) {
  isPressed = true;

  if (event->button() == Qt::LeftButton)
    emit clicked();

  QWidget::mousePressEvent(event);
  update();
}

void User::mouseReleaseEvent(QMouseEvent *event) {
  isPressed = false;
  QWidget::mouseReleaseEvent(event);
  update();
}

void User::enterEvent(QEnterEvent *event) {
  isHover = true;
  QWidget::enterEvent(event);
  update();
}

void User::leaveEvent(QEvent *event) {
  isHover = false;
  QWidget::leaveEvent(event);
  update();
}
