#pragma once

#include <QVBoxLayout>
#include <QVector>
#include <QWidget>
#include <QLabel>
#include <QFont>
#include <QTimer>

#include "../../../core/services/auth/SignupService.h"
#include "../../../core/validators/auth/NameValidator.h"
#include "../../../core/validators/auth/PasswordValidator.h"
#include "../../../core/validators/auth/UsernameValidator.h"
#include "../../../core/validators/auth/EmailValidator.h"

class Dialog;
class TermsConditions;
class Button;

namespace Ui::Utils { 
    class CustomTextField; 
    class TextWithBtn;
    class PasswordRules;
    class CheckWithBtn;
};

namespace Ui { class AuthWindow; };
namespace Ui {
    /*  ------------------------- Account Signup ------------------ */
    class Signup : public QWidget {
        Q_OBJECT

        public:
        explicit Signup(QWidget *parent = nullptr, Ui::AuthWindow *authWindow = nullptr);
        void setDarkMode(bool isDarkMode);

        Ui::Utils::TextWithBtn *redirectToSignin() const;
        Ui::Utils::CheckWithBtn *termsConditionsWidget() const;
        Ui::Utils::PasswordRules *passwordValidatorWidget() const;

        private:
        // Heading
        QLabel *heading = nullptr;

        // Name, Password, Username, Email Fields
        Ui::Utils::CustomTextField *_nameField = nullptr;
        Ui::Utils::CustomTextField *_passwordField = nullptr;
        Ui::Utils::CustomTextField *_usernameField = nullptr;
        Ui::Utils::CustomTextField *_emailField = nullptr;

        // Holds all labels inside fields widgets
        QVector<QLabel *> labels;

        // Hold all field widgets
        QVector<QVBoxLayout *> fieldsLayouts;

        // Password Rules Validator Widget
        Ui::Utils::PasswordRules *_passwordValidatorWidget = nullptr;

        // Terms & Conditions
        Ui::Utils::CheckWithBtn *_termsConditionsWidget = nullptr;
        Dialog *termsConditionsDialog = nullptr;
        TermsConditions *termsConditionsDialogWidget = nullptr;

        // Create Account Button
        Button *_createAccountBtn = nullptr;

        // Redirect to Sign In page widget
        Ui::Utils::TextWithBtn *_redirectToSignInWidget = nullptr;

        // Core
        Core::SignupService *signupCore = nullptr;
        NameValidator *nameValidator = nullptr;

        UsernameValidator *usernameValidator = nullptr;
        int usernameRetryAttempts = 0;
        QTimer *usernameTimer;

        EmailValidator *emailValidator = nullptr;
        int emailRetryAttempts = 0;
        QTimer *emailTimer;

        PasswordValidator *passwordValidator = nullptr;
        QTimer *passwordTimer = nullptr;

        // Helper Functions
        Ui::Utils::CustomTextField *createTextField(const QString &placeholderText = QString(), 
                                                    bool hasValidity = false);

        QVBoxLayout *createLabeledTextFieldLayout(const QString &labelName = QString(), 
                                                  Ui::Utils::CustomTextField *currField = nullptr);

        QFont font(const QString &family = "Segoe UI", 
                   int fontSize = 10, 
                   QFont::Weight weight = QFont::Medium);
                
        void updateCreateAccountBtnState(bool isEnabled, const QString &text);
        void handleCreateAccountError(const QString &errorName, 
                                      bool createAccButtonEnabled = false, 
                                      const QString &createAccButtonText = "Create Account");

        signals:
        void credentialsStored(const QString &email);

        private slots:
        void onCreateAccountBtnClicked();
        void onFailedToStoreCredentials(const Core::SignupService::Error &error);
        void onNameValidated(bool isValid);
        void onUsernameInvalid();
        void onUsernameAvailable(bool isAvailable);
        void onFailedToCheckUsername();
        void onEmailInvalid();
        void onEmailAvailable(bool isAvailable);
        void onFailedToCheckEmail();
        void onPasswordValidationUpdated(const PasswordValidator::PasswordValidationResult &result);

    };
};