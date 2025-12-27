#include "AccountOTP.h"

/* Resend OTP Widget */
TextWithBtn::TextWithBtn(QWidget *parent) : QWidget(parent) {
   setAttribute(Qt::WA_TranslucentBackground);

   // Text 
   text = new Label(false, "Segoe UI", 10, QFont::Normal, false, "Didn`t receive the OTP?", Qt::AlignHCenter);
   text->setStyleSheet("color: black;");
   text->setParent(this);
   text->setFixedSize(136, 22);
   text->move(0,0);
   
   // Resend Button
   button = new Button(this);
   button->setDisplayMode(Button::TextOnly);
   button->setFixedSize(QSize(50, 12));
   button->setHyperLink(true);
   button->setText("Resend");
   button->setFontProperties("Segoe UI", 10, false, false);
   button->setHyperLinkColors("#008EDE", "#15F2FF");
   button->move(text->width() + 4, 2);
   connect(button, &Button::clicked, this, [this](){
      emit onButtonClicked();
   });

   // Timer Label
   timer = new Label(false, "Segoe UI", 10, QFont::Normal, false, "00:00", Qt::AlignHCenter);
   timer->setStyleSheet("color: black;");
   timer->setParent(this);
   timer->setFixedSize(36, 22);
   timer->move(text->width() + button->width() + 4,0);

   setFixedSize(QSize(text->width() + button->width() + timer->width() + 2, 18));
}

// Getters of Resend OTP Widget
Label* TextWithBtn::textLabel() const { return text; }
Button * TextWithBtn::resendButton() const { return button; }
Label *TextWithBtn::timerLabel() const { return timer; }

/* AccountOTP Main Widget Implementation */
AccountOTP::AccountOTP(QWidget *parent) : QWidget(parent) {
   setAttribute(Qt::WA_TranslucentBackground);

   // Main Icon of OTP at the top
   icon = new Label(true);
   icon->setFixedSize(QSize(106, 106));
   icon->setPixmap(QPixmap(":/icons/AccountOTP/otp.png").scaled(106, 106, Qt::KeepAspectRatio, Qt::SmoothTransformation));
   icon->setScaledContents(true);

   // Main Heading
   heading = new Label(false, "Inter", 22, QFont::Bold, false, "Verify with OTP");
   heading->setStyleSheet("color: black;");
   
   // Text Under Main Heading
   text = new Label(false, "Segoe UI", 10, QFont::Medium, false);
   text->setWordWrap(true);
   text->setStyleSheet("color: #8D8D8D;");
   text->setFixedWidth(324);
   connect(this, &AccountOTP::emailEntered, this, &AccountOTP::onEmailEntered);

   // OTP Widget (Contains 6 input boxes)
   otpWidget = new OTPWidget;

   // Message Label (For displaying error messages)
   message = new Label(false, "Segoe UI", 10, QFont::Medium, false);
   message->setWordWrap(true);
   message->setStyleSheet("color: #FF0000;");
   message->setFixedWidth(324);

   // Resend OTP Widget (Resend Text + Button + Timer)
   resendOtp = new TextWithBtn;
   connect(resendOtp, &TextWithBtn::onButtonClicked, this, [this](){
      emit resendClicked();
   });

   // Verify Button
   verify = new Button("Verify");
   verify->setDisplayMode(Button::TextOnly);
   verify->setFixedSize(QSize(360, 36));
   verify->setGradientColor(true, "#008EDE", "#15F2FF");
   verify->setHoverGradientColor("#008EDE");
   verify->setFontProperties("Segoe UI", 11, true, false);
   connect(verify, &Button::clicked, this, [this](){
      emit verifyClicked();
   });

   // Cancel Button
   cancel = new Button("Cancel");
   cancel->setDisplayMode(Button::TextOnly);
   cancel->setSecondary(true);
   cancel->setFixedSize(QSize(360, 36));
   cancel->setFontProperties("Segoe UI", 11, true, false);
   connect(cancel, &Button::clicked, this, [this](){
      emit cancelClicked();
   });

   // Main Layout
   layout = new QVBoxLayout;
   layout->setSpacing(0);
   layout->addWidget(icon, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(heading, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(text, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(otpWidget, 0, Qt::AlignHCenter);
   layout->addSpacing(10);
   layout->addWidget(message, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(resendOtp, 0, Qt::AlignHCenter);
   layout->addSpacing(26);
   layout->addWidget(verify, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(cancel, 0, Qt::AlignHCenter);
   layout->addStretch();
   setLayout(layout);

   // Theme Mode 
   connect(this, &AccountOTP::themeModeChanged, this, &AccountOTP::onThemeModeChanged);
}

// Getters of AccountOTP Main Widget
OTPWidget* AccountOTP::OTP() const { return otpWidget; } // Return OTP Widget (for retrieving entered OTP) in backend
Button* AccountOTP::verifyBtn() const { return verify; } // Return Verify Button for updating its properties in backend
TextWithBtn* AccountOTP::resendOtpWidget() const { return resendOtp;} // Return Resend OTP Widget for updating its properties in backend
Label* AccountOTP::messageLabel() const { return message; } // Return Message Label for displaying error messages in backend

void AccountOTP::onEmailEntered(const QString &email) {
   QString e = email;
   int idx = e.indexOf('@');
   if (idx > 3) 
      for (int i = 3; i < idx; i++)  
         e[i] = '*';
   text->setText(QString("To ensure your security, please enter the One-Time Password (OTP) sent to your registered email-address (%1) below.").arg(e));
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
      resendOtp->textLabel()->setStyleSheet(QString("color: %1").arg(enable ? "white" : "black")); 
      resendOtp->timerLabel()->setStyleSheet(QString("color: %1").arg(enable ? "white" : "black")); 
   } 
   if (otpWidget) otpWidget->setDarkMode(enable);
}