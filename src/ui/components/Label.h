#pragma once

#include <QLabel>

class QTimer;
class QPropertyAnimation;
class QGraphicsOpacityEffect;
class QFont;
class QString;
class QColor;

class Label : public QLabel {
   Q_OBJECT 

   public:
   explicit Label(const QString &family = "Segoe UI",
                  int pointSize = 8,
                  QFont::Weight weight = QFont::Normal,
                  bool italic = false,
                  const QString &text = QString(),
                  Qt::Alignment alignment = Qt::AlignCenter,
                  QWidget *parent = nullptr);
   
   explicit Label(bool isIconic = false,
                  Qt::Alignment alignment = Qt::AlignCenter,
                  QWidget *parent = nullptr);

   void setTextColor(const QColor &color);
};


class AnimatedLabel : public QLabel {
   Q_OBJECT

   public:
   explicit AnimatedLabel(const QString &family = "Segoe UI",
                        int pointSize = 8,
                        QFont::Weight weight = QFont::Normal,
                        bool italic = false,
                        const QString &text = QString(),
                        Qt::Alignment alignment = Qt::AlignCenter,
                        QWidget *parent = nullptr);

   explicit AnimatedLabel(bool isIconic = false,
                        const QString &family = "Segoe UI",
                        int pointSize = 8,
                        QFont::Weight weight = QFont::Normal,
                        bool italic = false,
                        const QString &text = " ",
                        Qt::Alignment alignment = Qt::AlignCenter,
                        QWidget *parent = nullptr);

   void setAnimatedText(const QString &text);

   void show();
   void hide();

   private:
   void init();

   QGraphicsOpacityEffect *effect = nullptr;
   QPropertyAnimation *fadeIn = nullptr;
   QPropertyAnimation *fadeOut = nullptr;
};