#pragma once

#include "Button.h"
#include <QWidget>
#include "../windows/subWindow/SubWindow.h"
#include <QScreen>
#include <QCursor>
#include <QColor>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QShowEvent>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QEvent>

class Overlay : public QWidget {
   Q_OBJECT
   public:
   explicit Overlay(QWidget *parent = nullptr);

   protected:
   void paintEvent(QPaintEvent *event) override;
};

class Dialog : public SubWindow 
{
   Q_OBJECT

public:
   Dialog(QWidget *centralWidget = nullptr, QWidget *parent = nullptr, bool closeBtn = true);
   Q_INVOKABLE void setDarkMode(bool value);

protected:
   void showEvent(QShowEvent *event) override;
   void resizeEvent(QResizeEvent *event) override;
   void closeEvent(QCloseEvent *event) override;
   bool eventFilter(QObject* obj, QEvent* event) override;
   
private:
   void centerInParent();

   QWidget *contentWidget = nullptr;
   Overlay *overlay = nullptr;
   void setup();

   bool isDarkMode = false;
   bool setupDone = false;
};

