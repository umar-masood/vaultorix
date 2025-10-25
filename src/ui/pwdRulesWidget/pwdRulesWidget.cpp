#include "PwdRulesWidget.h"

RuleItem::RuleItem(const QString &ruleTxt, QWidget *parent) : QWidget(parent)
{
   setFixedHeight(22);
   text = ruleTxt;

   setAttribute(Qt::WA_TranslucentBackground);
   iconChecked = QPixmap(":/icons/AccountCreate/checked.svg").scaled(iconSize, iconSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

   iconUnchecked = QPixmap(":/icons/AccountCreate/unchecked.svg").scaled(iconSize, iconSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

   font.setFamily("Segoe UI");
   font.setPointSize(10);
   font.setWeight(QFont::Normal);

   setUnchecked();
}

void RuleItem::paintEvent(QPaintEvent *event){
   QPainter painter(this);
   painter.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing);

   // Icon
   const QPixmap &icon = checkedState ? iconChecked : iconUnchecked;
   int iconY = (height() - iconSize) / 2;
   painter.drawPixmap(0, iconY, iconSize, iconSize, icon);

   // Text
   painter.setFont(font);
   painter.setPen(QColor(checkedState ? coloredText : uncoloredText));

   int textX = iconSize + 8;
   painter.drawText(QRect(textX, 2, width() - textX, height()), 0, text);
}

void RuleItem::setChecked()
{
   checkedState = true;
   update();
}

void RuleItem::setUnchecked()
{
   checkedState = false;
   update();
}

PwdRulesWidget::PwdRulesWidget(QWidget *parent) : QWidget(parent)
{
   setAttribute(Qt::WA_TranslucentBackground);
   setFixedSize(QSize(300, 176));

   auto *mainLayout = new QVBoxLayout(this);
   mainLayout->setSpacing(0);
   mainLayout->setContentsMargins(0, 0, 0, 0);

   lst_8 = new RuleItem("At least 8 characters");
   o_lc = new RuleItem("One lowercase letter");
   o_uc = new RuleItem("One uppercase letter");
   o_n = new RuleItem("One number");
   o_sc = new RuleItem("One special character");
   stg_pwd = new RuleItem("Strong Password");

   mainLayout->addWidget(stg_pwd);
   mainLayout->addSpacing(8);
   mainLayout->addWidget(lst_8);
   mainLayout->addSpacing(8);
   mainLayout->addWidget(o_lc);
   mainLayout->addSpacing(8);
   mainLayout->addWidget(o_uc);
   mainLayout->addSpacing(8);
   mainLayout->addWidget(o_n);
   mainLayout->addSpacing(8);
   mainLayout->addWidget(o_sc);
   mainLayout->addStretch();
}

RuleItem* PwdRulesWidget::atLeastEight() const {
   return lst_8;
}
RuleItem* PwdRulesWidget::oneLowerCase() const {
   return o_lc;
}
RuleItem* PwdRulesWidget::oneUpperCase() const {
   return o_uc;
}
RuleItem* PwdRulesWidget::oneDigit() const {
   return o_n;
}
RuleItem* PwdRulesWidget::oneSpecialChar() const {
   return o_sc;
}
RuleItem* PwdRulesWidget::strongPwd() const {
   return stg_pwd;
}
