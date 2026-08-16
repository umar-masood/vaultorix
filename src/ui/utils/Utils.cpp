#include "Utils.h"

namespace Ui::Utils {

    QPixmap cropToCircle(const QPixmap &pixmap, int size) {
        if (pixmap.isNull()) {
            qWarning() << "\nError: setAvator() :: Loading avator";
            return QPixmap();
        }

        // Getting minimum dimension
        int s = qMin(pixmap.width(), pixmap.height());

        // Define the Square
        QRect square((pixmap.width() - s) / 2, (pixmap.height() - s) / 2, s, s);

        // Extracting the defined square from whole image
        QPixmap image = pixmap.copy(square).scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Transparent Pixmap of 'size'
        QPixmap circle(size, size);
        circle.fill(Qt::transparent);

        // Painting on Transparent Pixmap
        QPainter painter(&circle);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

        // Defining circular path for clipping on Transparent Pixmap
        QPainterPath path;
        path.addRoundedRect(0, 0, size, size, size / 2, size / 2);

        // Clip circle on transparent pixmap
        painter.setClipPath(path);

        // Draw squared image inside this circle
        painter.drawPixmap(0, 0, image);

        return circle;
    }

    // ------------  PASSWORD RULES ITEMS WIDGET  ------------
    RuleItem::RuleItem(const QString &ruleTxt, QWidget *parent) : QWidget(parent) {
        setFixedHeight(22);
        setAttribute(Qt::WA_TranslucentBackground);

        text = ruleTxt;

        validIcon = IconManager::renderSvg(IconManager::icon(Icons::Valid), iconSize);
        invalidIcon = IconManager::renderSvg(IconManager::icon(Icons::Invalid), iconSize);

        font.setFamily("Segoe UI");
        font.setPointSize(10);
        font.setWeight(QFont::Normal);

        setInvalid();
    }

    void RuleItem::paintEvent(QPaintEvent *event) {
        QPainter painter(this);
        painter.setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform | QPainter::Antialiasing);

        // Icon
        const QPixmap &icon = validityState ? validIcon : invalidIcon;
        int iconY = (height() - iconSize.height()) / 2;
        painter.drawPixmap(0, iconY, iconSize.width(), iconSize.height(), icon);

        // Text
        painter.setFont(font);
        painter.setPen(QColor(validityState ? coloredText : uncoloredText));

