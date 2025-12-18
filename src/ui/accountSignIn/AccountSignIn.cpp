#include "AccountSignIn.h"

AccountSignIn::AccountSignIn(QWidget *parent) : QWidget(parent)
{
   setAttribute(Qt::WA_TranslucentBackground);

   icon = new Label(true);
   icon->setFixedSize(QSize(150, 150));
   icon->setPixmap(QPixmap(":/icons/AccountSignIn/sign-in.png").scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
   icon->setScaledContents(true);

   heading = new Label(false, "Inter", 22, QFont::Bold, false, "Sign in to your Account");
   
   text = new Label(false, "Segoe UI", 10, QFont::Medium, false, "Hi, Welcome back!");
   text->setWordWrap(true);
   text->setStyleSheet("color: #8D8D8D;");
   text->setFixedWidth(324);

   username = new TextField;
   username->setPlaceholderText("Username");
   username->setContextMenu(false);
   username->setSize(QSize(360, 36));
   username->setTextFieldIcon(true);
   username->setClearButton(true);
   username->setTextFieldIconSize(QSize(18,18));
   
   pwd = new TextField;
   pwd->setContextMenu(false);
   pwd->setPlaceholderText("Password");
   pwd->setSize(QSize(360, 36));
   pwd->setPasswordTextField(true);
   pwd->setTextFieldIcon(true);
   pwd->setTextFieldIconSize(QSize(18,18));

   forgotPwd = new Button("Forgot Password?");
   forgotPwd->setDisplayMode(Button::TextOnly);
   forgotPwd->setSize(QSize(110, 18));
   forgotPwd->setHyperLink(true);
   forgotPwd->setFontProperties("Segoe UI", 10, false, false);
   forgotPwd->setHyperLinkColors("#008EDE", "#15F2FF");
   connect(forgotPwd, &Button::clicked, this, [this](){
      emit onForgotPwdClicked();
   });

   signIn = new Button("Sign in");
   signIn->setDisplayMode(Button::TextOnly);
   signIn->setSize(QSize(360, 36));
   signIn->setGradientColor(true, "#008EDE", "#15F2FF");
   signIn->setHoverGradientColor("#008EDE");
   signIn->setFontProperties("Segoe UI", 11, true, false);
   connect(signIn, &Button::clicked, this, [this](){
      emit onSignInClicked();
   });

   cancel = new Button("Cancel");
   cancel->setDisplayMode(Button::TextOnly);
   cancel->setSecondary(true);
   cancel->setSize(QSize(360, 36));
   cancel->setFontProperties("Segoe UI", 11, true, false);
   connect(cancel, &Button::clicked, this, [this](){
      emit onCancelClicked();
   });

   layout = new QVBoxLayout(this);
   layout->setSpacing(0);
   layout->addStretch();
   layout->addWidget(icon, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(heading, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(text, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(username, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(pwd, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(forgotPwd, 0, Qt::AlignRight);
   layout->addSpacing(26);
   layout->addWidget(signIn, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(cancel, 0, Qt::AlignHCenter);
   layout->addStretch();

   setDarkMode(false);
}

void AccountSignIn::setDarkMode(bool value)
{
   isDarkMode = value;
   
   if (heading) heading->setStyleSheet(QString("color: %1;").arg(isDarkMode ? "white" : "black"));
   if (cancel) cancel->setDarkMode(isDarkMode);

   const QString userIcon = ":/icons/AccountSignIn/user.svg";
   const QString pwdIcon = ":/icons/AccountSignIn/password.svg";

   if (username) {
      username->setIconPaths(userIcon, userIcon);
      username->setDarkMode(isDarkMode);
   }

   if (pwd) {
      pwd->setIconPaths(pwdIcon,pwdIcon);
      pwd->setDarkMode(isDarkMode);
   }
}

TextField *AccountSignIn::usernameField() const
{
   return username;
}

TextField *AccountSignIn::pwdField() const
{
   return pwd;
}
