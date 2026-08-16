#include "SpinnerProgress.h"
#include "SmoothOpacity.h"

#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QFont>
#include <QTimer>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QtMath>
#include <QDebug>
#include <algorithm>

SpinnerProgress::SpinnerProgress(QWidget *parent) : QWidget(parent) {
   hide();
   setAttribute(Qt::WA_TranslucentBackground, true);

   opacity = new SmoothOpacity;
   setGraphicsEffect(opacity);

   animation = new QPropertyAnimation(opacity, "opacity");
   animation->setDuration(300);
   animation->setEasingCurve(QEasingCurve::InOutQuad);

   loadDefaultColors();
}

void SpinnerProgress::fadeIn() {
   animation->stop();
   disconnect(animation, &QPropertyAnimation::finished, nullptr, nullptr);
   animation->setStartValue(0.0);
   animation->setEndValue(1.0);
   animation->start();
   show();
}

void SpinnerProgress::fadeOut() {
   animation->stop();
   disconnect(animation, &QPropertyAnimation::finished, nullptr, nullptr);
   animation->setStartValue(1.0);
   animation->setEndValue(0.0);
   connect(animation, &QPropertyAnimation::finished, this, &QWidget::hide);
   animation->start();
}

void SpinnerProgress::start() {
   if (timer && !timer->isActive() && isIndeterminate) 
      timer->start(16); // ~60 FPS
   
   fadeIn();
}

void SpinnerProgress::stop() {
   if (isIndeterminate && timer) 
     timer->stop();
   
   fadeOut();
}

void SpinnerProgress::setDarkMode(bool enable) {
   isDarkMode = enable;
   loaderTextColor = isDarkMode ? QColor("#F1F5F9") : QColor("#111827");
   update();
}

void SpinnerProgress::setIndeterminate(bool enable) {
   isIndeterminate = enable;
   if (isIndeterminate && !timer) {
      timer = new QTimer(this);
      connect(timer, &QTimer::timeout, this, [=]() {
         angle = (angle + 5) % 360;
         update();
      });
   }
}

void SpinnerProgress::setRange(int min, int max) {
   if (min < 0 || max > 100 || min >= max) {
      minimum = 0; maximum = 100;
   } else {
      minimum = min; maximum = max;
   }
}

void SpinnerProgress::setValue(int value) {
   if (value > 100 || value < 0) {
      qWarning() << "Error: Spinner Progress :: Your Progress current value is invalid";
      return;
   }

   currentValue = value;
   update();
}

int SpinnerProgress::getValue() const { return currentValue; }

void SpinnerProgress::loadDefaultColors() {
   _colors[BackgroundLight]   = QColor("#F2F2F2");
   _colors[BackgroundDark]    = QColor("#383838");
   _colors[ForegroundLight]   = QColor("#109AC7");
   _colors[ForegroundDark]    = QColor("#109AC7");
   loaderTextColor = isDarkMode ? "#F1F5F9" : "#111827";
}

void SpinnerProgress::setColor(const SpinnerColor &state, const QColor &color) {
   _colors[state] = color;
   update();
}

void SpinnerProgress::setTextColor(const QColor &color) {
   loaderTextColor = color;
   update();
}


QColor SpinnerProgress::color(const SpinnerColor &state) const { return _colors.value(state, Qt::transparent); }

void SpinnerProgress::setArcWidth(int w) {
   arcWidth = w;
   update();
}

void SpinnerProgress::paintEvent(QPaintEvent *) {
   QPainter painter(this);
   painter.setRenderHint(QPainter::Antialiasing);
   painter.setBrush(Qt::NoBrush);

   // Background arc
   QPen pen;
   pen.setWidth(arcWidth);
   pen.setCapStyle(Qt::RoundCap);
   pen.setJoinStyle(Qt::RoundJoin);
   pen.setColor(isDarkMode ? color(BackgroundDark) : color(BackgroundLight));

   int size = qMin(width(), height()) - 2 * margin;
   QRectF rec((width() - size) / 2, (height() - size) / 2, size, size);

   painter.setPen(pen);
   painter.drawArc(rec, 0, 360 * 16);

   // Foreground arc
   pen.setColor(isDarkMode ? color(ForegroundDark) : color(ForegroundLight));
   painter.setPen(pen);

   QPoint center(width() / 2, height() / 2);

   if (isIndeterminate) {
      painter.save();
      painter.translate(center);
      painter.rotate(angle);
      painter.translate(-center);
      painter.drawArc(rec, 0, 270 * 16);
      painter.restore();
   } else {
      double percent = static_cast<double>(currentValue - minimum) / (maximum - minimum);
      int spanAngle = static_cast<int>(percent * 360 * 16);

      painter.drawArc(rec, 90 * 16, -spanAngle);

      // Percentage Text
      QFont font("Segoe UI", 10, QFont::Normal);
      painter.setFont(font);
      painter.setPen(loaderTextColor);
      painter.drawText(rec, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(static_cast<int>(static_cast<double>(currentValue - minimum) / (maximum - minimum) * 100)) + "%");
   }
}

