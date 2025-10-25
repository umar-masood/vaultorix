#include "AccountOTP.h"

Label::Label(bool isIconic, const QString &family, int pointSize, QFont::Weight weight, bool italic, const QString &text, Qt::Alignment alignment, QWidget *parent) : QLabel(parent) {
   setAttribute(Qt::WA_TranslucentBackground);
   setAlignment(alignment);

   if (!isIconic) {
      setText(text);

      QFont fnt;
      fnt.setPointSize(pointSize);
      fnt.setFamily(family);
      fnt.setWeight(weight);
      fnt.setItalic(italic);
      setFont(fnt);
   }
}

TextWithBtn::TextWithBtn(QWidget *parent) : QWidget(parent)
{
   setAttribute(Qt::WA_TranslucentBackground);

   text = new Label(false, "Segoe UI", 10, QFont::Normal, false, "Didn`t receive the OTP?", Qt::AlignHCenter);
   text->setStyleSheet("color: black;");
   text->setParent(this);
   text->setFixedSize(136, 22);
   text->move(0,0);
   
   button = new Button(this);
   button->setDisplayMode(Button::TextOnly);
   button->setSize(QSize(50, 12));
   button->setHyperLink(true);
   button->setText("Resend");
   button->setFontProperties("Segoe UI", 10, false, false);
   button->setHyperLinkColors("#008EDE", "#15F2FF");
   button->move(text->width() + 4, 2);
   connect(button, &Button::clicked, this, [this](){
      emit onButtonClicked();
   });

   setFixedSize(QSize(text->width() + button->width() + 2, 18));
}

Label* TextWithBtn::label() const { return text; }

AccountOTP::AccountOTP(QWidget *parent) : QWidget(parent)
{
   setAttribute(Qt::WA_TranslucentBackground);

   icon = new Label(true);
   icon->setFixedSize(QSize(106, 106));
   icon->setPixmap(QPixmap(":/icons/AccountOTP/otp.png").scaled(106, 106, Qt::KeepAspectRatio, Qt::SmoothTransformation));
   icon->setScaledContents(true);

   heading = new Label(false, "Inter", 22, QFont::Bold, false, "Verify with OTP");
   heading->setStyleSheet("color: black;");
   
   text = new Label(false, "Segoe UI", 10, QFont::Medium, false);
   text->setWordWrap(true);
   text->setStyleSheet("color: #8D8D8D;");
   text->setFixedWidth(324);
   connect(this, &AccountOTP::emailEntered, this, &AccountOTP::onEmailEntered);

   otpWidget = new OTPWidget;

   resendOtp = new TextWithBtn;
   connect(resendOtp, &TextWithBtn::onButtonClicked, this, [this](){
      emit resendClicked();
   });

   submit = new Button("Submit");
   submit->setDisplayMode(Button::TextOnly);
   submit->setSize(QSize(360, 36));
   submit->setGradientColor(true, "#008EDE", "#15F2FF");
   submit->setHoverGradientColor("#008EDE");
   submit->setFontProperties("Segoe UI", 11, true, false);
   connect(submit, &Button::clicked, this, [this](){
      emit submitClicked();
   });

   cancel = new Button("Cancel");
   cancel->setDisplayMode(Button::TextOnly);
   cancel->setSecondary(true);
   cancel->setSize(QSize(360, 36));
   cancel->setFontProperties("Segoe UI", 11, true, false);
   connect(cancel, &Button::clicked, this, [this](){
      emit cancelClicked();
   });

   layout = new QVBoxLayout(this);
   layout->setSpacing(0);
   layout->addWidget(icon, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(heading, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(text, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(otpWidget, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(resendOtp, 0, Qt::AlignHCenter);
   layout->addSpacing(26);
   layout->addWidget(submit, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(cancel, 0, Qt::AlignHCenter);
   layout->addStretch();

   // Theme Mode 
   connect(this, &AccountOTP::themeModeChanged, this, &AccountOTP::onThemeModeChanged);
}

void AccountOTP::onEmailEntered(const QString &email) {
   QString e = email;
   int idx = e.indexOf('@');
   if (idx > 3)
      for (int i = 3; i < idx; i++)  e[i] = '*';
      
   text->setText(QString("To ensure your security, please enter the One-Time Password (OTP) sent to your registered email-address (%1) below.").arg(e));
}

OTPWidget* AccountOTP::OTP() const {
   return otpWidget;
}

void AccountOTP::setEmail(const QString &email)
{
   if (!email.isEmpty()) 
      emit emailEntered(email);
}

void AccountOTP::setDarkMode(bool value)
{
   if (isDarkMode == value) return;
   isDarkMode = value;
   emit themeModeChanged(isDarkMode);
}

void AccountOTP::onThemeModeChanged(bool enable) {
   if (heading) heading->setStyleSheet(QString("color: %1;").arg(enable ? "white" : "black"));
   if (cancel) cancel->setDarkMode(enable);
   if (resendOtp) resendOtp->label()->setStyleSheet(QString("color: %1").arg(enable ? "white" : "black")); 
   if (otpWidget) otpWidget->setDarkMode(enable);
}