        int textX = iconSize.width() + 8;
        painter.drawText(QRect(textX, 2, width() - textX, height()), 0, text);
    }

    void RuleItem::setValid() {
        validityState = true;
        update();
    }

    void RuleItem::setInvalid() {
        validityState = false;
        update();
    }

    /* --------------- Password Rules Widget ------------- */
    PasswordRules::PasswordRules(QWidget *parent) : QWidget(parent) {
        setAttribute(Qt::WA_TranslucentBackground);
        setFixedSize(QSize(300, 176));

        // Main Layout
        mainLayout = new QVBoxLayout(this);
        mainLayout->setSpacing(0);
        mainLayout->setContentsMargins(0, 0, 0, 0);

        // Rules Items
        _atLeastEightChars = new RuleItem(tr("At least 8 characters"));
        _atLeastOneLowerChar = new RuleItem(tr("At least one lowercase letter"));
        _atLeastOneUpperChar = new RuleItem(tr("At least one uppercase letter"));
        _atLeastOneDigit = new RuleItem(tr("At least one digit"));
        _atLeastOneSpecialChar = new RuleItem(tr("At least one special character"));
        _strongPassword = new RuleItem(tr("Strong Password"));

        // Adding into Layout
        mainLayout->addWidget(_strongPassword);
        mainLayout->addSpacing(8);
        mainLayout->addWidget(_atLeastEightChars);
        mainLayout->addSpacing(8);
        mainLayout->addWidget(_atLeastOneLowerChar);
        mainLayout->addSpacing(8);
        mainLayout->addWidget(_atLeastOneUpperChar);
        mainLayout->addSpacing(8);
        mainLayout->addWidget(_atLeastOneDigit);
        mainLayout->addSpacing(8);
        mainLayout->addWidget(_atLeastOneSpecialChar);
        mainLayout->addStretch();
    }

    RuleItem *PasswordRules::atLeastEight() const { return _atLeastEightChars; }
    RuleItem *PasswordRules::atLeastOneLowerCaseChar() const { return _atLeastOneLowerChar; }
    RuleItem *PasswordRules::atLeastOneUpperCaseChar() const { return _atLeastOneUpperChar; }
    RuleItem *PasswordRules::atLeastOneDigit() const { return _atLeastOneDigit; }
    RuleItem *PasswordRules::atLeastOneSpecialChar() const { return _atLeastOneSpecialChar; }
    RuleItem *PasswordRules::strongPassword() const { return _strongPassword; }

    /* ------------------  CUSTOM TEXT FIELD ------------------- */
    CustomTextField::CustomTextField(bool hasValidity, QWidget *parent) : TextField(parent) {
        if (hasValidity) {
            // Check Icon
            _validityIcon = new QLabel(this);
            _validityIcon->setAttribute(Qt::WA_TranslucentBackground);
            _validityIcon->setFixedSize(QSize(20, 20));

            setPadding(0, 0, _validityIcon->width() + 24);

            QTimer::singleShot(0, this, [this]() {
                int x = width() - (_validityIcon->width() + 12);
                int y = (height() - _validityIcon->height()) / 2;
                _validityIcon->move(x, y); 
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
            tooltip->setTargetWidget(_validityIcon);
            tooltip->setText(tooltipText);
        }

        hasTip = true;
    }

    void CustomTextField::setValid() {
        if (_validityIcon)
            _validityIcon->setPixmap(QPixmap(IconManager::icon(Icons::Valid)).scaled(QSize(20, 20), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    void CustomTextField::setInvalid() {
        if (_validityIcon)
            _validityIcon->setPixmap(QPixmap(IconManager::icon(Icons::Invalid)).scaled(QSize(20, 20), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    void CustomTextField::setDarkMode(bool value) {
        TextField::setDarkMode(value);
        if (tooltip)
            tooltip->setDarkMode(value);
    }

    QLabel *CustomTextField::validityIcon() const {
        return _validityIcon;
    }

    /* ------------------  CHECKBOX WITH HYPERLINK BUTTON CUSTOM WIDGET ------------------- */
    CheckWithBtn::CheckWithBtn(QWidget *parent) : QWidget(parent) {
        setAttribute(Qt::WA_TranslucentBackground);

        // Checkbox
        _checkbox = new CheckBox(tr("I agree to our"));
        _checkbox->setParent(this);
        _checkbox->move(0, 0);

        // Button
        _button = new Button;
        _button->setCursor(Qt::PointingHandCursor);
        _button->setParent(this);
        _button->setDisplayMode(Button::TextOnly);
        _button->setFixedSize(QSize(160, 12));
        _button->setText(tr("Terms & Conditions"));
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
    Button *CheckWithBtn::button() const { return _button; }
    CheckBox *CheckWithBtn::checkBox() const { return _checkbox; }

    
    /* ------------------------ Text with Button Widget ------------------------ */
    TextWithBtn::TextWithBtn(const QString &promptText,
                             const QSize &promptTextSize,
                             const QString &hyperlinkText,
                             const QSize &hyperlinkSize,
                             bool hasTimer,
                             QWidget *parent) : QWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);

        // Text
        _text = new Label("Segoe UI", 10, QFont::Normal, false, promptText, Qt::AlignHCenter);
        _text->setStyleSheet("color: black;");
        _text->setParent(this);
        _text->setFixedSize(promptTextSize);
        _text->move(0, 0);

        // Resend Button
        _button = new Button(this);
        _button->setDisplayMode(Button::TextOnly);
        _button->setFixedSize(hyperlinkSize);
        _button->setText(hyperlinkText);
        _button->setFontProperties("Segoe UI", 10);
        _button->setHyperLink(true);
        _button->move(_text->width() + 4, 2);

        // Signal Slot
        connect(_button, &Button::clicked, this, &TextWithBtn::buttonClicked);

        setFixedSize(QSize(_text->width() + _button->width() + 2, 18));

        // Timer Label
        if (hasTimer) {
            _timer = new Label("Segoe UI", 10, QFont::Normal, false, "00:00", Qt::AlignHCenter);
            _timer->setStyleSheet("color: black;");
            _timer->setParent(this);
            _timer->setFixedSize(36, 22);
            _timer->move(_text->width() + _button->width() + 4, 0);

            setFixedSize(QSize(_text->width() + _button->width() + _timer->width() + 2, 18));
        }
    }

    /* --------------------  Getters  -----------------  */
    Label *TextWithBtn::text() const { return _text; }
    Button *TextWithBtn::button() const { return _button; }
    Label *TextWithBtn::timer() const { return _timer; }

};