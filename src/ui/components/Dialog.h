#pragma once

#include "../windows/subwindow/Subwindow.h"
#include <QPointer>

class QVBoxLayout;

class Dialog : public SubWindow {
   Q_OBJECT

   public:
   Dialog(QPointer<QWidget> centralWidget = nullptr, QWidget *parent = nullptr, bool hasCloseButton = true);
   Q_INVOKABLE void setDarkMode(bool value);
   
   private:
   QPointer<QWidget> contentWidget = nullptr;
   bool isDarkMode = false;
};

