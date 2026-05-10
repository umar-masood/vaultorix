#include "Signup.h"

#include "../../../../resources/IconManager.h"
#include "../../../core/theme/ThemeManager.h"

#include "../../components/TextField.h"
#include "../../components/CheckBox.h"
#include "../../components/Button.h"
#include "../../components/ToolTip.h"
#include "../../components/Dialog.h"
#include "../../utils/Utils.h"

#include "../auth_window/AuthWindow.h"

#include "../../dialogs/error_dialog/ErrorDialog.h"
#include "../../dialogs/terms_conditions/TermsConditions.h"

using Ui::AuthWindow;
using Ui::Signup;
using namespace Ui::Utils;

/* =========================================================================================
                              ACCOUNT SIGN UP IMPLEMENTATION
   ========================================================================================= */
Signup::Signup(QWidget *parent, Ui::AuthWindow *authWindow) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setFocusPolicy(Qt::StrongFocus);

    // Heading
    heading = new QLabel(tr("Create an Account"));
    heading->setAttribute(Qt::WA_TranslucentBackground);
    heading->setFixedSize(QSize(276, 36));
    heading->setAlignment(Qt::AlignCenter);
    heading->setFont(font("Inter", 22, QFont::Bold));
    labels.append(heading);

    // Name
    _nameField = createTextField(tr("Enter your full name"), true);
    _nameField->setContextMenu(true);
    auto *nameLayout = createLabeledTextFieldLayout(tr("Full Name"), _nameField);
    connect(_nameField, &CustomTextField::textChanged, this, [this](){
        if (nameValidator)
            nameValidator->checkNameValidity(_nameField->text());
    });

    // Username
    _usernameField = createTextField(tr("Enter unique username"), true);
    _usernameField->setContextMenu(false);
    auto *usernameLayout = createLabeledTextFieldLayout(tr("Username"), _usernameField);

    usernameTimer = new QTimer(this);
    usernameTimer->setSingleShot(true);
    connect(usernameTimer, &QTimer::timeout, this, [this]() {
        QString text = _usernameField->text();

        if (text.isEmpty()) {
            _usernameField->setInvalid();
            _usernameField->setTooltip("Username is empty.");
            signupCore->setUsernameValidity(false);
            return;
        }

        if (usernameValidator)
            usernameValidator->checkUsernameValidityAndAvailability(text);
    });

    connect(_usernameField, &CustomTextField::textChanged, this, [this]() {
        usernameTimer->stop();
        usernameTimer->start(400);
    });

    // Email
    _emailField = createTextField(tr("Enter your email-address"), true);
    _emailField->setContextMenu(false);
    auto *emailLayout = createLabeledTextFieldLayout(tr("Email-Address"), _emailField);

    emailTimer = new QTimer(this);
    emailTimer->setSingleShot(true);
    connect(emailTimer, &QTimer::timeout, this, [this]() {
        QString text = _emailField->text();

        if (text.isEmpty()) {
            _emailField->setInvalid();
            _emailField->setTooltip("Email-address is empty.");
            signupCore->setEmailValidity(false);
            return;
        }

        if (emailValidator)
            emailValidator->checkEmailValidityAndAvailability(text);
    });

    connect(_emailField, &CustomTextField::textChanged, this, [this]() {
        emailTimer->stop();
        emailTimer->start(400);
    });

    // Password
    _passwordField = createTextField(tr("Enter strong password"));
    _passwordField->setContextMenu(false);
    _passwordField->setPasswordMode(true);
    auto *passwordLayout = createLabeledTextFieldLayout(tr("Password"), _passwordField);

    passwordTimer = new QTimer(this);
    passwordTimer->setSingleShot(true);

    connect(passwordTimer, &QTimer::timeout, this, [this]() {
        if (!passwordValidator)
            return;

        QString text = _passwordField->text();

        if (text.isEmpty()) {
            _passwordValidatorWidget->atLeastEight()->setInvalid();
            _passwordValidatorWidget->atLeastOneUpperCaseChar()->setInvalid();
            _passwordValidatorWidget->atLeastOneLowerCaseChar()->setInvalid();
            _passwordValidatorWidget->atLeastOneDigit()->setInvalid();
            _passwordValidatorWidget->atLeastOneSpecialChar()->setInvalid();
            _passwordValidatorWidget->strongPassword()->setInvalid();

            signupCore->setPasswordValidity(false);
            return;
        }

        passwordValidator->checkPasswordValidity(text);
    });

    connect(_passwordField, &CustomTextField::textChanged, this, [this]() {
        passwordTimer->stop();
        passwordTimer->start(400);
    });

    // Adding all field layouts to a vector
    fieldsLayouts = {nameLayout, usernameLayout, emailLayout, passwordLayout};

    // Validator
    _passwordValidatorWidget = new PasswordRules;

    // T&Cs Consent Widget
    _termsConditionsWidget = new CheckWithBtn;

    // Terms & Conditions Popup
    termsConditionsDialogWidget = new TermsConditions;
    termsConditionsDialog = new Dialog(termsConditionsDialogWidget, authWindow, true);
    // Signal Slot of T&Cs Dialog
    connect(_termsConditionsWidget, &CheckWithBtn::boxChecked, this, [this](bool checked) {
        signupCore->setTCValidity(checked);
    });
    connect(_termsConditionsWidget, &CheckWithBtn::onButtonClicked, this, [=]() { termsConditionsDialog->show(); });

    // Layout
    auto *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(heading, 0, Qt::AlignHCenter);
    layout->addSpacing(16);

    for (auto *fieldLayout : fieldsLayouts) {
        layout->addLayout(fieldLayout);
        layout->setAlignment(fieldLayout, Qt::AlignHCenter);
        layout->addSpacing(12);
    }

    layout->addWidget(_passwordValidatorWidget, 0, Qt::AlignLeft);
    layout->addSpacing(12);
    layout->addWidget(_termsConditionsWidget, 0, Qt::AlignLeft);

    // Create Account Button
    _createAccountBtn = new Button;
    _createAccountBtn->setCursor(Qt::PointingHandCursor);
    _createAccountBtn->setDisplayMode(Button::TextOnly);
    _createAccountBtn->setFixedSize(QSize(360, 36));
    _createAccountBtn->setLoaderButton(true);
    _createAccountBtn->setGradientColors("#008EDE", "#15F2FF", "#008EDE");
    _createAccountBtn->setText(tr("Create Account"));
    _createAccountBtn->setFontProperties("Segoe UI", 10, QFont::Normal);
    _createAccountBtn->setEnabled(false);
    connect(_createAccountBtn, &Button::clicked, this, &Signup::onCreateAccountBtnClicked);
    
    // Redirect to Sign In page widget
    _redirectToSignInWidget = new TextWithBtn(tr("Already have an account?"), QSize(150, 22), "Sign In", QSize(46, 20), false);
    _redirectToSignInWidget->button()->move(_redirectToSignInWidget->text()->width() + 4, -2); // Used manual movement to proper align it with prompt text of this widget.

    layout->addSpacing(18);
    layout->addWidget(_createAccountBtn, 0, Qt::AlignLeft);
    layout->addSpacing(16);
    layout->addWidget(_redirectToSignInWidget, 0, Qt::AlignCenter);
    layout->addSpacing(10);
    layout->addStretch();

    // Setting layout on widget
    setLayout(layout);

    // Theme Mode
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &Signup::setDarkMode);
    setDarkMode((tm.isDarkMode())); 

    // Signup Core
    signupCore = new Core::SignupService(this);
    connect(signupCore, &Core::SignupService::credentialsStored, this, &Signup::credentialsStored);
    connect(signupCore, &Core::SignupService::failedToStoreCredentials, this, &Signup::onFailedToStoreCredentials);
    connect(signupCore, &Core::SignupService::validationDone, this, [this](bool isDone) {
        _createAccountBtn->setEnabled(isDone);
    });

    // Validators
    nameValidator = new NameValidator(this);
    connect(nameValidator, &NameValidator::nameValidated, this, &Signup::onNameValidated);

    usernameValidator = new UsernameValidator(this);
    connect(usernameValidator, &UsernameValidator::usernameAvailable, this, &Signup::onUsernameAvailable);
    connect(usernameValidator, &UsernameValidator::usernameInvalid, this, &Signup::onUsernameInvalid);
    connect(usernameValidator, &UsernameValidator::failedToCheckUsername, this, &Signup::onFailedToCheckUsername);

    emailValidator = new EmailValidator(this);
    connect(emailValidator, &EmailValidator::emailAvailable, this, &Signup::onEmailAvailable);
    connect(emailValidator, &EmailValidator::emailInvalid, this, &Signup::onEmailInvalid);
    connect(emailValidator, &EmailValidator::failedToCheckEmail, this, &Signup::onFailedToCheckEmail);

    passwordValidator = new PasswordValidator(this);
    connect(passwordValidator, &PasswordValidator::validationUpdated, this, &Signup::onPasswordValidationUpdated);

}

