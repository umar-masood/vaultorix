#pragma once

#include "../../components/Button.h"
#include "../../components/TextField.h"
#include "../../components/Label.h"

#include "../../../core/services/auth/SigninService.h"

namespace Ui::Utils { class TextWithBtn; };
namespace Ui { class AuthWindow; };
namespace Ui {
    /* ---------------- Account Sign in ------------------  */
    class Signin : public QWidget {
       Q_OBJECT
    
       public:
       explicit Signin(QWidget *parent = nullptr, Ui::AuthWindow *authWindow = nullptr);
       void setDarkMode(bool isDarkMode);
       Ui::Utils::TextWithBtn *redirectToSignUpWidget() const { return _redirectToSignUpWidget; }
    
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
       TextField *_usernameField = nullptr;
    
       // Password Field
       TextField *_passwordField = nullptr;
    
       // Forgot Password Button
       Button *_forgotPwdBtn = nullptr;
    
       // Sign In Button
       Button *_signInBtn = nullptr;
    
       // Cancel Button
       Button *_cancelBtn = nullptr;
    
       // For Sign up if it doesn't have an active account
       Ui::Utils::TextWithBtn *_redirectToSignUpWidget = nullptr;

       // Signin Core
       Core::SigninService *signInCore = nullptr;
    
       // Auth Window
       Ui::AuthWindow *_authWindow = nullptr;

       // Helpers
       void handleSignInError(const QString &errorName, 
                              bool isSignInButtonEnabled, 
                              const QString &signInButtonText = "Sign in");
                              
       void updateSignInBtnState(bool isEnabled, const QString &text);

       private slots:
       void onSignInClicked();
       void onCancelClicked();
       void onErrorDialogActionBtnClicked(const QString &key);
       void onSignedIn(const QJsonObject &obj);
       void onFailedToSignIn(const Core::SigninService::SignInError &error);

       signals:
       void verificationRequired(const QString &email, const QString &authType);
    };
};