#include "AccountCreate.h"

/* ========================================================================================= 
                              ACCOUNT CREATE IMPLEMENTATION              
   ========================================================================================= */
AccountCreate::AccountCreate(QWidget *parent, AccountWindow *accountWindow) : QWidget(parent) {
   setAttribute(Qt::WA_TranslucentBackground);
   setFocusPolicy(Qt::StrongFocus);

   // Heading  
   heading = new QLabel("Create an Account");
   heading->setAttribute(Qt::WA_TranslucentBackground);
   heading->setFixedSize(QSize(276, 36));
   heading->setAlignment(Qt::AlignCenter);
   heading->setFont(font("Inter", 22, QFont::Bold));
   labels.append(heading);

   // Name
   name = createTextField("Enter your full name", true);
   name->setContextMenu(true);
   nameLayout = createLabeledTextFieldLayout("Full Name", name);

   // Username
   username = createTextField("Enter unique username", true);
   username->setContextMenu(false);
   usernameLayout = createLabeledTextFieldLayout("Username", username);

   // Email
   email = createTextField("Enter your email-address", true);
   email->setContextMenu(false);
   emailLayout = createLabeledTextFieldLayout("Email-Address", email);

   // Password
   password = createTextField("Enter strong password");
   password->setContextMenu(false);
   password->setPasswordTextField(true);
   passwordLayout = createLabeledTextFieldLayout("Password", password);

   // Adding all field layouts to a vector
   fieldsLayouts = {nameLayout, usernameLayout, emailLayout, passwordLayout};

   // Validator
   _passwordValidatorWidget = new PwdRulesWidget;

   // T&Cs Consent Widget
   _termsConditionsWidget = new CheckWithBtn;

   // Terms & Conditions Popup
   termsConditionsDialogWidget = new TermsConditions;
   termsConditionsDialog = new Dialog(termsConditionsDialogWidget, accountWindow, true);
   accountWindow->setSubWidgets({termsConditionsDialogWidget, termsConditionsDialog});
   // Signal Slot of T&Cs Dialog
   connect(_termsConditionsWidget, &CheckWithBtn::onButtonClicked, this, [=]() { termsConditionsDialog->show(); });

   // Layout
   layout = new QVBoxLayout;
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
   createAccBtn = new Button;
   createAccBtn->setCursor(Qt::PointingHandCursor);
   createAccBtn->setDisplayMode(Button::TextOnly);
   createAccBtn->setFixedSize(QSize(360, 36));
   createAccBtn->setLoaderButton(true);
   createAccBtn->setGradientColors("#008EDE", "#15F2FF", "#008EDE");
   createAccBtn->setText("Create Account");
   createAccBtn->setFontProperties("Segoe UI", 11, QFont::DemiBold);
   createAccBtn->setEnabled(false);

   // Redirect to Sign In page widget
   _redirectToSignInWidget = new TextWithBtn("Already have an account?", QSize(150, 22), "Sign In", QSize(46,20), false);
   _redirectToSignInWidget->button()->move(_redirectToSignInWidget->text()->width() + 4, -2); // Used manual movement to proper align it with prompt text of this widget.

   layout->addSpacing(18);
   layout->addWidget(createAccBtn, 0, Qt::AlignLeft);
   layout->addSpacing(16);
   layout->addWidget(_redirectToSignInWidget, 0, Qt::AlignCenter);
   layout->addSpacing(10);
   layout->addStretch();

   // Setting layout on widget
   setLayout(layout);

   // Initial theme
   setDarkMode(isDarkMode);
}

