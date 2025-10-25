#include "OTPWidget.h"

OTPWidget::OTPWidget(QWidget *parent) : QWidget(parent)
{
   setAttribute(Qt::WA_TranslucentBackground);
   setFixedSize(QSize(360, 60));

   auto *layout = new QHBoxLayout(this);
   layout->addStretch();
   layout->setSpacing(0);

   QFont font;
   font.setFamily("Segoe UI");
   font.setBold(true);
   font.setPointSize(22);

   for (int i = 0; i < boxes; i++)
   {
      auto *box = new QLabel;
      box->setFont(font);
      box->setAlignment(Qt::AlignCenter);
      box->setStyleSheet(normalStyle);
      box->setFixedSize(QSize(46, 46));
      box->setContentsMargins(0, 0, 0, 3);
      OTPBoxes.append(box);
      layout->addWidget(box);
      if (i < 4)
         layout->addSpacing(28);
   }

   layout->addStretch();
   setFocusPolicy(Qt::StrongFocus);

   updateStyles();
   updateHighlight();
}

void OTPWidget::setDarkMode(bool value)
{
   isDarkMode = value;
   updateStyles();
   updateHighlight();
}

void OTPWidget::keyPressEvent(QKeyEvent *event)
{
   const int n = OTPBoxes.size();
   if ((event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) ||
       (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z))
   {
      QString text = event->text().toUpper();
      if (!text.isEmpty() && currentIndex >= 0 && currentIndex < n)
      {
         OTPBoxes[currentIndex]->setText(text);
         currentIndex++;
         if (currentIndex >= n)
            currentIndex = n - 1;
         updateHighlight();
      }

      if (currentIndex == n - 1 && !OTPBoxes[n - 1]->text().isEmpty())
      {
         QString c;
         for (auto *i : OTPBoxes)
            c.append(i->text());
         emit OTPcompleted(c);
      }
   }
   else if (event->key() == Qt::Key_Backspace)
   {
      if (currentIndex >= 0 && currentIndex < n)
      {
         if (!OTPBoxes[currentIndex]->text().isEmpty())
         {
            OTPBoxes[currentIndex]->clear();
         }
         else
         {
            if (currentIndex > 0)
               currentIndex--;
            OTPBoxes[currentIndex]->clear();
            updateHighlight();
         }
      }
   }
}

void OTPWidget::updateStyles()
{
   QString bg_color_normal = isDarkMode ? "#2D2D2D" : "#FBFBFB";
   QString bg_color_focused = isDarkMode ? "#1F1F1F" : "#FFFFFF";
   QString text_color = isDarkMode ? "white" : "black";
   QString border_color = isDarkMode ? "#4D4D4D" : "#CCCCCC";

   normalStyle = QString(
                     "background-color: %1; border-radius: 7px; border: 1px solid %2; color: %3;")
                     .arg(bg_color_normal)
                     .arg(border_color)
                     .arg(text_color);

   focusedStyle = QString(
                      "background-color: %1; border: 2px solid #109AC7; border-radius: 7px; color: %2;")
                      .arg(bg_color_focused)
                      .arg(text_color);
}

void OTPWidget::updateHighlight()
{
   for (int i = 0; i < OTPBoxes.size(); i++)
   {
      if (i == currentIndex)
         OTPBoxes[i]->setStyleSheet(focusedStyle);
      else
         OTPBoxes[i]->setStyleSheet(normalStyle);
   }
}
