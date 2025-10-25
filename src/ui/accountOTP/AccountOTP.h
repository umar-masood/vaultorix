#pragma once

#include <QVBoxLayout>
#include "../accountOTP/OTPWidget.h"
#include "../components/Button.h"

class Label : public QLabel {
   public:
   explicit Label(bool isIconic = false,
                  const QString &family = "Segoe UI",
                  int pointSize = 8,
                  QFont::Weight weight = QFont::Normal,
                  bool italic = false,
                  const QString &text = " ",
                  Qt::Alignment alignment = Qt::AlignCenter,
                  QWidget *parent = nullptr);
};

class TextWithBtn : public QWidget {
   Q_OBJECT
   public:
   explicit TextWithBtn(QWidget *parent = nullptr);
   Label* label() const;

   private:
   Label *text = nullptr;
   Button *button = nullptr;

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

   private:
   bool isDarkMode = false;

   Label *icon = nullptr;
   Label *heading = nullptr;
   Label *text = nullptr;

   OTPWidget *otpWidget = nullptr;
   TextWithBtn *resendOtp = nullptr;

   Button *submit = nullptr;
   Button *cancel = nullptr;
   QVBoxLayout *layout = nullptr;

   signals:
   void emailEntered(const QString &email);
   void resendClicked();
   void submitClicked();
   void cancelClicked();
   void themeModeChanged(bool enable);

   private slots:
   void onThemeModeChanged(bool enable);
   void onEmailEntered(const QString &email);
};