/* --------------------  Setters  -----------------  */
void Signup::setDarkMode(bool isDarkMode) {
    // Labels (Email, Username, Password show above each TextField) inside field widgets
    for (auto *label : labels)
        label->setStyleSheet(QString("color: %1;").arg(isDarkMode ? "white" : "black"));

    // TextFields inside fields layouts such as emailLayout, usernameLayout etc.
    for (auto *field : {_nameField, _usernameField, _passwordField, _emailField})
        field->setDarkMode(isDarkMode);

    // T&Cs Consent Widget (CheckBox + Button to T&Cs dialog)
    _termsConditionsWidget->setDarkMode(isDarkMode);

    // Terms and Conditions Dialog Box Theme Handling
    termsConditionsDialogWidget->setDarkMode(isDarkMode);
    termsConditionsDialog->setDarkMode(isDarkMode);

    // Redirect to Sign In page widget
    _redirectToSignInWidget->text()->setStyleSheet(QString("color: %1").arg(isDarkMode ? "white" : "black"));
}

/* ------------------  Getters -------------------  */
TextWithBtn *Signup::redirectToSignin() const { return _redirectToSignInWidget; }
CheckWithBtn *Signup::termsConditionsWidget() const { return _termsConditionsWidget; }
PasswordRules *Signup::passwordValidatorWidget() const { return _passwordValidatorWidget; }

