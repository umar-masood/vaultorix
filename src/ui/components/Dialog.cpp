#include "Dialog.h"

Dialog::Dialog(QWidget *centralWidget, QWidget *parent, bool hasCloseButton) : SubWindow(centralWidget->size(), parent, hasCloseButton, false), contentWidget(centralWidget) {
   setFocusPolicy(Qt::StrongFocus);
   setModal(true);

   auto* layout = new QVBoxLayout(contentArea());
   layout->setSpacing(0);
   layout->setContentsMargins(0, 0, 0, 0);
   
   if (contentWidget)
      layout->addWidget(contentWidget, 0, Qt::AlignCenter);
}

void Dialog::setDarkMode(bool value) {
   if (isDarkMode == value) 
      return;
   
   isDarkMode = value;
   
   SubWindow::setDarkMode(isDarkMode);
}