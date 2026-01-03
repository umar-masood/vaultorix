#include "AccountSignIn.h"

AccountSignIn::AccountSignIn(QWidget *parent) : QWidget(parent) {
   setAttribute(Qt::WA_TranslucentBackground);

   // Main Icon
   illustration = new Label(true);
   illustration->setFixedSize(QSize(150, 150));
   illustration->setPixmap(QPixmap(":/icons/AccountSignIn/sign-in.png").scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
   illustration->setScaledContents(true);

   // Heading
   heading = new Label(false, "Inter", 22, QFont::Bold, false, "Sign in to your Account");
   
   // Text under Heading (Welcome Text)
   text = new Label(false, "Segoe UI", 10, QFont::Medium, false, "Hi, Welcome back!");
   text->setWordWrap(true);
   text->setStyleSheet("color: #8D8D8D;");
   text->setFixedWidth(324);

   // Username Field
   username = new TextField;
   username->setPlaceholderText("Username");
   username->setContextMenu(false);
   username->setFixedSize(QSize(360, 36));
   username->setTextFieldIcon(true);
   username->setClearButton(true);
   username->setTextFieldIconSize(QSize(18,18));
   
   // Password Field
   password = new TextField;
   password->setContextMenu(false);
   password->setPlaceholderText("Password");
   password->setFixedSize(QSize(360, 36));
   password->setPasswordTextField(true);
   password->setTextFieldIcon(true);
   password->setTextFieldIconSize(QSize(18,18));

   // Forgot Password Field
   forgotPwd = new Button("Forgot Password?");
   forgotPwd->setDisplayMode(Button::TextOnly);
   forgotPwd->setFixedSize(QSize(110, 18));
   forgotPwd->setHyperLink(true);
   forgotPwd->setFontProperties("Segoe UI", 10, false, false);
   forgotPwd->setHyperLinkColors("#008EDE", "#15F2FF");
   connect(forgotPwd, &Button::clicked, this, [this]() { emit forgotPwdClicked(); });

   // Sign In Button
   signInBtn = new Button("Sign in");
   signInBtn->setDisplayMode(Button::TextOnly);
   signInBtn->setFixedSize(QSize(360, 36));
   signInBtn->setGradientColor(true, "#008EDE", "#15F2FF");
   signInBtn->setHoverGradientColor("#008EDE");
   signInBtn->setFontProperties("Segoe UI", 11, true, false);
   signInBtn->setLoaderButton(true);
   connect(signInBtn, &Button::clicked, this, [this]() { emit signInClicked(); });

   // Cancel Button
   cancelBtn = new Button("Cancel");
   cancelBtn->setDisplayMode(Button::TextOnly);
   cancelBtn->setSecondary(true);
   cancelBtn->setFixedSize(QSize(360, 36));
   cancelBtn->setFontProperties("Segoe UI", 11, true, false);
   connect(cancelBtn, &Button::clicked, this, [this]() { emit cancelClicked(); });

   // Main Layout
   layout = new QVBoxLayout(this);
   layout->setSpacing(0);
   layout->addStretch();
   layout->addWidget(illustration, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(heading, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(text, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(username, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(password, 0, Qt::AlignHCenter);
   layout->addSpacing(20);
   layout->addWidget(forgotPwd, 0, Qt::AlignRight);
   layout->addSpacing(26);
   layout->addWidget(signInBtn, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(cancelBtn, 0, Qt::AlignHCenter);
   layout->addStretch();

   setDarkMode(false);
}

void AccountSignIn::setDarkMode(bool value) {
   isDarkMode = value;
   
   if (heading) heading->setStyleSheet(QString("color: %1;").arg(isDarkMode ? "white" : "black"));
   if (cancelBtn) cancelBtn->setDarkMode(isDarkMode);

   if (username) {
      username->setIconPaths(userIcon, userIcon);
      username->setDarkMode(isDarkMode);
   }

   if (password) {
      password->setIconPaths(passwordIcon,passwordIcon);
      password->setDarkMode(isDarkMode);
   }
}

TextField *AccountSignIn::usernameField() const { return username; }
TextField *AccountSignIn::passwordField() const { return password; }
Button* AccountSignIn::signInButton() const { return signInBtn; }  