/* --------------------  Slots    -----------------  */
void Signup::onCreateAccountBtnClicked() {
    updateCreateAccountBtnState(false, "");
    
    Core::SignupService::SignupData data;
    data.fullName = _nameField->text();
    data.username = _usernameField->text();
    data.password = _passwordField->text();
    data.email    = _emailField->text();

    if (signupCore)
        signupCore->storeCredentials(data);
}

void Signup::onFailedToStoreCredentials(const Core::SignupService::Error &error) {
    switch (error) {
        case Core::SignupService::Error::SomethingWentWrong:
        handleCreateAccountError("SomethingWentWrong", true);
        break;

        case Core::SignupService::Error::MaxAttemptsLimitReached:
        handleCreateAccountError("MaxAttempts");
    }
}

void Signup::onNameValidated(bool isValid) {
    QString text = _nameField->text();

    if (text.isEmpty()) {
        _nameField->setInvalid();
        _nameField->setTooltip("Full name is empty.");

        signupCore->setNameValidity(false);
        return;
    }
    
    signupCore->setNameValidity(isValid);

    if (isValid) {        
        _nameField->setValid();
        _nameField->setTooltip("Valid full name");
    } else {
        _nameField->setInvalid();
        _nameField->setTooltip("Invalid full name");
    }
}

void Signup::onUsernameInvalid() {
    _usernameField->setInvalid();
    _usernameField->setTooltip(
    "Invalid username.\n\n"
    "Please ensure that:\n"
    "• 3-20 characters, starting with a letter\n"
    "• Letters, numbers, . _ - only\n"
    "• Cannot start or end with . _ -\n"
    "• No excessive character repetition"
    );

    signupCore->setUsernameValidity(false);
}

void Signup::onUsernameAvailable(bool isAvailable) {
    if (isAvailable) {
        _usernameField->setValid();
        _usernameField->setTooltip("Username is available.");
    } else {
        _usernameField->setInvalid();
        _usernameField->setTooltip("Username is already taken.\nPlease use another.");
    }
    
    signupCore->setUsernameValidity(isAvailable);
}

