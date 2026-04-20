#pragma once
#include "../../components/TextField.h"
#include "../../components/CheckBox.h"
#include "../../components/Button.h"
#include "../../components/ToolTip.h"
#include "../../components/Dialog.h"

#include "../../dialogs/terms_conditions/TermsConditions.h"

#include "../auth_window/AuthWindow.h"
#include "../otp/Otp.h"

#include <QLabel>

/* ---------------- Password Validation Rules Items --------------------- */
class RuleItem : public QWidget {
    Q_OBJECT

    public:
    RuleItem(const QString &ruleTxt, QWidget *parent = nullptr);
    void setValid();
    void setInvalid();

    protected:
    void paintEvent(QPaintEvent *event) override;

    private:
    // Flag to check validity of each rule
    bool validityState = false;

    // Icon Size
    QSize iconSize = QSize(20, 20);

    // Different Icons for different states
    QPixmap validIcon, invalidIcon;

    // Text Color Codes
    QString uncoloredText = "#8D8D8D";
    QString coloredText = "#009138";

    QString text;
    QFont font;
};

class PasswordRules : public QWidget {
    Q_OBJECT

    public:
    explicit PasswordRules(QWidget *parent = nullptr);

    // Getters of Password Validation Rules Widgets
    RuleItem *atLeastEight() const;
    RuleItem *atLeastOneLowerCaseChar() const;
    RuleItem *atLeastOneUpperCaseChar() const;
    RuleItem *atLeastOneDigit() const;
    RuleItem *atLeastOneSpecialChar() const;
    RuleItem *strongPassword() const;

    private:
    // Main Layout
    QVBoxLayout *mainLayout = nullptr;

    // Password Validation Rules Widgets
    RuleItem *_atLeastEightChars = nullptr;
    RuleItem *_atLeastOneLowerChar = nullptr;
    RuleItem *_atLeastOneUpperChar = nullptr;
    RuleItem *_atLeastOneDigit = nullptr;
    RuleItem *_atLeastOneSpecialChar = nullptr;
    RuleItem *_strongPassword = nullptr;
};

/* -------------- Customized TextField --------------- */
class CustomTextField : public TextField {
    Q_OBJECT

    public:
    explicit CustomTextField(bool hasValidity = false, QWidget *parent = nullptr);
    void setValid();
    void setInvalid();
    void setDarkMode(bool value) override;
    void setTooltip(const QString &tooltipText);
    QLabel *validityIcon() const;

    private:
    // Validity Icon
    QLabel *_validityIcon = nullptr;
    // ToolTip
    ToolTip *tooltip = nullptr;

    // Flag
    bool hasTip = false;
};

/* ------------------ CheckBox with Button (In this case for Terms & Conditions Acceptance) -------------- */
class CheckWithBtn : public QWidget {
    Q_OBJECT

    public:
    explicit CheckWithBtn(QWidget *parent = nullptr);
    void setDarkMode(bool value);
    Button *button() const;
    CheckBox *checkBox() const;

    private:
    // Theme Mode Flag
    bool isDarkMode = false;

    // CheckBox
    CheckBox *_checkbox = nullptr;

    // HyperLink Button
    Button *_button = nullptr;

    signals:
    void onButtonClicked();
    void boxChecked(bool checked);
};

namespace Ui::Auth {
    /*  ------------------------- Account Signup ------------------ */
    class Signup : public QWidget {
        Q_OBJECT

        public:
        explicit Signup(QWidget *parent = nullptr, AuthWindow *authWindow = nullptr);
        void setDarkMode(bool isDarkMode);

        Button *createAccountButton() const;
        CustomTextField *nameField() const;
        CustomTextField *usernameField() const;
        CustomTextField *passwordField() const;
        CustomTextField *emailField() const;
        TextWithBtn *redirectToSignin() const;
        CheckWithBtn *termsConditionsWidget() const;
        PasswordRules *passwordValidatorWidget() const;

        private:
        // Heading
        QLabel *heading = nullptr;

        // Name
        CustomTextField *name = nullptr;
        QVBoxLayout *nameLayout = nullptr;

        // Password
        CustomTextField *password = nullptr;
        QVBoxLayout *passwordLayout = nullptr;

        // Username
        CustomTextField *username = nullptr;
        QVBoxLayout *usernameLayout = nullptr;

        // Email
        CustomTextField *email = nullptr;
        QVBoxLayout *emailLayout = nullptr;

        // Holds all labels inside fields widgets
        QVector<QLabel *> labels;

        // Hold all field widgets
        QVector<QVBoxLayout *> fieldsLayouts;

        // Layout
        QVBoxLayout *layout = nullptr;

        // Password Rules Validator
        PasswordRules *_passwordValidatorWidget = nullptr;

        // Terms & Conditions Cosent Widget
        CheckWithBtn *_termsConditionsWidget = nullptr;

        // Terms & Conditions Dialog
        Dialog *termsConditionsDialog = nullptr;
        TermsConditions *termsConditionsDialogWidget = nullptr;

        // Create Account Button
        Button *createAccBtn = nullptr;

        // Redirect to Sign In page widget
        TextWithBtn *_redirectToSignInWidget = nullptr;

        // Helper Functions
        CustomTextField *createTextField(const QString &placeholderText = QString(), bool hasValidity = false);
        QVBoxLayout *createLabeledTextFieldLayout(const QString &labelName = QString(), CustomTextField *currField = nullptr);
        QFont font(const QString &family = "Segoe UI", int fontSize = 10, QFont::Weight weight = QFont::Medium);

        signals:
        void onNameEntered();
        void onUsernameEntered();
        void onEmailEntered();
        void onPasswordEntered();
    };
};