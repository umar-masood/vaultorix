#include "UserMenu.h"

UserMenu::UserMenu(QWidget *parent) : RoundedBox(parent) {
    setFixedSize(QSize(280, 362));
    hide();

    // Smooth Opacity
    smooth_opacity = new SmoothOpacity;
    setGraphicsEffect(smooth_opacity);

    // Opacity Animation
    animation = new QPropertyAnimation(smooth_opacity, "opacity");
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setDuration(300);

    // Installing Event Filter for auto closing on outside click
    qApp->installEventFilter(this);

    // Option Buttons
    // Account Settings
    account_settings_btn = createButton("Account Settings", IconManager::icon(Icons::AccountSettings));

    // Manage Subscription
    manage_subscription_btn = createButton("Manage Subscription", IconManager::icon(Icons::Subscription));

    // Preferences
    preferences_btn = createButton("Preferences", IconManager::icon(Icons::Preferences));

    // Bug Report
    report_bug_btn = createButton("Report a Bug", IconManager::icon(Icons::Bug));

    // About us
    about = createButton("About Vaultorix", IconManager::icon(Icons::About));

    option_buttons = {account_settings_btn, manage_subscription_btn, preferences_btn, report_bug_btn, about};

    // Layout
    layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addSpacing(164); // Reserved for painting
    layout->addStretch();
    layout->addWidget(account_settings_btn, 0, Qt::AlignHCenter);
    layout->addWidget(manage_subscription_btn, 0, Qt::AlignHCenter);
    layout->addWidget(preferences_btn, 0, Qt::AlignHCenter);
    layout->addWidget(report_bug_btn, 0, Qt::AlignHCenter);
    layout->addWidget(about, 0, Qt::AlignHCenter);
    layout->addStretch();

    // Initial Theme
    setDarkMode(isDarkMode);

    // Signal Slot
    for (auto *btn : option_buttons)
        connect(btn, &Button::clicked, this, &UserMenu::fadeOut);
}

Button* UserMenu::accountSettingsButton() const { return account_settings_btn; }
Button* UserMenu::manageSubscriptionButton() const { return manage_subscription_btn; }
Button* UserMenu::preferencesButton() const { return preferences_btn; }
Button* UserMenu::reportBugButton() const { return report_bug_btn; }
Button* UserMenu::aboutButton() const { return about; }

void UserMenu::fadeIn() {
    animation->stop();
    disconnect(animation, &QPropertyAnimation::finished, nullptr, nullptr);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start();
    show();
}

void UserMenu::fadeOut() {
    animation->stop();
    disconnect(animation, &QPropertyAnimation::finished, nullptr, nullptr);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    connect(animation, &QPropertyAnimation::finished, this, &QWidget::hide);
    animation->start();
}

bool UserMenu::eventFilter(QObject *o, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QWidget *clickedWidget = QApplication::widgetAt(mouseEvent->globalPosition().toPoint());

        if (this->isVisible() && this->isAncestorOf(clickedWidget)) 
            fadeOut();
    }

    if (event->type() == QEvent::ApplicationDeactivate)
        if (this->isVisible())  
            fadeOut();

    return QObject::eventFilter(o, event);
}

void UserMenu::setDarkMode(bool enable) {
    if (isDarkMode == enable) 
        return;

    isDarkMode = enable;

    for (auto *btn : option_buttons) 
        btn->setDarkMode(isDarkMode);

    RoundedBox::setDarkMode(isDarkMode);
}

Button* UserMenu::createButton(const QString &text, const QString &iconPath) {
    auto *btn = new Button;
    btn->setDisplayMode(Button::IconText);
    btn->setFixedSize(QSize(262, 36));
    btn->setText(text);
    btn->setFontProperties("Segoe UI", 10);
    btn->setSecondary(true);
    btn->setNormalBackgroundTransparent(true);
    btn->setBorderTransparent(true);
    btn->setIconSize(QSize(18, 18));
    btn->setIconPaths(iconPath, iconPath);
    btn->setFontXY(0, -1);
    return btn;
}

void UserMenu::paintEvent(QPaintEvent *event) {
    RoundedBox::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

    // User Profile Rect
    const int padding = 12;
    const int spacing = 8;

    int X = padding;
    int Y = padding;
    int W = width() - 2 * padding;
    int H = 110;

    QRect userProfileRect(X, Y, W, H);

    // Avator
    int avatorX = userProfileRect.x() + (userProfileRect.width() - _avator.width()) / 2;
    int avatorY = userProfileRect.y() + 2; // 2 : for adjustment due to base class
    painter.drawPixmap(avatorX, avatorY, 
                    (_avator.isNull()) ? (IconManager::icon(Icons::Avator)) :  _avator);

    // Name Text Color
    painter.setPen(isDarkMode ? Qt::white : Qt::black);
    
    // Name
    QFont fnt;
    fnt.setFamily("Segoe UI");
    fnt.setPixelSize(16);
    fnt.setWeight(QFont::DemiBold);
    painter.setFont(fnt);

    // Font Metrics
    QFontMetrics nameFm(fnt);
    int nameTxtH = nameFm.height();
    int nameTxtY = userProfileRect.y() + _avator.height() + spacing;

    // Elided
    QString elidedName = nameFm.elidedText(_name, Qt::ElideRight, userProfileRect.width());

    // Rect
    QRect nameTxtRect(userProfileRect.x(), nameTxtY , userProfileRect.width(), nameTxtH);
    painter.drawText(nameTxtRect, Qt::AlignHCenter, elidedName);

    // Email
    fnt.setPixelSize(14);
    fnt.setWeight(QFont::Normal);
    painter.setFont(fnt);

    // Email Text Color
    painter.setPen(isDarkMode ? "#757575" : "#ACABAB");

    // Font Metrics
    QFontMetrics emailFm(fnt);
    int emailTxtH = emailFm.height();
    QString elidedEmail = emailFm.elidedText(_email, Qt::ElideRight, userProfileRect.width());

    QRect emailTxtRect(userProfileRect.x(), nameTxtRect.bottom(), userProfileRect.width(), emailTxtH);
    painter.drawText(emailTxtRect, Qt::AlignHCenter, elidedEmail);

    // Seperator;
    int sepX1 = 2;
    int sepX2 = width();
    int sepY1 = emailTxtRect.bottom() + spacing;
    int sepY2 = sepY1;

    // Seperator Color
    QPen pen;
    pen.setColor("#AFAFAF");
    pen.setWidthF(0.2);
    painter.setPen(pen);
    
    painter.drawLine(sepX1, sepY1, sepX2, sepY2);
};

void UserMenu::setAvator(const QPixmap &avator) {
    _avator = avator;
    update();
}

void UserMenu::setName(const QString &name) { 
  _name = name;
  update();
}

void UserMenu::setEmail(const QString &email) { 
  _email = email;
  update();
}