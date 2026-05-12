#include "Signin.h"

#include "../../../../resources/IconManager.h"
#include "../../../../resources/IllustrationManager.h"

#include "../../../core/theme/ThemeManager.h"
#include "../../../core/session/SessionManager.h"

#include "../../dialogs/error_dialog/ErrorDialog.h"
#include "../../vault/vault_window/VaultWindow.h"
#include "../auth_window/AuthWindow.h"
#include "../../utils/Utils.h"

#include <QApplication>

using Ui::Signin;
using Ui::Utils::TextWithBtn;

/* =========================================================================================
                              ACCOUNT SIGN IN IMPLEMENTATION
   ========================================================================================= */
Signin::Signin(QWidget *parent, Ui::AuthWindow *authWindow) : QWidget(parent), _authWindow(authWindow) {
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::StrongFocus);

    // Icons
    UserIcon = IconManager::icon(Icons::UserAvator);
    KeyIcon = IconManager::icon(Icons::Key);

    // Main Icon
    illustration = new Label(true);
    illustration->setFixedSize(QSize(150, 150));
    illustration->setPixmap(QPixmap(IllustrationManager::illustration(Illustrations::SignIn))
                                .scaled(150, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    illustration->setScaledContents(true);

    // Heading
    heading = new Label("Inter", 22, QFont::Bold, false, "Sign in");

    // Text under Heading (Welcome Text)
    text = new Label("Segoe UI", 10, QFont::Medium, false, "Hi, Welcome back!");
    text->setWordWrap(true);
    text->setStyleSheet("color: #8D8D8D;");
    text->setFixedWidth(324);

    // Username Field
    _usernameField = new TextField;
    _usernameField->setPlaceholderText("Username");
    _usernameField->setContextMenu(false);
    _usernameField->setFixedSize(QSize(360, 36));
    _usernameField->setIconic(true);
    _usernameField->setClearButton(true);
    _usernameField->setIconSize(QSize(18, 18));

    // Password Field
    _passwordField = new TextField;
    _passwordField->setContextMenu(false);
    _passwordField->setPlaceholderText("Password");
    _passwordField->setFixedSize(QSize(360, 36));
    _passwordField->setPasswordMode(true);
    _passwordField->setIconic(true);
    _passwordField->setIconSize(QSize(18, 18));

    // Forgot Password Field
    _forgotPwdBtn = new Button("Forgot Password?");
    _forgotPwdBtn->setCursor(Qt::PointingHandCursor);
    _forgotPwdBtn->setDisplayMode(Button::TextOnly);
    _forgotPwdBtn->setFixedSize(QSize(110, 18));
    _forgotPwdBtn->setFontProperties("Segoe UI", 10);
    _forgotPwdBtn->setHyperLink(true);

    // Sign In Button
    _signInBtn = new Button("Sign in");
    _signInBtn->setCursor(Qt::PointingHandCursor);
    _signInBtn->setDisplayMode(Button::TextOnly);
    _signInBtn->setFixedSize(QSize(360, 36));
    _signInBtn->setGradientColors("#008EDE", "#15F2FF", "#008EDE");
    _signInBtn->setFontProperties("Segoe UI", 10, QFont::Normal);
    _signInBtn->setLoaderButton(true);
    connect(_signInBtn, &Button::clicked, this, &Signin::onSignInClicked);

    // Cancel Button
    _cancelBtn = new Button("Cancel");
    _cancelBtn->setCursor(Qt::PointingHandCursor);
    _cancelBtn->setDisplayMode(Button::TextOnly);
    _cancelBtn->setSecondary(true);
    _cancelBtn->setFixedSize(QSize(360, 36));
    _cancelBtn->setFontProperties("Segoe UI", 10, QFont::Normal);
    connect(_cancelBtn, &Button::clicked, this, &Signin::onCancelClicked);

    // Sign up page redirect widget
    _redirectToSignUpWidget = new TextWithBtn("Don't have an account?", QSize(136, 22), "Create an account", QSize(110, 12), false);

    // Main Layout
    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->addStretch();

    layout->addWidget(illustration, 0, Qt::AlignHCenter);
    layout->addSpacing(16);
    layout->addWidget(heading, 0, Qt::AlignHCenter);
    layout->addSpacing(20);
    layout->addWidget(text, 0, Qt::AlignHCenter);
    layout->addSpacing(20);

    layout->addWidget(_usernameField, 0, Qt::AlignHCenter);
    layout->addSpacing(16);
    layout->addWidget(_passwordField, 0, Qt::AlignHCenter);

    layout->addSpacing(20);
    layout->addWidget(_forgotPwdBtn, 0, Qt::AlignRight);
    layout->addSpacing(20);

    layout->addWidget(_signInBtn, 0, Qt::AlignHCenter);
    layout->addSpacing(16);
    layout->addWidget(_cancelBtn, 0, Qt::AlignHCenter);

    layout->addSpacing(20);
    layout->addWidget(_redirectToSignUpWidget, 0, Qt::AlignHCenter);
    layout->addStretch();

    // Theme Mode
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &Signin::setDarkMode);
    setDarkMode((tm.isDarkMode())); 

    // Signin Core
    signInCore = new Core::SigninService(this);

    // Signal Slots
    connect(signInCore, &Core::SigninService::failedToSignIn, this, &Signin::onFailedToSignIn);
    connect(signInCore, &Core::SigninService::signedIn, this, &Signin::onSignedIn);
    connect(signInCore, &Core::SigninService::verificationRequired, this, &Signin::verificationRequired);
    connect(ErrorDialogManager::instance(), &ErrorDialogManager::actionTriggered, this, &Signin::onErrorDialogActionBtnClicked);
}

