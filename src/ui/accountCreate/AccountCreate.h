#pragma once
#include "../components/TextField.h" 
#include "../pwdValidator/pwdValidator.h"
#include "../components/CheckBox.h"
#include "../components/Button.h"

class CustomTextField : public TextField
{
   Q_OBJECT

public:
   explicit CustomTextField(bool useCheck = false, QWidget *parent = nullptr);

   void setChecked();
   void setUnchecked();

private:
   QLabel *checkIcon = nullptr;
   const QString checked = ":/icons/AccountCreate/checked.svg";
   const QString unchecked = ":/icons/AccountCreate/unchecked.svg";
};

class CheckWithBtn : public QWidget
{
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
   void onBoxChecked(bool checked);
};

class AccountCreate : public QWidget
{
   Q_OBJECT

public:
   explicit AccountCreate(QWidget *parent = nullptr);
   Q_INVOKABLE void setDarkMode(bool value);

   CustomTextField* nameField() const;
   CustomTextField* usernameField() const;
   CustomTextField* pwdField() const;
   CustomTextField* emailField() const;
   CheckWithBtn* termsCondsBtn() const;
   
private:
   bool isDarkMode = false;

   QLabel *heading = nullptr;

   CustomTextField *NAME = nullptr;
   QWidget *_NAME = nullptr;
   CustomTextField *PWD = nullptr;
   QWidget *_PWD = nullptr;

   CustomTextField *USERNAME = nullptr;
   QWidget *_USERNAME = nullptr;
   CustomTextField *EMAIL = nullptr;
   QWidget *_EMAIL = nullptr;
   QVector<QWidget *> fieldsWidgets;

   QVBoxLayout *layout = nullptr;

   CustomTextField *Field(const QString &placeholderText = "", bool useCheck = false);
   QWidget *LabeledField(const QString labelName = "", CustomTextField *currField = nullptr);
   QFont font(const QString &family = "Segoe UI", int fontSize = 10, QFont::Weight weight = QFont::Medium);

   PwdValidator *pwdValidator = nullptr;
   CheckWithBtn *agreement = nullptr;
   Button *createAccBtn = nullptr;

   signals:
   void onNameEntered();
   void onUsernameEntered();
   void onEmailEntered();
   void onPasswordEntered();
};
