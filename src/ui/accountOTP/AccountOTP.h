#pragma once

#include <QVBoxLayout>
#include "../accountOTP/OTPWidget.h"
#include "../components/Button.h"

class Label : public QLabel {

   Q_OBJECT 

   public:
   explicit Label(bool isIconic = false,
                  const QString &family = "Segoe UI",
                  int pointSize = 8,
                  QFont::Weight weight = QFont::Normal,
                  bool italic = false,
                  const QString &text = " ",
                  Qt::Alignment alignment = Qt::AlignCenter,
                  QWidget *parent = nullptr);
   
   void show();
   void hide();
   void setTextAnimated(const QString &text);

   private:
   QPropertyAnimation *fadeIn = nullptr;
   QPropertyAnimation *fadeOut = nullptr;
   QGraphicsOpacityEffect *opacity = nullptr;

};

class TextWithBtn : public QWidget {
   Q_OBJECT

   public:
   explicit TextWithBtn(QWidget *parent = nullptr);
   
   Label* textLabel() const;
   Button* resendButton() const;
   Label* timerLabel() const;

   private:
   Label *text = nullptr;
   Button *button = nullptr;
   Label *timer = nullptr;

   signals:
   void onButtonClicked();
};

class AccountOTP : public QWidget {
   Q_OBJECT
   public:
   explicit AccountOTP(QWidget *parent = nullptr);
   
   void setEmail(const QString &email);
   Q_INVOKABLE void setDarkMode(bool value);

   OTPWidget *OTP() const;
   Button *verifyBtn() const;
   TextWithBtn *resendOtpWidget() const;
   Label *messageLabel() const;

   private:
   bool isDarkMode = false;

   Label *icon = nullptr;
   Label *heading = nullptr;
   Label *text = nullptr;
   Label *message = nullptr;

   OTPWidget *otpWidget = nullptr;
   TextWithBtn *resendOtp = nullptr;

   Button *verify = nullptr;
   Button *cancel = nullptr;
   QVBoxLayout *layout = nullptr;

   signals:
   void emailEntered(const QString &email);
   void resendClicked();
   void verifyClicked();
   void cancelClicked();
   void themeModeChanged(bool enable);

   private slots:
   void onThemeModeChanged(bool enable);
   void onEmailEntered(const QString &email);
};