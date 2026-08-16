#include "ErrorDialog.h"

#include "../../../core/theme/ThemeManager.h"

#include "../../components/Dialog.h"
#include "../../components/Button.h"
#include "../../components/Label.h"

#include <QVBoxLayout>
#include <QSize>
#include <QString>

/* ──────────────────────── Error Widget ──────────────────────── */

Error::Error(const QString &text,
             const QString &illustrationLight,
             const QString &illustrationDark,
             QWidget *parent)
    : QWidget(parent), lightIcon(illustrationLight), darkIcon(illustrationDark)
{
    setAttribute(Qt::WA_TranslucentBackground);

    layout = new QVBoxLayout(this);
    layout->setSpacing(0);

    actionBtn = new Button;
    actionBtn->setDisplayMode(Button::TextOnly);
    actionBtn->setFixedSize(QSize(316, 36));
    actionBtn->setText(tr("Retry"));
    actionBtn->setFontProperties("Segoe UI", 10, QFont::DemiBold);
    actionBtn->setBorderTransparent(true);

    QPixmap pixmap(lightIcon);
    illustration = new Label(true);
    illustration->setFixedSize(pixmap.size());
    illustration->setPixmap(pixmap);

    if (!text.isEmpty()) {
        label = new Label("Segoe UI", 10);
        label->setFixedSize(QSize(290, 36));
        label->setWordWrap(true);
        label->setText(text);
    }

    layout->addStretch();
    layout->addWidget(illustration, 0, Qt::AlignCenter);
    layout->addSpacing(2);

    if (label) {
        layout->addStretch();
        layout->addWidget(label, 0, Qt::AlignCenter);
        layout->addSpacing(12);
    }

    layout->addWidget(actionBtn, 0, Qt::AlignCenter);
    layout->addStretch();

    setFixedWidth(342);
    adjustSize();

    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &Error::setDarkMode);
    setDarkMode(tm.isDarkMode());
}

void Error::setDarkMode(bool value) {
    isDarkMode = value;
    if (illustration)
        illustration->setPixmap(QPixmap(isDarkMode ? darkIcon : lightIcon));
    if (label)
        label->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");
}

Button* Error::actionButton() { return actionBtn; }

/* ──────────────────── Error Dialog Manager ──────────────────── */

ErrorDialogManager::ErrorDialogManager(QObject *parent) : QObject(parent)
{
    create("InvalidCredentials",
           tr("The email or password you entered is incorrect. Please try again."),
           tr("Retry"),
           WrongCredentialsIllustration);

    create("MaxAttempts",
           tr("Maximum invalid attempts limit reached. Please try again after 24 hours."),
           tr("OK"),
           MaxLimitReachedIllustration);

    create("SomethingWentWrong",
           tr("Something went wrong. Please try again."),
           tr("Retry"),
           SomethingWentWrongIllustration);

    create("AccessDenied",
           tr("We have blocked access to your account from this device for security reasons."),
           tr("OK"),
           AccessDeniedIllustration);

    create("RequestTimeout",
           tr("Request timeout. Please check your internet connection and try again."),
           tr("Retry"),
           TimeoutIllustration);

    create("FurtherAttemptBlocked",
           tr("You cannot create multiple accounts on same device."),
           tr("OK"),
           AccessDeniedIllustration);

    create("NoInternet",
           tr("You're offline. We couldn't connect to the internet. Check your connection and try again."),
           tr("Retry"),
           NoInternetIllustration);
}

ErrorDialogManager* ErrorDialogManager::instance()
{
    static auto *manager = new ErrorDialogManager(qApp);
    return manager;
}

void ErrorDialogManager::create(const QString &key,
                                 const QString &text,
                                 const QString &actionButtonText,
                                 const QString &iconPath)
{
    auto *item = new ErrorDialogItem;

    item->widget = new Error(text, iconPath, iconPath);
    item->dialog = nullptr; 

    auto actionBtn = item->widget->actionButton();
    actionBtn->setText(actionButtonText);
    connect(actionBtn, &Button::clicked, this, [this, key]() {
        emit actionTriggered(key);
        close(key);
    });

    dialogs.insert(key, item);
}

void ErrorDialogManager::show(const QString &dialogKey, const QString &windowKey) {
    if (!dialogs.contains(dialogKey))
        return;

    QWidget *parent = parentWindows.value(windowKey, nullptr);
    if (!parent)
        return;

    auto *item = dialogs[dialogKey];
    if (!item || !item->widget)
        return;

    item->dialog = new Dialog(item->widget, parent, false);

    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, item->dialog, &Dialog::setDarkMode);
    item->dialog->setDarkMode(tm.isDarkMode());

    item->dialog->show();
}

void ErrorDialogManager::close(const QString &key) {
    if (!dialogs.contains(key))
        return;

    auto *item = dialogs[key];
    if (!item || !item->dialog)
        return;

    item->widget->setParent(nullptr);
    item->dialog->close();
    item->dialog->deleteLater();
}

void ErrorDialogManager::registerWindow(const QString &key, QWidget *window) {
    if (window)
        parentWindows[key] = window;
}