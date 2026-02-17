#include "PwdRulesWidget.h"
#include "../../../resources/IconManager.h"

/* ------------  Custom Rule Item Widget ------------  */
RuleItem::RuleItem(const QString &ruleTxt, QWidget *parent) : QWidget(parent) {
   setFixedHeight(22);
   setAttribute(Qt::WA_TranslucentBackground);

   text = ruleTxt;

   validIcon = QPixmap(IconManager::icon(Icons::Valid)).scaled(iconSize, iconSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
   invalidIcon = QPixmap(IconManager::icon(Icons::Invalid)).scaled(iconSize, iconSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

   font.setFamily("Segoe UI");
   font.setPointSize(10);
   font.setWeight(QFont::Normal);

   setInvalid();
}

void RuleItem::paintEvent(QPaintEvent *event){
   QPainter painter(this);
   painter.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing);

   // Icon
   const QPixmap &icon = validityState ? validIcon : invalidIcon;
   int iconY = (height() - iconSize) / 2;
   painter.drawPixmap(0, iconY, iconSize, iconSize, icon);

   // Text
   painter.setFont(font);
   painter.setPen(QColor(validityState ? coloredText : uncoloredText));

   int textX = iconSize + 8;
   painter.drawText(QRect(textX, 2, width() - textX, height()), 0, text);
}

void RuleItem::setValid() {
   validityState = true;
   update();
}

void RuleItem::setInvalid() {
   validityState = false;
   update();
}

/* --------------- Password Rules Widget ------------- */
PwdRulesWidget::PwdRulesWidget(QWidget *parent) : QWidget(parent) {
   setAttribute(Qt::WA_TranslucentBackground);
   setFixedSize(QSize(300, 176));

   // Main Layout
   mainLayout = new QVBoxLayout(this);
   mainLayout->setSpacing(0);
   mainLayout->setContentsMargins(0, 0, 0, 0);

   // Rules Items
   _atLeastEightChars      =   new RuleItem("At least 8 characters");
   _atLeastOneLowerChar    =   new RuleItem("At least one lowercase letter");
   _atLeastOneUpperChar    =   new RuleItem("At least one uppercase letter");
   _atLeastOneDigit        =   new RuleItem("At least one digit");
   _atLeastOneSpecialChar  =   new RuleItem("At least one special character");
   _strongPassword         =   new RuleItem("Strong Password");

   // Adding into Layout
   mainLayout->addWidget(_strongPassword);
   mainLayout->addSpacing(8);
   mainLayout->addWidget(_atLeastEightChars);
   mainLayout->addSpacing(8);
   mainLayout->addWidget(_atLeastOneLowerChar);
   mainLayout->addSpacing(8);
   mainLayout->addWidget(_atLeastOneUpperChar);
   mainLayout->addSpacing(8);
   mainLayout->addWidget(_atLeastOneDigit);
   mainLayout->addSpacing(8);
   mainLayout->addWidget(_atLeastOneSpecialChar);
   mainLayout->addStretch();
}

RuleItem* PwdRulesWidget::atLeastEight() const { return _atLeastEightChars; }
RuleItem* PwdRulesWidget::atLeastOneLowerCaseChar() const { return _atLeastOneLowerChar; }
RuleItem* PwdRulesWidget::atLeastOneUpperCaseChar() const { return _atLeastOneUpperChar; }
RuleItem* PwdRulesWidget::atLeastOneDigit() const { return _atLeastOneDigit; }
RuleItem* PwdRulesWidget::atLeastOneSpecialChar() const { return _atLeastOneSpecialChar; }
RuleItem* PwdRulesWidget::strongPassword() const { return _strongPassword; }
