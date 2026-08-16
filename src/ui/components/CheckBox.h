#pragma once

#include <QWidget>
#include <QPainter>
#include <QEvent>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QPixmap>
#include <QString>
#include <QFont>

class QVariantAnimation;

class CheckBox : public QWidget {
   Q_OBJECT
   
   public:
   explicit CheckBox(const QString &text = "", QWidget *parent = nullptr);
   void setDarkMode(bool value);
   bool isChecked() const;
   void setChecked(bool value);

   protected:
   void paintEvent(QPaintEvent *event) override;
   void enterEvent(QEnterEvent *event) override;
   void leaveEvent(QEvent *event) override;
   void mousePressEvent(QMouseEvent *event) override;

   private:
   QFont font() const;

   // Animation functions
   void startCheckAnimation(bool checked);
   void drawAnimatedCheck(QPainter &painter, const QRectF &rec, qreal progress);

   bool isDarkMode = false;
   bool isHover = false;
   bool _isChecked = false;

   // Animation values
   qreal checkProgress = 0.0;
   QVariantAnimation *checkAnimation = nullptr;

   QPixmap pixmap;
   QString text;

   signals:
   void toggled(bool checked);
};