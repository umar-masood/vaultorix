#include "AccountSignIn.h"

/* ========================================================================================= 
                              ACCOUNT SIGN IN IMPLEMENTATION              
   ========================================================================================= */
AccountSignIn::AccountSignIn(QWidget *parent) : QWidget(parent) {
   setAttribute(Qt::WA_TranslucentBackground);
   setFocusPolicy(Qt::StrongFocus);

   // Main Icon
   illustration = new Label(true);
   illustration->setFixedSize(QSize(150, 150));
   illustration->setPixmap(QPixmap(":/icons/accountSignIn/sign-in.png").scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
   illustration->setScaledContents(true);

   // Heading
   heading = new Label(false, "Inter", 22, QFont::Bold, false, "Sign in");
   
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
   forgotPwd->setCursor(Qt::PointingHandCursor);
   forgotPwd->setDisplayMode(Button::TextOnly);
   forgotPwd->setFixedSize(QSize(110, 18));
   forgotPwd->setHyperLink(true);
   forgotPwd->setFontProperties("Segoe UI", 10, false, false);
   forgotPwd->setHyperLinkColors("#008EDE", "#15F2FF");
   connect(forgotPwd, &Button::clicked, this, [this]() { emit forgotPwdClicked(); });

   // Sign In Button
   signInBtn = new Button("Sign in");
   signInBtn->setCursor(Qt::PointingHandCursor);
   signInBtn->setDisplayMode(Button::TextOnly);
   signInBtn->setFixedSize(QSize(360, 36));
   signInBtn->setGradientColor(true, "#008EDE", "#15F2FF");
   signInBtn->setHoverGradientColor("#008EDE");
   signInBtn->setFontProperties("Segoe UI", 11, true, false);
   signInBtn->setLoaderButton(true);
   connect(signInBtn, &Button::clicked, this, [this]() { emit signInClicked(); });

   // Cancel Button
   cancelBtn = new Button("Cancel");
   cancelBtn->setCursor(Qt::PointingHandCursor);
   cancelBtn->setDisplayMode(Button::TextOnly);
   cancelBtn->setSecondary(true);
   cancelBtn->setFixedSize(QSize(360, 36));
   cancelBtn->setFontProperties("Segoe UI", 11, true, false);
   connect(cancelBtn, &Button::clicked, this, [this]() { emit cancelClicked(); });

   // Sign up page redirect widget
   _redirectToSignUpWidget = new TextWithBtn("Don't have an account?", QSize(136,22), "Create an account", QSize(110,12), false);

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
   layout->addSpacing(20);

   layout->addWidget(signInBtn, 0, Qt::AlignHCenter);
   layout->addSpacing(16);
   layout->addWidget(cancelBtn, 0, Qt::AlignHCenter);
   
   layout->addSpacing(20);
   layout->addWidget(_redirectToSignUpWidget, 0, Qt::AlignHCenter);
   layout->addStretch();

   // Initial Theme
   setDarkMode(false);
}

/* --------------------  Setters  -----------------  */
void AccountSignIn::setDarkMode(bool value) {
   isDarkMode = value;
 
   // Header (Account Sign In)
   heading->setStyleSheet(QString("color: %1;").arg(isDarkMode ? "white" : "black"));

   // Cancel Button
   cancelBtn->setDarkMode(isDarkMode);

   // Username TextField
   username->setIconPaths(userIcon, userIcon);
   username->setDarkMode(isDarkMode);
   
   // Password TextField
   password->setIconPaths(passwordIcon,passwordIcon);
   password->setDarkMode(isDarkMode);
   
   // Redirect to Sign Up page widget
   _redirectToSignUpWidget->text()->setStyleSheet(QString("color: %1").arg(isDarkMode ? "white" : "black")); 
}

/* --------------------  Getters  -----------------  */
TextField* AccountSignIn::usernameField() const { return username; }
TextField* AccountSignIn::passwordField() const { return password; }
TextWithBtn *AccountSignIn::redirectToSignUpWidget() const { return _redirectToSignUpWidget; }
Button *AccountSignIn::signInButton() const { return signInBtn; }