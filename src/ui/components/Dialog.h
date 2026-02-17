#pragma once

#include "../windows/subWindow/SubWindow.h"

#include <QVBoxLayout>
#include <QApplication>

class Dialog : public SubWindow {
   Q_OBJECT

   public:
   Dialog(QWidget *centralWidget = nullptr, QWidget *parent = nullptr, bool hasCloseButton = true);
   Q_INVOKABLE void setDarkMode(bool value);
   
   private:
   QWidget *contentWidget = nullptr;
   bool isDarkMode = false;
};