/* --------------------  Setters  -----------------  */
void AccountCreate::setDarkMode(bool value) {
   isDarkMode = value;

   // Labels (Email, Username, Password show above each TextField) inside field widgets
   for (auto *label : labels)
      label->setStyleSheet(QString("color: %1;").arg(isDarkMode ? "white" : "black"));

   // TextFields inside fields layouts such as emailLayout, usernameLayout etc.
   for (auto *field : {name, username, password, email})
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
Button* AccountCreate::createAccountButton() const { return createAccBtn; }
CustomTextField *AccountCreate::nameField() const { return name; }
CustomTextField *AccountCreate::usernameField() const { return username; }
CustomTextField *AccountCreate::passwordField() const { return password; }
CustomTextField *AccountCreate::emailField() const { return email; }
TextWithBtn * AccountCreate::redirectToSignInWidget() const { return _redirectToSignInWidget; }
/**
 * @return A widget contains a checkbox and hyperlink button to the terms and conditions dialog box
 */
CheckWithBtn *AccountCreate::termsConditionsWidget() const { return _termsConditionsWidget; }
/**
 * @return Password rules widget such as it must have eight digits, one uppercase and one lowercase letter etc.
 */
PwdRulesWidget * AccountCreate::passwordValidatorWidget() const { return _passwordValidatorWidget; }

/* --------------------  Helpers  -----------------  */
CustomTextField *AccountCreate::createTextField(const QString &placeholderText, bool hasValidity) {
   auto *field = new CustomTextField(hasValidity);
   field->setPlaceholderText(placeholderText);
   field->setFixedSize(QSize(360, 36));
   field->setDarkMode(isDarkMode);
   return field;
}

QVBoxLayout *AccountCreate::createLabeledTextFieldLayout(const QString labelName, CustomTextField *currField) {
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

QFont AccountCreate::font(const QString &family , int fontSize, QFont::Weight weight) {
   QFont font;
   font.setFamily(family);
   font.setPointSize(fontSize);
   font.setWeight(weight);
   return font;
}


/* ========================================================= 
             CUSTOMIZED WIDGETS FOR ABOVE CLASS               
   ========================================================= */

/* ------------------  CUSTOM TEXT FIELD ------------------- */
CustomTextField::CustomTextField(bool hasValidity, QWidget *parent) : TextField(parent) {
   if (hasValidity) {

      // Check Icon
      validityIcon = new QLabel(this);
      validityIcon->setAttribute(Qt::WA_TranslucentBackground);
      validityIcon->setFixedSize(QSize(20, 20));
      
      setPadding(0, 0, validityIcon->width() + 24);

      QTimer::singleShot(0, this, [this]() {
         int x = width() - (validityIcon->width() + 12);
         int y = (height() - validityIcon->height()) / 2;
         validityIcon->move(x, y); 
      });

      // ToolTip showing over check icon inside textfield
      tooltip = new ToolTip;

      // Initially set invalid
      setInvalid();
   }
}

/* --------------------  Setters  -----------------  */
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
      tooltip->setTargetWidget(validityIcon);
      tooltip->setText(tooltipText);
   }

   hasTip = true;
}


void CustomTextField::setValid() {
   if (validityIcon) 
      validityIcon->setPixmap(QPixmap(IconManager::icon(Icons::Valid)).scaled(QSize(20, 20), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void CustomTextField::setInvalid() {
   if (validityIcon) 
      validityIcon->setPixmap(QPixmap(IconManager::icon(Icons::Invalid)).scaled(QSize(20, 20), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void CustomTextField::setDarkMode(bool value) {
   TextField::setDarkMode(value);
   if (tooltip)
      tooltip->setDarkMode(value);
}

/* ------------------  CHECKBOX WITH HYPERLINK BUTTON CUSTOM WIDGET ------------------- */
CheckWithBtn::CheckWithBtn(QWidget *parent) : QWidget(parent) {
   setAttribute(Qt::WA_TranslucentBackground);

   // Checkbox
   _checkbox = new CheckBox("I agree to our");
   _checkbox->setParent(this);
   _checkbox->move(0, 0); 

   // Button
   _button = new Button;
   _button->setCursor(Qt::PointingHandCursor);
   _button->setParent(this);
   _button->setDisplayMode(Button::TextOnly);
   _button->setFixedSize(QSize(160, 12));
   _button->setText("Terms & Conditions");
   _button->setFontProperties("Segoe UI", 10);
   _button->setHyperLink(true);
   _button->move(_checkbox->width() + 4, 4); 

   // Signals Slots
   connect(_button, &Button::clicked, this, [this]() { emit onButtonClicked(); });
   connect(_checkbox, &CheckBox::toggled, this, [this](bool checked) { emit boxChecked(checked); });
   
   // Setting fixed size
   setFixedSize(QSize((_checkbox->width() + _button->width() + 5), 22));   
}

/* --------------------  Setters  -----------------  */
void CheckWithBtn::setDarkMode(bool value) {
   if (isDarkMode == value) 
      return;
   
   isDarkMode = value;

   _checkbox->setDarkMode(isDarkMode); 
}

/* --------------------  Getters  -----------------  */
Button* CheckWithBtn::button() const { return _button; }
CheckBox* CheckWithBtn::checkBox() const { return _checkbox; }
