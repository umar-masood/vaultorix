#pragma once

#include "../accountOTP/AccountOTP.h"
#include "../components/Button.h"
#include "../components/TextField.h"
#include "../components/Label.h"

class AccountSignIn : public QWidget {
   Q_OBJECT

   public:
   explicit AccountSignIn(QWidget *parent = nullptr);
   Q_INVOKABLE void setDarkMode(bool value);
   TextField* usernameField() const;
   TextField* passwordField() const;
   Button* signInButton() const;

   private:
   // Theme Mode Flag
   bool isDarkMode = false;
   
   // Icon Paths
   const QString userIcon = ":/icons/AccountSignIn/user.svg";
   const QString passwordIcon = ":/icons/AccountSignIn/password.svg";

   // Illustration
   Label *illustration = nullptr;

   // Heading
   Label *heading = nullptr;

   // Text under heading (Welcome Text)
   Label *text = nullptr;

   // Username Field
   TextField *username = nullptr;

   // Password Field
   TextField *password = nullptr;

   // Forgot Password Button
   Button *forgotPwd = nullptr;

   // Sign In Button
   Button *signInBtn = nullptr;

   // Cancel Button
   Button *cancelBtn = nullptr;

   // Main Layout
   QVBoxLayout *layout = nullptr;

   signals:
   void forgotPwdClicked();
   void signInClicked();
   void cancelClicked();
};
