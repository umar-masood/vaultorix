#include "AccountOTP.h"
#include <QTimer>

Label::Label(bool isIconic, const QString &family, int pointSize, QFont::Weight weight, bool italic, const QString &text, Qt::Alignment alignment, QWidget *parent) : QLabel(parent) {
   setAttribute(Qt::WA_TranslucentBackground);
   setAlignment(alignment);

   opacity = new QGraphicsOpacityEffect(this);
   opacity->setOpacity(1.0);
   setGraphicsEffect(opacity);

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

void Label::show() {
   if (!isVisible()) {
      opacity->setOpacity(0.0);
      QLabel::show();
   }

   fadeIn = new QPropertyAnimation(opacity, "opacity", this);
   fadeIn->setStartValue(0.0);
   fadeIn->setEndValue(1.0);
   fadeIn->setDuration(500);
   fadeIn->setEasingCurve(QEasingCurve::InOutQuad);
   fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
}

void Label::hide() {
   fadeOut = new QPropertyAnimation(opacity, "opacity", this);
   fadeOut->setStartValue(1.0);
   fadeOut->setEndValue(0.0);
   fadeOut->setDuration(500);
   fadeOut->setEasingCurve(QEasingCurve::InOutQuad);

   connect(fadeOut, &QPropertyAnimation::finished, this, [this](){
      QLabel::hide();
      opacity->setOpacity(1.0);
   });

   fadeOut->start(QAbstractAnimation::DeleteWhenStopped);
}

void Label::setTextAnimated(const QString &text)
{
   hide();
   QTimer::singleShot(500, [this, text]() {
      QLabel::setText(text);
      show();
   });
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

   timer = new Label(false, "Segoe UI", 10, QFont::Normal, false, "00:00", Qt::AlignHCenter);
   timer->setStyleSheet("color: black;");
   timer->setParent(this);
   timer->setFixedSize(36, 22);
   timer->move(text->width() + button->width() + 4,0);

   setFixedSize(QSize(text->width() + button->width() + timer->width() + 2, 18));
}

Label* TextWithBtn::label() const { return text; }

Button * TextWithBtn::btn() const { return button; }

Label *TextWithBtn::timerLabel() const { return timer; }

AccountOTP::AccountOTP(QWidget *parent) : QWidget(parent) {
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

   message = new Label(false, "Segoe UI", 10, QFont::Medium, false);
   message->setWordWrap(true);
   message->setStyleSheet("color: red;");
   message->setFixedWidth(324);

   resendOtp = new TextWithBtn;
   connect(resendOtp, &TextWithBtn::onButtonClicked, this, [this](){
      emit resendClicked();
   });

   verify = new Button("Verify");
   verify->setDisplayMode(Button::TextOnly);
   verify->setSize(QSize(360, 36));
   verify->setGradientColor(true, "#008EDE", "#15F2FF");
   verify->setHoverGradientColor("#008EDE");
   verify->setFontProperties("Segoe UI", 11, true, false);
   connect(verify, &Button::clicked, this, [this](){
      emit verifyClicked();
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
   layout->addSpacing(8);
   layout->addWidget(message, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(resendOtp, 0, Qt::AlignHCenter);
   layout->addSpacing(26);
   layout->addWidget(verify, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(cancel, 0, Qt::AlignHCenter);
   layout->addStretch();

   // Theme Mode 
   connect(this, &AccountOTP::themeModeChanged, this, &AccountOTP::onThemeModeChanged);
}

void AccountOTP::onEmailEntered(const QString &email) {
   QString e = email;
   int idx = e.indexOf('@');
   if (idx > 3) for (int i = 3; i < idx; i++)  e[i] = '*';
   text->setText(QString("To ensure your security, please enter the One-Time Password (OTP) sent to your registered email-address (%1) below.").arg(e));
}

OTPWidget* AccountOTP::OTP() const {
   return otpWidget;
}

Button *AccountOTP::verifyBtn() const {
   return verify;
}

TextWithBtn *AccountOTP::resendOtpWidget() const {
   return resendOtp;
}

Label *AccountOTP::messageLabel() const {
   return message;
}

void AccountOTP::setEmail(const QString &email) {
   if (!email.isEmpty()) 
      emit emailEntered(email);
}

void AccountOTP::setDarkMode(bool value) {
   if (isDarkMode == value) return;
   isDarkMode = value;
   emit themeModeChanged(isDarkMode);
}

void AccountOTP::onThemeModeChanged(bool enable) {
   if (heading) heading->setStyleSheet(QString("color: %1;").arg(enable ? "white" : "black"));
   if (cancel) cancel->setDarkMode(enable);
   if (resendOtp) {
      resendOtp->label()->setStyleSheet(QString("color: %1").arg(enable ? "white" : "black")); 
      resendOtp->timerLabel()->setStyleSheet(QString("color: %1").arg(enable ? "white" : "black")); 
   } 
   if (otpWidget) otpWidget->setDarkMode(enable);
}