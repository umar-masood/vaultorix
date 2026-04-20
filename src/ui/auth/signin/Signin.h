#pragma once

#include "../../components/Button.h"
#include "../../components/TextField.h"
#include "../../components/Label.h"

#include "../otp/Otp.h"

namespace Ui::Auth {
    /* ---------------- Account Sign in ------------------  */
    class Signin : public QWidget {
       Q_OBJECT
    
       public:
       explicit Signin(QWidget *parent = nullptr);
       void setDarkMode(bool isDarkMode);
    
       TextField* usernameField() const;
       TextField* passwordField() const;
       TextWithBtn* redirectToSignup() const;
       Button* signInButton() const;
    
       private:
       // Icons
       QString UserIcon, KeyIcon;
    
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
    
       // For Sign up if it doesn't have an active account
       TextWithBtn *_redirectToSignUpWidget = nullptr;
    
       // Main Layout
       QVBoxLayout *layout = nullptr;
    
       signals:
       void forgotPwdClicked();
       void signInClicked();
       void cancelClicked();
    };
};