/* --------------------  Setters  -----------------  */
void Signin::setDarkMode(bool isDarkMode) {
    // Header
    heading->setStyleSheet(QString("color: %1;").arg(isDarkMode ? "white" : "black"));

    // Cancel Button
    _cancelBtn->setDarkMode(isDarkMode);

    // Username TextField
    _usernameField->setIconPaths(UserIcon, UserIcon);
    _usernameField->setDarkMode(isDarkMode);

    // Password TextField
    _passwordField->setIconPaths(KeyIcon, KeyIcon);
    _passwordField->setDarkMode(isDarkMode);

    // Redirect to Sign Up page widget
    _redirectToSignUpWidget->text()->setStyleSheet(QString("color: %1").arg(isDarkMode ? "white" : "black"));
}

/* -------------------   Slots   ------------------- */
void Signin::updateSignInBtnState(bool isEnabled, const QString &text) {
    _signInBtn->setEnabled(isEnabled);
    _signInBtn->setText(text);
}

void Signin::handleSignInError(const QString &errorName, bool isSignInButtonEnabled, const QString &signInButtonText) {
    ErrorDialogManager::instance()->show(errorName, "Auth"); 
    updateSignInBtnState(isSignInButtonEnabled, signInButtonText);
}

void Signin::onErrorDialogActionBtnClicked(const QString &key) {
    if (key == "MaxAttempts") 
        QApplication::quit();
}

void Signin::onSignInClicked() {
    QString username = _usernameField->text();
    QString password = _passwordField->text();

    if (username.isEmpty() || password.isEmpty()) return;
    if (username.length() < 3 || password.length() < 8) return;

    if (signInCore) {
        updateSignInBtnState(false, "");
        signInCore->verifyCredentials(username, password);
    }
}

void Signin::onSignedIn(const QJsonObject &obj) {
    updateSignInBtnState(false, "Signed In");

    SessionManager::instance().loadFromJson(obj);

    if (_authWindow) {
        _authWindow->close();
        _authWindow->deleteLater();
    }

    for (auto *field : {_usernameField, _passwordField}) {
        field->setText("");
        field->setEnabled(false);
    }

    Ui::Vault::VaultWindow::instance()->show();
}

using SignInError = Core::SigninService::SignInError;
void Signin::onFailedToSignIn(const SignInError &error) {
    switch (error) {
        case SignInError::AccessDenied:              handleSignInError("AccessDenied", true);           break;
        case SignInError::SomethingWentWrong:        handleSignInError("SomethingWentWrong", true);     break;
        case SignInError::InvalidCredentials:        handleSignInError("InvalidCredentials", true);     break;
        case SignInError::MaxAttemptsLimitReached:   handleSignInError("MaxAttempts", false);           break;
    }
}

void Signin::onCancelClicked() {
    QApplication::quit();
}