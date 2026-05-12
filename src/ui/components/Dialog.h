#pragma once

#include "../windows/subwindow/Subwindow.h"

class QVBoxLayout;

class Dialog : public SubWindow {
   Q_OBJECT

   public:
   Dialog(QWidget *centralWidget = nullptr, QWidget *parent = nullptr, bool hasCloseButton = true);
   Q_INVOKABLE void setDarkMode(bool value);
   
   private:
   QWidget *contentWidget = nullptr;
   bool isDarkMode = false;
};

