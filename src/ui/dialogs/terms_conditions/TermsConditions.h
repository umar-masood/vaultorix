#pragma once

#include <QWidget>

class ScrollBar;
class Label;
class QVBoxLayout;
class QTextBrowser;

class TermsConditions : public QWidget {
   Q_OBJECT

   public:
   explicit TermsConditions(QWidget *parent = nullptr);
   void setDarkMode(bool value);

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
