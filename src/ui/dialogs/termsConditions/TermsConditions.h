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
   bool isDarkMode = false;

   ScrollBar *scrollBar = nullptr;
   QTextBrowser *termsConds = nullptr;
   Label *heading = nullptr;
   QVBoxLayout *layout = nullptr;
};
