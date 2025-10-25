#include "pwdValidator.h"

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

PwdValidator::PwdValidator(QWidget *parent) : QWidget(parent)
{
   setAttribute(Qt::WA_TranslucentBackground);
   setFixedSize(QSize(300, 176));
   load_weak_pwds();

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

void PwdValidator::vald_pwd_reqs(const QString &pw)
{
   QString pwd = pw;
   QByteArray pwdBytes = pwd.toUtf8();
   pwd.fill(QChar(0));

   bool hasLength = pwdBytes.size() >= 8;
   bool hasUpper = false;
   bool hasLower = false;
   bool hasDigit = false;
   bool hasSpecial = false;

   for (char c : pwdBytes)
   {
      if (c >= 'A' && c <= 'Z')
         hasUpper = true;
      else if (c >= 'a' && c <= 'z')
         hasLower = true;
      else if (c >= '0' && c <= '9')
         hasDigit = true;
      else
         hasSpecial = true;
   }

   bool notWeak = !chk_weak_pwd(pwdBytes);

   hasLength ? lst_8->setChecked() : lst_8->setUnchecked();
   hasUpper ? o_uc->setChecked() : o_uc->setUnchecked();
   hasLower ? o_lc->setChecked() : o_lc->setUnchecked();
   hasDigit ? o_n->setChecked() : o_n->setUnchecked();
   hasSpecial ? o_sc->setChecked() : o_sc->setUnchecked();

   bool is_stg_pwd = hasLength && hasUpper && hasLower && hasDigit && hasSpecial && notWeak;

   if (is_stg_pwd)
   {
      stg_pwd->setChecked();
      emit pwd_is_strong();
   }
   else
   {
      stg_pwd->setUnchecked();
      emit pwd_is_weak();
   }

   pwdBytes.fill(0);
}

void PwdValidator::load_weak_pwds()
{
   QFile file("C:/Users/umara/Desktop/Vaultorix/WEAK_PWDS_LEKD.txt");
   if (file.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      QTextStream in(&file);
      while (!in.atEnd())
      {
         QString text = in.readLine().trimmed().toLower();
         if (!text.isEmpty())
            weak_pwds.insert(text);
      }
   }
   else
   {
      qWarning() << "Error in checking password whether it's weak or strong";
   }
}

bool PwdValidator::chk_weak_pwd(QByteArray pwd)
{
   return weak_pwds.contains(QString::fromUtf8(pwd.toLower()));
}
