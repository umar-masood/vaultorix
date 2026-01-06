#include "AccountOTP.h"

/* -------   OTP Widget ----------------  */
OTPWidget::OTPWidget(QWidget *parent) : QWidget(parent) {
   setAttribute(Qt::WA_TranslucentBackground);
   setFixedSize(QSize(360, 60));
   setFocusPolicy(Qt::StrongFocus);

   // Layout
   auto *layout = new QHBoxLayout(this);
   layout->addStretch();
   layout->setSpacing(0);

   // Font
   QFont font;
   font.setFamily("Segoe UI");
   font.setBold(true);
   font.setPointSize(22);

   // Boxes
   for (int i = 0; i < boxes; i++) {
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

   updateStyles();
   updateHighlight();
}

void OTPWidget::setDarkMode(bool value) {
   isDarkMode = value;
   updateStyles();
   updateHighlight();
}

void OTPWidget::keyPressEvent(QKeyEvent *event) {
   const int n = OTPBoxes.size();
   if ((event->key() >= Qt::Key_0 && event->key() <= Qt::Key_9) ||
      (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z))
   {
      QString text = event->text().toUpper();
      if (!text.isEmpty() && currentIndex >= 0 && currentIndex < n) {
         OTPBoxes[currentIndex]->setText(text);
         currentIndex++;
         if (currentIndex >= n)
            currentIndex = n - 1;
         updateHighlight();
      }

      if (currentIndex == n - 1 && !OTPBoxes[n - 1]->text().isEmpty()) {
         QString c;
         for (auto *i : OTPBoxes)
            c.append(i->text());
         emit OTPcompleted(c);
      }

   } else if (event->key() == Qt::Key_Backspace) {
      if (currentIndex >= 0 && currentIndex < n) {
         if (!OTPBoxes[currentIndex]->text().isEmpty()) {
            OTPBoxes[currentIndex]->clear();
         } else {
            if (currentIndex > 0)
               currentIndex--;
            OTPBoxes[currentIndex]->clear();
            updateHighlight();
         }
      }
   }
}

void OTPWidget::updateStyles() {
   bg_color_normal = isDarkMode ? "#2D2D2D" : "#FBFBFB";
   bg_color_focused = isDarkMode ? "#1F1F1F" : "#FFFFFF";
   text_color = isDarkMode ? "white" : "black";
   border_color = isDarkMode ? "#4D4D4D" : "#CCCCCC";

   normalStyle = QString(
                     "background-color: %1; border-radius: 7px; border: 1px solid %2; color: %3;")
                     .arg(bg_color_normal)
                     .arg(border_color)
                     .arg(text_color);

   focusedStyle = QString(
                      "background-color: %1; border: 2px solid #109AC7; border-radius: 7px; color: %2;")
                      .arg(bg_color_focused)
                      .arg(text_color);

   disabledStyle = QString(
                     "background-color: %1; border-radius: 7px; border: 1px solid %2; color: gray;")
                     .arg(bg_color_normal)
                     .arg(border_color);
}

void OTPWidget::updateHighlight() {
   for (int i = 0; i < OTPBoxes.size(); i++) {
      if (!isEnabled()) 
         OTPBoxes[i]->setStyleSheet(disabledStyle);
      else if (i == currentIndex)
         OTPBoxes[i]->setStyleSheet(focusedStyle);
      else
         OTPBoxes[i]->setStyleSheet(normalStyle);
   }
}

void OTPWidget::setEnabled(bool enabled) {
   QWidget::setEnabled(enabled);
   updateStyles();
   updateHighlight();
}

void OTPWidget::showEvent(QShowEvent *event) {
   QWidget::showEvent(event);
   this->setFocus(Qt::OtherFocusReason);
}

/* Resend OTP Widget */
TextWithBtn::TextWithBtn(QWidget *parent, const QString &text, const QString &hyperlinkText, const QSize &hyperlinkSize, bool useTimer) : QWidget(parent) {
   setAttribute(Qt::WA_TranslucentBackground);

   // Text 
   _text = new Label(false, "Segoe UI", 10, QFont::Normal, false, text, Qt::AlignHCenter);
   _text->setStyleSheet("color: black;");
   _text->setParent(this);
   _text->setFixedSize(136, 22);
   _text->move(0,0);

   // Resend Button
   _button = new Button(this);
   _button->setDisplayMode(Button::TextOnly);
   _button->setFixedSize(hyperlinkSize);
   _button->setHyperLink(true);
   _button->setText(hyperlinkText);
   _button->setFontProperties("Segoe UI", 10, false, false);
   _button->setHyperLinkColors("#008EDE", "#15F2FF");
   _button->move(_text->width() + 4, 2);

   connect(_button, &Button::clicked, this, [this]() { emit onButtonClicked(); });

   setFixedSize(QSize(_text->width() + _button->width() + 2, 18));

   // Timer Label
   if (useTimer) {
      _timer = new Label(false, "Segoe UI", 10, QFont::Normal, false, "00:00", Qt::AlignHCenter);
      _timer->setStyleSheet("color: black;");
      _timer->setParent(this);
      _timer->setFixedSize(36, 22);
      _timer->move(_text->width() + _button->width() + 4,0);   

      setFixedSize(QSize(_text->width() + _button->width() + _timer->width() + 2, 18));
   }
}

// Getters of Resend OTP Widget
Label* TextWithBtn::text() const { return _text; }
Button * TextWithBtn::button() const { return _button; }
Label *TextWithBtn::timer() const { return _timer; }

/* AccountOTP Main Widget Implementation */
AccountOTP::AccountOTP(QWidget *parent) : QWidget(parent) {
   setAttribute(Qt::WA_TranslucentBackground);

   // Main Icon of OTP at the top
   illustration = new Label(true);
   illustration->setFixedSize(QSize(106, 106));
   illustration->setPixmap(QPixmap(":/icons/AccountOTP/otp.png").scaled(106, 106, Qt::KeepAspectRatio, Qt::SmoothTransformation));
   illustration->setScaledContents(true);

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
   _otpWidget = new OTPWidget;

   // Message Label (For displaying error messages)
   _message = new AnimatedLabel(false, "Segoe UI", 10, QFont::Medium, false);
   _message->setWordWrap(true);
   _message->setStyleSheet("color: #FF0000;");
   _message->setFixedWidth(324);

   // Resend OTP Widget (Resend Text + Button + Timer)
   _resendOtpWidget = new TextWithBtn(nullptr, "Didn`t receive the OTP?", "Resend", QSize(50,12), true);
   connect(_resendOtpWidget, &TextWithBtn::onButtonClicked, this, [this]() { emit resendClicked(); });

   // Verify Button
   _verifyBtn = new Button("Verify");
   _verifyBtn->setDisplayMode(Button::TextOnly);
   _verifyBtn->setFixedSize(QSize(360, 36));
   _verifyBtn->setGradientColor(true, "#008EDE", "#15F2FF");
   _verifyBtn->setHoverGradientColor("#008EDE");
   _verifyBtn->setFontProperties("Segoe UI", 11, true, false);
   connect(_verifyBtn, &Button::clicked, this, [this]() { emit verifyClicked(); });

   // Cancel Button
   _cancelBtn = new Button("Cancel");
   _cancelBtn->setDisplayMode(Button::TextOnly);
   _cancelBtn->setSecondary(true);
   _cancelBtn->setFixedSize(QSize(360, 36));
   _cancelBtn->setFontProperties("Segoe UI", 11, true, false);
   connect(_cancelBtn, &Button::clicked, this, [this]() { emit cancelClicked(); });

   // Main Layout
   layout = new QVBoxLayout;
   layout->setSpacing(0);
   layout->addWidget(illustration, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(heading, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(text, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(_otpWidget, 0, Qt::AlignHCenter);
   layout->addSpacing(10);
   layout->addWidget(_message, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(_resendOtpWidget, 0, Qt::AlignHCenter);
   layout->addSpacing(26);
   layout->addWidget(_verifyBtn, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(_cancelBtn, 0, Qt::AlignHCenter);
   layout->addStretch();
   setLayout(layout);

   // Theme Mode 
   connect(this, &AccountOTP::themeModeChanged, this, &AccountOTP::onThemeModeChanged);
}

// Getters of AccountOTP Main Widget
OTPWidget* AccountOTP::otpWidget() const { return _otpWidget; } // Return OTP Widget (for retrieving entered OTP) in backend
Button* AccountOTP::verifyButton() const { return _verifyBtn; } // Return Verify Button for updating its properties in backend
TextWithBtn* AccountOTP::resendOtpWidget() const { return _resendOtpWidget; } // Return Resend OTP Widget for updating its properties in backend
AnimatedLabel* AccountOTP::message() const { return _message; } // Return Message Label for displaying error messages in backend

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
   if (_cancelBtn) _cancelBtn->setDarkMode(enable);
   if (_resendOtpWidget) {
      _resendOtpWidget->text()->setStyleSheet(QString("color: %1").arg(enable ? "white" : "black")); 
      _resendOtpWidget->timer()->setStyleSheet(QString("color: %1").arg(enable ? "white" : "black")); 
   } 

   if (_otpWidget) _otpWidget->setDarkMode(enable);
}