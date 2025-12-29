#pragma once
#include "../components/TextField.h" 
#include "../pwdRulesWidget/pwdRulesWidget.h"
#include "../components/CheckBox.h"
#include "../components/Button.h"
#include "../components/ToolTip.h"
#include "../dialogs/termsConditions/TermsConditions.h"
#include "../components/Dialog.h"
#include "../accountWindow/AccountWindow.h"
#include <QLabel>

class CustomTextField : public TextField {
   Q_OBJECT

   public:
   explicit CustomTextField(bool useCheck = false, QWidget *parent = nullptr);

   void setChecked();
   void setUnchecked();
   void setDarkMode(bool value) override;
   void setTooltip(const QString &tooltipText);

   private:
   QLabel *checkIcon = nullptr;
   ToolTip *tooltip = nullptr;
   bool hasTip = false;
   
   const QString checked = ":/icons/AccountCreate/checked.svg";
   const QString unchecked = ":/icons/AccountCreate/unchecked.svg";
};

class CheckWithBtn : public QWidget {
   Q_OBJECT

   public:
   explicit CheckWithBtn(QWidget *parent = nullptr);
   void setDarkMode(bool value);
   Button* button() const;
   CheckBox* termsCheckBox() const;

   private:
   bool isDarkMode = false;
   CheckBox *checkbox = nullptr;
   Button *_button = nullptr;

   signals:
   void themeModeChanged(bool enable);
   void onButtonClicked();
   void boxChecked(bool checked);
};

class AccountCreate : public QWidget {
   Q_OBJECT

   public:
   explicit AccountCreate(QWidget *parent = nullptr, AccountWindow *accountWindow = nullptr);
   Q_INVOKABLE void setDarkMode(bool value);

   Button* createBtn() const;
   CustomTextField* nameField() const;
   CustomTextField* usernameField() const;
   CustomTextField* pwdField() const;
   CustomTextField* emailField() const;
   CheckWithBtn* termsCondsBtn() const;
   PwdRulesWidget *pwdRulesWidget() const;
   
   private:
   bool isDarkMode = false;

   // Heading
   QLabel *heading = nullptr;

   // Name Widget
   CustomTextField *name = nullptr;
   QWidget *nameWidget = nullptr;

   // Password Widget
   CustomTextField *pwd = nullptr;
   QWidget *pwdWidget = nullptr;

   // Username Widget
   CustomTextField *username = nullptr;
   QWidget *usernameWidget = nullptr;

   // Email Widget
   CustomTextField *email = nullptr;
   QWidget *emailWidget = nullptr;

   // To hold all field widgets for easy access
   QVector<QWidget *> fieldsWidgets;

   // Layout
   QVBoxLayout *layout = nullptr;

   // Password Rules Validator
   PwdRulesWidget *pwdValidator = nullptr;

   // Terms & Conditions
   CheckWithBtn *agreement = nullptr;
   Dialog *termsDialog = nullptr;
   TermsConditions *termsConditionsWidget = nullptr;
      // Getters
      // Dialog *termsCondsDialog() const;
      // TermsConditions *termsCondsWidget() const;

   // Create Account Button
   Button *createAccBtn = nullptr;

   // Helper Functions
   CustomTextField *Field(const QString &placeholderText = "", bool useCheck = false);
   QWidget *LabeledField(const QString labelName = "", CustomTextField *currField = nullptr);
   QFont font(const QString &family = "Segoe UI", int fontSize = 10, QFont::Weight weight = QFont::Medium);

   signals:
   void onNameEntered();
   void onUsernameEntered();
   void onEmailEntered();
   void onPasswordEntered();
};
