#include "TermsConditions.h"

TermsConditions::TermsConditions(QWidget *parent) : QWidget(parent) {
   setFixedSize(500, 600);
   setAttribute(Qt::WA_TranslucentBackground);

   // Scroll Bar
   scrollBar = new ScrollBar(Qt::Vertical);

   // Heading
   heading = new Label(false, "Inter", 18, QFont::Bold, false, "Terms & Conditions", Qt::AlignLeft);
   heading->setStyleSheet("color: #0086CF;");

   // Terms & Conditions HTML Text View Widget
   termsConds = new QTextBrowser(this);
   termsConds->setSource(QUrl::fromLocalFile(":/terms/termsConds.html"));
   termsConds->setOpenExternalLinks(true);
   termsConds->setFrameShape(QFrame::NoFrame);
   termsConds->setFixedSize(478, 530);
   termsConds->setVerticalScrollBar(scrollBar);
   termsConds->setStyleSheet("background-color: transparent;");
   termsConds->setContextMenuPolicy(Qt::NoContextMenu);
   termsConds->setTextInteractionFlags(Qt::NoTextInteraction);

   // Main Layout
   layout = new QVBoxLayout(this);
   layout->setSpacing(0);
   layout->setContentsMargins(10, 10, 10, 10);
   layout->addWidget(heading, 0, Qt::AlignLeft);
   layout->addSpacing(2);
   layout->addWidget(termsConds, 0, Qt::AlignCenter);
   layout->addStretch();

   setDarkMode(isDarkMode);
}

void TermsConditions::setDarkMode(bool value) {
   if (isDarkMode == value) 
      return;

   isDarkMode = value;
   scrollBar->setDarkMode(isDarkMode);
}