#include "User.h"

User::User(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_Hover);
    setFixedSize(210, 58);
    setAvator(QPixmap("C:/Users/umara/Downloads/App (1).png"), 36);
}

void User::setDarkMode(bool enable) { isDarkMode = enable; update(); }
void User::setEmail(const QString &email) { this->email = email; }
void User::setName(const QString &name) { this->name = name; }

void User::setAvator(const QPixmap &pixmap, int size) {
    if (pixmap.isNull()) {
        qWarning() << "\nError: Loading avator";
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
    QColor bg_color;
    if (isPressed)
        bg_color = isDarkMode ? "#2c2c2c" : "#FFFFFF";
    else if (isHover)
        bg_color = isDarkMode ? "#323232" : "#F0F0F0";
    else if (!isEnabled())
        bg_color = isDarkMode ? "#555555" : "#E0E0E0";
    else
        bg_color = Qt::transparent;

    painter.setBrush(bg_color);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 6, 6);

    // Avator
    painter.drawPixmap(QRect(12, (height() - avator.height()) / 2, 36, 36), avator);

    // Name Text
    QFont fnt;
    fnt.setFamily("Inter");
    fnt.setPixelSize(14);
    fnt.setBold(true);
    painter.setFont(fnt);

    // Font Metrics of Name Text
    QFontMetrics fm(fnt);
    int maxWidth = 138;
    QString elidedText = fm.elidedText(name, Qt::ElideRight, maxWidth);

    painter.setPen(isDarkMode ? "white" : "black");
    painter.drawText(12 + avator.width() + 12, 26, elidedText);

    // Email Text
    fnt.setPointSize(10);
    fnt.setBold(false);
    painter.setFont(fnt);

    // Font Metrics of Email Text
    fm = QFontMetrics(fnt);
    elidedText = fm.elidedText(email, Qt::ElideRight, maxWidth);

    painter.setPen(isDarkMode ? "#A4A4A4" : "#878787");
    painter.drawText(12 + avator.width() + 12, 44, elidedText);
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
