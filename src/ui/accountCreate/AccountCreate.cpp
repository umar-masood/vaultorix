#include "AccountCreate.h"

// CustomTextField Implementation
CustomTextField::CustomTextField(bool useCheck, QWidget *parent) : TextField(parent) {
   if (useCheck) {
      setSpacingRight(true);
      checkIcon = new QLabel(this);
      checkIcon->setAttribute(Qt::WA_TranslucentBackground);
      checkIcon->setFixedSize(QSize(20, 20));
      
      QTimer::singleShot(0, this, [this]() {
         int x = width() - (checkIcon->width() + 12);
         int y = (height() - checkIcon->height()) / 2;
         checkIcon->move(x, y); 
      });

      tooltip = new ToolTip;

      setUnchecked();
   }
}

void CustomTextField::setTooltip(const QString &tooltipText) {
   if (tooltipText.isEmpty()) {
      if (tooltip) {
         tooltip->hide();
         tooltip->setTargetWidget(nullptr);
      }
      hasTip = false;
      return;
   }

   if (tooltip) {
      tooltip->setTargetWidget(checkIcon);
      tooltip->setText(tooltipText);
   }

   hasTip = true;
}


void CustomTextField::setChecked() {
   if (checkIcon)
      checkIcon->setPixmap(QPixmap(checked).scaled(QSize(20, 20), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void CustomTextField::setUnchecked() {
   if (checkIcon)
      checkIcon->setPixmap(QPixmap(unchecked).scaled(QSize(20, 20), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void CustomTextField::setDarkMode(bool value) {
   TextField::setDarkMode(value);
   if (tooltip)
      tooltip->setDarkMode(value);
}

// Terms & Conditions Widget
CheckWithBtn::CheckWithBtn(QWidget *parent) : QWidget(parent) {
   setAttribute(Qt::WA_TranslucentBackground);
   setFocusPolicy(Qt::StrongFocus);

   // Create checkbox
   _checkbox = new CheckBox("I agree to our");
   _checkbox->setParent(this);
   _checkbox->move(0, 0); 

   // Create button
   _button = new Button;
   _button->setParent(this);
   _button->setDisplayMode(Button::TextOnly);
   _button->setFixedSize(QSize(160, 12));
   _button->setHyperLink(true);
   _button->setText("Terms & Conditions");
   _button->setFontProperties("Segoe UI", 10, false, false);
   _button->setHyperLinkColors("#008EDE", "#15F2FF");
   _button->move(_checkbox->width() + 4, 4); 

   connect(_button, &Button::clicked, this, [this]() { emit onButtonClicked(); });
   connect(_checkbox, &CheckBox::toggled, this, [this](bool checked) { emit boxChecked(checked); });
   connect(this, &CheckWithBtn::themeModeChanged, this, [this](bool enable) { if (_checkbox) _checkbox->setDarkMode(enable); });
   
   setFixedSize(QSize((_checkbox->width() + _button->width() + 5), 22));
}

void CheckWithBtn::setDarkMode(bool value) {
   if (isDarkMode == value) return;
   isDarkMode = value;
   emit themeModeChanged(isDarkMode);
}

Button* CheckWithBtn::button() const { return _button; }
CheckBox* CheckWithBtn::checkBox() const { return _checkbox; }

// AccountCreate Implementation
AccountCreate::AccountCreate(QWidget *parent, AccountWindow *accountWindow) : QWidget(parent) {
   setAttribute(Qt::WA_TranslucentBackground);

   // Heading
   heading = new QLabel("Create an Account");
   heading->setAttribute(Qt::WA_TranslucentBackground);
   heading->setFixedSize(QSize(276, 36));
   heading->setAlignment(Qt::AlignCenter);
   heading->setFont(font("Inter", 22, QFont::Bold));

   // Name
   name = Field("Enter your full name", true);
   name->setContextMenu(true);
   nameWidget = LabeledField("Full Name", name);

   // Username
   username = Field("Enter unique username", true);
   username->setContextMenu(false);
   usernameWidget = LabeledField("Username", username);

   // Email
   email = Field("Enter your email-address", true);
   email->setContextMenu(false);
   emailWidget = LabeledField("Email-Address", email);

   // Password
   password = Field("Enter strong password");
   password->setContextMenu(false);
   password->setPasswordTextField(true);
   passwordWidget = LabeledField("Password", password);

   fieldsWidgets = {nameWidget, usernameWidget, emailWidget, passwordWidget};

   // Validator
   _passwordValidatorWidget = new PwdRulesWidget;

   // Agreement
   _termsConditionsWidget = new CheckWithBtn;

   // Terms & Conditions Popup
   termsConditionsDialogWidget = new TermsConditions;
   termsConditionsDialog = new Dialog(termsConditionsDialogWidget, accountWindow, true);
   accountWindow->setSubWidgets({termsConditionsDialogWidget, termsConditionsDialog});
   
   connect(_termsConditionsWidget, &CheckWithBtn::onButtonClicked, this, [=]() { termsConditionsDialog->show(); });

   // Layout
   layout = new QVBoxLayout;
   layout->addWidget(heading, 0, Qt::AlignHCenter);
   layout->addSpacing(20);

   for (auto *widget : fieldsWidgets) {
      layout->addWidget(widget, 0 , Qt::AlignHCenter);
      layout->addSpacing(10);
   }

   layout->addWidget(_passwordValidatorWidget, 0, Qt::AlignLeft);
   layout->addSpacing(10);
   layout->addWidget(_termsConditionsWidget, 0, Qt::AlignLeft);

   // Create Account Button
   createAccBtn = new Button;
   createAccBtn->setDisplayMode(Button::TextOnly);
   createAccBtn->setFixedSize(QSize(360, 36));
   createAccBtn->setLoaderButton(true);
   createAccBtn->setGradientColor(true, "#008EDE", "#15F2FF");
   createAccBtn->setHoverGradientColor("#008EDE");
   createAccBtn->setText("Create Account");
   createAccBtn->setFontProperties("Segoe UI", 11, true, false);
   createAccBtn->setEnabled(false);

   layout->addSpacing(20);
   layout->addWidget(createAccBtn, 0, Qt::AlignLeft);
   layout->addStretch();

   setLayout(layout);
   setDarkMode(isDarkMode);
}

void AccountCreate::setDarkMode(bool value) {
   isDarkMode = value;

   QVector<QLabel *> labels;
   for (auto *widget : fieldsWidgets) {
      QLabel *label = widget->findChild<QLabel *>();
      if (label)
         labels.append(label);
   }

   labels.append(heading);

   QString labelColor = isDarkMode ? "white" : "black";
   for (auto *label : labels)
      label->setStyleSheet(QString("color: %1;").arg(labelColor));

   QVector<CustomTextField *> textFields = {name, username, password, email};
   for (auto *field : textFields)
      field->setDarkMode(isDarkMode);

   if (_termsConditionsWidget) _termsConditionsWidget->setDarkMode(isDarkMode);
}

Button* AccountCreate::createAccountButton() const { return createAccBtn; }

CustomTextField *AccountCreate::nameField() const { return name; }
CustomTextField *AccountCreate::usernameField() const { return username; }
CustomTextField *AccountCreate::passwordField() const { return password; }
CustomTextField *AccountCreate::emailField() const { return email; }

CheckWithBtn *AccountCreate::termsConditionsWidget() const { return _termsConditionsWidget; }
PwdRulesWidget * AccountCreate::passwordValidatorWidget() const { return _passwordValidatorWidget; }

CustomTextField *AccountCreate::Field(const QString &placeholderText, bool useCheck) {
   auto *field = new CustomTextField(useCheck);
   field->setPlaceholderText(placeholderText);
   field->setFixedSize(QSize(360, 36));
   return field;
}

QWidget *AccountCreate::LabeledField(const QString labelName, CustomTextField *currField) {
   auto *widget = new QWidget;
   widget->setFixedSize(currField->width(), 60);
   widget->setAttribute(Qt::WA_TranslucentBackground);

   auto *label = new QLabel;
   label->setAttribute(Qt::WA_TranslucentBackground);
   label->setText(labelName);
   label->setFont(font());

   auto *layout = new QVBoxLayout(widget);
   layout->setSpacing(0);
   layout->setContentsMargins(0, 0, 0, 0);
   layout->addWidget(label, 0, Qt::AlignLeft);
   layout->addSpacing(6);
   layout->addWidget(currField, 0, Qt::AlignLeft);
   layout->addStretch();

   return widget;
}

QFont AccountCreate::font(const QString &family , int fontSize, QFont::Weight weight) {
   QFont font;
   font.setFamily(family);
   font.setPointSize(fontSize);
   font.setWeight(weight);
   return font;
}