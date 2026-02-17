#pragma once
#include "../../components/TextField.h" 
#include "../pwdRulesWidget/PwdRulesWidget.h"
#include "../../components/CheckBox.h"
#include "../../components/Button.h"
#include "../../components/ToolTip.h"
#include "../../dialogs/termsConditions/TermsConditions.h"
#include "../../components/Dialog.h"
#include "../accountWindow/AccountWindow.h"
#include "../accountOTP/AccountOTP.h"

#include <QLabel>

/* -------------- Customized TextField --------------- */
class CustomTextField : public TextField {
   Q_OBJECT

   public:
   explicit CustomTextField(bool hasValidity = false, QWidget *parent = nullptr);
   void setValid();
   void setInvalid();
   void setDarkMode(bool value) override;
   void setTooltip(const QString &tooltipText);

   private:
   // Validity Icon
   QLabel *validityIcon = nullptr;
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
   Button* button() const;
   CheckBox* checkBox() const;

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

/*  ------------------------- Account Create ------------------ */
class AccountCreate : public QWidget {
   Q_OBJECT

   public:
   explicit AccountCreate(QWidget *parent = nullptr, AccountWindow *accountWindow = nullptr);
   Q_INVOKABLE void setDarkMode(bool value);
   
   Button* createAccountButton() const;
   CustomTextField* nameField() const;
   CustomTextField* usernameField() const;
   CustomTextField* passwordField() const;
   CustomTextField* emailField() const;
   TextWithBtn* redirectToSignInWidget() const;
   CheckWithBtn* termsConditionsWidget() const;
   PwdRulesWidget* passwordValidatorWidget() const;
   
   private:
   // Theme Mode Flag
   bool isDarkMode = false;

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
   PwdRulesWidget *_passwordValidatorWidget = nullptr;

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
   QVBoxLayout *createLabeledTextFieldLayout(const QString labelName = QString(), CustomTextField *currField = nullptr);
   QFont font(const QString &family = "Segoe UI", int fontSize = 10, QFont::Weight weight = QFont::Medium);

   signals:
   void onNameEntered();
   void onUsernameEntered();
   void onEmailEntered();
   void onPasswordEntered();
};
