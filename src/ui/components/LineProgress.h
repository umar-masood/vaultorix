#pragma once

#include <QWidget>
#include <QHash>

class QPainter;
class QPaintEvent;
class QString;
class QColor;
class QTimer;
class QPropertyAnimation;
class SmoothOpacity;

class LineProgress : public QWidget {
   Q_OBJECT

   public:
   enum LineColor {
      BackgroundLight,
      BackgroundDark,
      ForegroundLight,
      ForegroundDark
   };

   explicit LineProgress(QWidget *parent = nullptr);

   void setColor(const LineColor &state, const QColor &color);    
   void setDarkMode(bool value);
   void setText(const QString &text);
   
   void setIndeterminate(bool value);  
   bool indeterminate() const;

   void setTrackHeight(int h); 

   void start();
   void stop();

   void setValue(int value);
   int getValue() const;

    inline uint qHash(const LineColor &state, uint seed = 0) {
        return ::qHash(static_cast<int>(state), seed);
    }

   protected:
   void paintEvent(QPaintEvent *event) override;

   signals:
   void completed();
   
   private: 
   void fadeIn();
   void fadeOut();
   void loadDefaultColors();
   QColor color(const LineColor &state) const;

   // Colors
   QHash<LineColor, QColor> _colors;

   bool isDarkMode = false;
   bool isIndeterminate = false;
   bool isCompleted = false;

   double offset = 0.0;
   double currentValue = 0.0;

   double minimum = 0.0;
   double maximum = 1.0;

   const int margin = 12;
   int trackHeight = 6;
   const int radius = trackHeight / 2;

   QTimer *timer = nullptr;   
   QPropertyAnimation *animation = nullptr;
   SmoothOpacity *opacity = nullptr;

   QString loaderText;
};
