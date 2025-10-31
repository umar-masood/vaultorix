#include "AccountCreate.h"

// CustomTextField Implementation
CustomTextField::CustomTextField(bool useCheck, QWidget *parent) : TextField(parent)
{
   if (useCheck)
   {
      setSpacingRight(true);
      checkIcon = new QLabel(this);
      checkIcon->setAttribute(Qt::WA_TranslucentBackground);
      checkIcon->setFixedSize(QSize(20, 20));

      QTimer::singleShot(0, this, [this]()
      {
         int x = width() - (checkIcon->width() + 12);
         int y = (height() - checkIcon->height()) / 2;
         checkIcon->move(x, y); 
      });

      setUnchecked();
   }
}

void CustomTextField::setChecked()
{
   if (checkIcon)
      checkIcon->setPixmap(QPixmap(checked).scaled(QSize(20, 20), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void CustomTextField::setUnchecked()
{
   if (checkIcon)
      checkIcon->setPixmap(QPixmap(unchecked).scaled(QSize(20, 20), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

// Terms & Conditions Widget
CheckWithBtn::CheckWithBtn(QWidget *parent) : QWidget(parent)
{
   setAttribute(Qt::WA_TranslucentBackground);

   // Create checkbox
   checkbox = new CheckBox("I agree to our");
   checkbox->setParent(this);
   checkbox->move(0, 0); 

   // Create button
   _button = new Button;
   _button->setParent(this);
   _button->setDisplayMode(Button::TextOnly);
   _button->setSize(QSize(160, 12));
   _button->setHyperLink(true);
   _button->setText("Terms & Conditions");
   _button->setFontProperties("Segoe UI", 10, false, false);
   _button->setHyperLinkColors("#008EDE", "#15F2FF");
   _button->move(checkbox->width() + 4, 4); 

   connect(_button, &Button::clicked, this, [this](){
      emit onButtonClicked();
   });

   connect(checkbox, &CheckBox::toggled, this, [this](bool checked){
      emit onBoxChecked(checked);
   });

   connect(this, &CheckWithBtn::themeModeChanged, this, [this](bool enable){
      if (checkbox) checkbox->setDarkMode(enable); 
   });
   
   setFixedSize(QSize((checkbox->width() + _button->width() + 5), 22));
}

void CheckWithBtn::setDarkMode(bool value) {
   if (isDarkMode == value) return;
   isDarkMode = value;
   emit themeModeChanged(isDarkMode);
}

Button* CheckWithBtn::button() const {
   return _button;
}

CheckBox* CheckWithBtn::termsCheckBox() const {
   return checkbox;
}

// AccountCreate Implementation
AccountCreate::AccountCreate(QWidget *parent) : QWidget(parent)
{
   setAttribute(Qt::WA_TranslucentBackground);

   // Heading
   heading = new QLabel("Create an Account");
   heading->setAttribute(Qt::WA_TranslucentBackground);
   heading->setFixedSize(QSize(276, 36));
   heading->setAlignment(Qt::AlignCenter);
   heading->setFont(font("Inter", 22, QFont::Bold));

   // Name
   NAME = Field("Enter your name");
   QRegularExpression rex("[A-Za-z ]+");
   NAME->setValidator(new QRegularExpressionValidator(rex, this));
   NAME->setClearButton(true);
   _NAME = LabeledField("Name", NAME);

   // Username
   USERNAME = Field("Enter unique username", true);
   _USERNAME = LabeledField("Username", USERNAME);

   // Email
   EMAIL = Field("Enter your email-address", true);
   EMAIL->setContextMenu(false);
   _EMAIL = LabeledField("Email-Address", EMAIL);

   // Password
   PWD = Field("Enter strong password");
   PWD->setContextMenu(false);
   PWD->setPasswordTextField(true);
   _PWD = LabeledField("Password", PWD);

   fieldsWidgets = {_NAME, _USERNAME, _EMAIL, _PWD};

   // Validator
   pwdValidator = new PwdRulesWidget;

   // Agreement
   agreement = new CheckWithBtn;

   // Layout
   layout = new QVBoxLayout;
   layout->addWidget(heading, 0, Qt::AlignHCenter);
   layout->addSpacing(20);

   for (auto *widget : fieldsWidgets) {
      layout->addWidget(widget, 0 , Qt::AlignHCenter);
      layout->addSpacing(10);
   }

   layout->addWidget(pwdValidator, 0, Qt::AlignLeft);
   layout->addSpacing(10);
   layout->addWidget(agreement, 0, Qt::AlignLeft);

   createAccBtn = new Button;
   createAccBtn->setDisplayMode(Button::TextOnly);
   createAccBtn->setSize(QSize(360, 36));
   createAccBtn->setGradientColor(true, "#008EDE", "#15F2FF");
   createAccBtn->setHoverGradientColor("#008EDE");
   createAccBtn->setText("Create Account");
   createAccBtn->setFontProperties("Segoe UI", 11, true, false);

   layout->addSpacing(20);
   layout->addWidget(createAccBtn, 0, Qt::AlignLeft);
   layout->addStretch();

   setLayout(layout);
   setDarkMode(isDarkMode);
}

void AccountCreate::setDarkMode(bool value)
{
   isDarkMode = value;

   QVector<QLabel *> labels;
   for (auto *widget : fieldsWidgets)
   {
      QLabel *label = widget->findChild<QLabel *>();
      if (label)
         labels.append(label);
   }

   labels.append(heading);

   QString labelColor = isDarkMode ? "white" : "black";
   for (auto *label : labels)
      label->setStyleSheet(QString("color: %1;").arg(labelColor));

   QVector<CustomTextField *> textFields = {NAME, USERNAME, PWD, EMAIL};
   for (auto *field : textFields)
      field->setDarkMode(isDarkMode);

   if (agreement) agreement->setDarkMode(isDarkMode);
}

Button* AccountCreate::createBtn() const {
   return createAccBtn;
}

CustomTextField *AccountCreate::nameField() const
{
   return NAME;
}

CustomTextField *AccountCreate::usernameField() const
{
   return USERNAME;
}

CustomTextField *AccountCreate::pwdField() const
{
   return PWD;
}

CustomTextField *AccountCreate::emailField() const
{
   return EMAIL;
}

CheckWithBtn *AccountCreate::termsCondsBtn() const
{
   return agreement;
}

CustomTextField *AccountCreate::Field(const QString &placeholderText, bool useCheck)
{
   auto *field = new CustomTextField(useCheck);
   field->setPlaceholderText(placeholderText);
   field->setSize(QSize(360, 36));
   return field;
}

QWidget *AccountCreate::LabeledField(const QString labelName, CustomTextField *currField)
{
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

PwdRulesWidget * AccountCreate::pwdRulesWidget() const {
   return pwdValidator;
}