#pragma once

#include "../accountOTP/OTPWidget.h"
#include "../components/Button.h"
#include "../components/TextField.h"
#include "../components/Label.h"

class AccountSignIn : public QWidget {
   Q_OBJECT
   public:
   explicit AccountSignIn(QWidget *parent = nullptr);

   Q_INVOKABLE void setDarkMode(bool value);
   TextField* usernameField() const;
   TextField* pwdField() const;
   Button* signInButton() const;

   private:
   bool isDarkMode = false;

   Label *icon = nullptr;
   Label *heading = nullptr;
   Label *text = nullptr;

   TextField *username = nullptr;
   TextField *pwd = nullptr;

   Button *forgotPwd = nullptr;

   Button *signIn = nullptr;
   Button *cancel = nullptr;
   QVBoxLayout *layout = nullptr;

   signals:
   void forgotPwdClicked();
   void signInClicked();
   void cancelClicked();
};
