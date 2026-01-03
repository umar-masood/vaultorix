#pragma once

#include <QTextBrowser>

#include "../../windows/subWindow/SubWindow.h"
#include "../../accountOTP/AccountOTP.h"
#include "../../accountWindow/AccountWindow.h"
#include "../../components/ScrollBar.h"

class TermsConditions : public QWidget {
   Q_OBJECT

   public:
   explicit TermsConditions(QWidget *parent = nullptr);
   Q_INVOKABLE void setDarkMode(bool value);

   signals:
   void themeModeChanged(bool enable);

   private:
   // Theme Mode Flag
   bool isDarkMode = false;

   // ScrollBar
   ScrollBar *scrollBar = nullptr;

   // Terms & Conditions HTML Page View Widget
   QTextBrowser *termsConds = nullptr;

   // Heading
   Label *heading = nullptr;

   // Main Layout
   QVBoxLayout *layout = nullptr;
};
