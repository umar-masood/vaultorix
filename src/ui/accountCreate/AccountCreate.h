#pragma once
#include "../components/TextField.h" 
#include "../pwdRulesWidget/PwdRulesWidget.h"
#include "../components/CheckBox.h"
#include "../components/Button.h"
#include "../components/ToolTip.h"
#include "../dialogs/termsConditions/TermsConditions.h"
#include "../components/Dialog.h"
#include "../accountWindow/AccountWindow.h"
#include <QLabel>

/* -------------- Customized TextField --------------- */
class CustomTextField : public TextField {
   Q_OBJECT

   public:
   explicit CustomTextField(bool useCheck = false, QWidget *parent = nullptr);
   void setChecked();
   void setUnchecked();
   void setDarkMode(bool value) override;
   void setTooltip(const QString &tooltipText);

   private:
   // Check Icon
   QLabel *checkIcon = nullptr;
   
   // ToolTop showing on Check Icon
   ToolTip *tooltip = nullptr;
   // Flag
   bool hasTip = false;
   
   // Icon Paths
   const QString checked = ":/icons/AccountCreate/checked.svg";
   const QString unchecked = ":/icons/AccountCreate/unchecked.svg";
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
   void themeModeChanged(bool enable);
   void onButtonClicked();
   void boxChecked(bool checked);
};

/*  ------------------------- Account Create Main Widget ------------------ */
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
   CheckWithBtn* termsConditionsWidget() const;
   PwdRulesWidget* passwordValidatorWidget() const;
   
   private:
   // Theme Mode Flag
   bool isDarkMode = false;

   // Heading
   QLabel *heading = nullptr;

   // Name Widget
   CustomTextField *name = nullptr;
   QWidget *nameWidget = nullptr;

   // Password Widget
   CustomTextField *password = nullptr;
   QWidget *passwordWidget = nullptr;

   // Username Widget
   CustomTextField *username = nullptr;
   QWidget *usernameWidget = nullptr;

   // Email Widget
   CustomTextField *email = nullptr;
   QWidget *emailWidget = nullptr;

   // Holds all labels inside fields widgets
   QVector<QLabel *> labels;

   // Hold all field widgets 
   QVector<QWidget *> fieldsWidgets;

   // Layout
   QVBoxLayout *layout = nullptr;

   // Password Rules Validator
   PwdRulesWidget *_passwordValidatorWidget = nullptr;

   // Terms & Conditions
   CheckWithBtn *_termsConditionsWidget = nullptr;

   Dialog *termsConditionsDialog = nullptr;
   TermsConditions *termsConditionsDialogWidget = nullptr;

   // Create Account Button
   Button *createAccBtn = nullptr;

   // Helper Functions
   CustomTextField *createTextField(const QString &placeholderText = "", bool useCheck = false);
   QWidget *createLabeledTextFieldWidget(const QString labelName = "", CustomTextField *currField = nullptr);
   QFont font(const QString &family = "Segoe UI", int fontSize = 10, QFont::Weight weight = QFont::Medium);
   void addLabelsInsideFieldsWidgets();

   signals:
   void onNameEntered();
   void onUsernameEntered();
   void onEmailEntered();
   void onPasswordEntered();
};