void Signup::onFailedToCheckUsername() {
    if (usernameRetryAttempts < 3) {
        usernameRetryAttempts++;
        usernameValidator->checkUsernameValidityAndAvailability(_usernameField->text());
    } else {
        _usernameField->setInvalid();
        _usernameField->setTooltip("Failed to check username availability.");
        usernameRetryAttempts = 0;
        signupCore->setUsernameValidity(false);
    }
}

void Signup::onEmailInvalid() {
    _emailField->setInvalid();
    _emailField->setTooltip(
        "Invalid email address.\n\n"
        "Please ensure that:\n"
        "• The email format is correct\n"
        "• No invalid or spam characters are used\n"
        "• The domain name is valid\n"
        "• The email address is unique"
    );
    signupCore->setEmailValidity(false);
}

void Signup::onEmailAvailable(bool isAvailable) {
    if (isAvailable) {
        _emailField->setValid();
        _emailField->setTooltip("Email-address is available.");
    } else {
        _emailField->setInvalid();
        _emailField->setTooltip("Email-address is already taken.\nPlease use another.");
    }
    
    signupCore->setEmailValidity(isAvailable);
}

void Signup::onFailedToCheckEmail() {
    if (emailRetryAttempts < 3) {
        emailRetryAttempts++;
        emailValidator->checkEmailValidityAndAvailability(_emailField->text());
    } else {
        _emailField->setInvalid();
        _emailField->setTooltip("Failed to check email availability.");
        emailRetryAttempts = 0;
        signupCore->setEmailValidity(false);
    }
}

void Signup::onPasswordValidationUpdated(const PasswordValidator::PasswordValidationResult &result) {
    signupCore->setPasswordValidity(result.isStrong);

    result.hasLength ?
        _passwordValidatorWidget->atLeastEight()->setValid() :
        _passwordValidatorWidget->atLeastEight()->setInvalid();

    result.hasUpper ?
        _passwordValidatorWidget->atLeastOneUpperCaseChar()->setValid() :
        _passwordValidatorWidget->atLeastOneUpperCaseChar()->setInvalid();

    result.hasLower ?
        _passwordValidatorWidget->atLeastOneLowerCaseChar()->setValid() :
        _passwordValidatorWidget->atLeastOneLowerCaseChar()->setInvalid();

    result.hasDigit ?
        _passwordValidatorWidget->atLeastOneDigit()->setValid() :
        _passwordValidatorWidget->atLeastOneDigit()->setInvalid();

    result.hasSpecial ?
        _passwordValidatorWidget->atLeastOneSpecialChar()->setValid() :
        _passwordValidatorWidget->atLeastOneSpecialChar()->setInvalid();

    result.isStrong ?
        _passwordValidatorWidget->strongPassword()->setValid() :
        _passwordValidatorWidget->strongPassword()->setInvalid();
}
/* --------------------  Helpers  -----------------  */
CustomTextField *Signup::createTextField(const QString &placeholderText, bool hasValidity) {
    auto *field = new CustomTextField(hasValidity);
    field->setPlaceholderText(placeholderText);
    field->setFixedSize(QSize(360, 36));
    return field;
}

QVBoxLayout *Signup::createLabeledTextFieldLayout(const QString &labelName, CustomTextField *currField) {
    auto *label = new QLabel;
    label->setAttribute(Qt::WA_TranslucentBackground);
    label->setText(labelName);
    label->setFont(font());
    labels.append(label);

    auto *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(label, 0, Qt::AlignLeft);
    layout->addSpacing(6);
    layout->addWidget(currField, 0, Qt::AlignLeft);
    layout->addStretch();

    return layout;
}

void Signup::updateCreateAccountBtnState(bool isEnabled, const QString &text) {
    _createAccountBtn->setEnabled(isEnabled);
    _createAccountBtn->setText(text);
}

void Signup::handleCreateAccountError(const QString &errorName, bool createAccButtonEnabled, const QString &createAccButtonText) {
    ErrorDialogManager::instance()->show(errorName, "Auth"); 
    updateCreateAccountBtnState(createAccButtonEnabled, createAccButtonText);
}

QFont Signup::font(const QString &family, int fontSize, QFont::Weight weight) {
    QFont font;
    font.setFamily(family);
    font.setPointSize(fontSize);
    font.setWeight(weight);
    return font;
}