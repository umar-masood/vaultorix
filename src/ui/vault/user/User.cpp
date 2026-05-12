#include "User.h"

#include "../../../core/theme/ThemeManager.h"
#include "../../../core/session/SessionManager.h"

#include "../user_menu/UserMenu.h"
#include "../../utils/Utils.h"
#include "../../../../resources/IconManager.h"

#include <QFont>
#include <QPixmap>
#include <QRect>

using Ui::Vault::User;
User::User(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_Hover);
    _avator = Ui::Utils::cropToCircle(IconManager::icon(Icons::Avator), 36);        

    auto &sm = SessionManager::instance();

    userMenu = new Ui::Vault::UserMenu;
    userMenu->setName(sm.fullName());
    userMenu->setEmail(sm.email());
    userMenu->setAvator(QPixmap(IconManager::icon(Icons::Avator))
                        .scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    connect(&sm, &SessionManager::avatorUpdated, this, [this](){
        auto &sm = SessionManager::instance();
        if (!sm.avatar().isNull()) {
            _avator = Ui::Utils::cropToCircle(sm.avatar(), 36); 
            userMenu->setAvator(Ui::Utils::cropToCircle(sm.avatar(), 100));   
        } else {
            _avator = Ui::Utils::cropToCircle(IconManager::icon(Icons::Avator), 36);        
            userMenu->setAvator(QPixmap(IconManager::icon(Icons::Avator))
                                .scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        }
    });

    connect(&sm, &SessionManager::userInfoChanged, this, [this](){
        auto &sm = SessionManager::instance();
        userMenu->setName(sm.fullName());
        userMenu->setEmail(sm.email());
    });
   
    connect(this, &User::clicked, this, [this]() {
        if (userMenu)
            userMenu->showAt(this); 
    });

    // Installing Event Filter for auto closing on outside click
    qApp->installEventFilter(this);

    // Theme
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &User::setDarkMode);
    setDarkMode(tm.isDarkMode());
}

bool User::eventFilter(QObject *o, QEvent *event) {
    if (!userMenu || !userMenu->isVisible())
        return QWidget::eventFilter(o, event);

    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *me = static_cast<QMouseEvent *>(event);
        QPoint globalPos = me->globalPosition().toPoint();
        QWidget *clickedWidget = QApplication::widgetAt(globalPos);

        // Click outside everything
        if (!clickedWidget) {
            userMenu->fadeOut();
            return QWidget::eventFilter(o, event);
        }

        bool clickedInsideMenu = userMenu->isAncestorOf(clickedWidget);
        bool clickedOnUser = this->isAncestorOf(clickedWidget);

        if (!clickedInsideMenu && !clickedOnUser)
            userMenu->fadeOut();
    }

    if (event->type() == QEvent::ApplicationDeactivate)
        userMenu->fadeOut();

    return QWidget::eventFilter(o, event);
}

void User::setDarkMode(bool isDarkMode) {
    update();
}

void User::setName(const QString &name) {
    _name = name;
    adjustWidgetSize();
}

void User::adjustWidgetSize() {
    QFontMetrics fm(font());
    int textW = fm.horizontalAdvance(_name);

    const int spacing = 12;
    int totalW = 12 + _avator.width() + 12 + textW + 12;

    setFixedSize(totalW, 58);
}

QFont User::font() {
    QFont fnt;
    fnt.setFamily("Segoe UI");
    fnt.setPixelSize(14);
    fnt.setWeight(QFont::Medium);
    return fnt;
}

void User::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    bool isDarkMode = ThemeManager::instance().isDarkMode();
    // Background
    QColor brushColor;

    if (isPressed)
        brushColor = isDarkMode ? "#242424" : "#FFFFFF";
    else if (isHover)
        brushColor = isDarkMode ? "#323232" : "#F2F2F2";
    else if (!isEnabled())
        brushColor = isDarkMode ? "#555555" : "#E0E0E0";
    else
        brushColor = Qt::transparent;

    painter.setBrush(brushColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 6, 6);

    // Avator
    int avatarY = (height() - _avator.height()) / 2;
    painter.drawPixmap(12, avatarY, _avator.width(), _avator.height(), _avator);

    // Name Text
    painter.setFont(font());
    painter.setPen(isDarkMode ? Qt::white : Qt::black);

    int textX = 12 + _avator.width() + 12;
    int textW = width() - textX - 12;

    QRect textRect(textX, 0, textW, height());
    painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, _name);
}

void User::mousePressEvent(QMouseEvent *event) {
    isPressed = true;

    if (event->button() == Qt::LeftButton)
        emit clicked();

    QWidget::mousePressEvent(event);
    update();
}

void User::mouseReleaseEvent(QMouseEvent *event)
{
    isPressed = false;
    QWidget::mouseReleaseEvent(event);
    update();
}

void User::enterEvent(QEnterEvent *event)
{
    isHover = true;
    QWidget::enterEvent(event);
    update();
}

void User::leaveEvent(QEvent *event)
{
    isHover = false;
    QWidget::leaveEvent(event);
    update();
}
