#pragma once
#include "SmoothOpacity.h"

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QColor>
#include <QFont>
#include <QTimer>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QShowEvent>
#include <QString>
#include <QtMath>
#include <algorithm>
#include <QHash>

class SpinnerProgress : public QWidget {
   Q_OBJECT

   public:
   enum SpinnerColor {
     BackgroundLight,
     BackgroundDark,
     ForegroundLight,
     ForegroundDark,
   };

   explicit SpinnerProgress(QWidget *parent = nullptr);


   void start();
   void stop();
   void setDarkMode(bool enable);
   void setIndeterminate(bool enable);
   void setRange(int min, int max);
   void setColor(const SpinnerColor &state, const QColor &color);
   void setTextColor(const QColor &color);
   void setArcWidth(int w);

   void setValue(int value);
   int getValue() const;

   inline uint qHash(const SpinnerColor &state, uint seed = 0) {
      return ::qHash(static_cast<int>(state), seed);
   }

   protected:
   void paintEvent(QPaintEvent *event) override;

   private:
   void fadeIn();
   void fadeOut();

   void loadDefaultColors();
   QColor color(const SpinnerColor &state) const;

   bool isDarkMode = false;
   bool isIndeterminate = false;

   const int margin = 8;
   int angle, minimum = 0, maximum = 0, currentValue = 0, arcWidth = 6;

   // Colors
   QHash<SpinnerColor, QColor> _colors;

   QTimer *timer = nullptr;

   QColor loaderTextColor;
   QString loaderText;

   QPropertyAnimation *animation = nullptr;
   SmoothOpacity *opacity = nullptr;
};

