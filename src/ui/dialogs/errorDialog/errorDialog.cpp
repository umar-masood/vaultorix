#include "ErrorDialog.h"

/* -------------- Error Widget ----------------- */
Error::Error(const QString &text, const QString &illustrationLight, const QString &illustrationDark, QWidget *parent) : QWidget(parent), lightIcon(illustrationLight), darkIcon(illustrationDark) {
    
    setAttribute(Qt::WA_TranslucentBackground);

    // Size
    int errorWidgetHeight = 0;

    // Main Layout
    layout = new QVBoxLayout(this);
    layout->setSpacing(0);

    // Action Button
    actionBtn = new Button;
    actionBtn->setDisplayMode(Button::TextOnly);
    actionBtn->setFixedSize(QSize(316, 36));
    actionBtn->setText("Retry");
    errorWidgetHeight += 36;

    // Illustration
    QPixmap pixmap(lightIcon);
    errorWidgetHeight += pixmap.height();
    
    illustration = new QLabel;
    illustration->setAttribute(Qt::WA_TranslucentBackground);
    illustration->setFixedSize(pixmap.size() + QSize(20, 20));
    illustration->setAlignment(Qt::AlignCenter);
    illustration->setPixmap(pixmap);

    // Text (only if provided)
    if (!text.isEmpty()) {
        textWidget = new Text;
        textWidget->setFixedSize(QSize(290, 36));
        textWidget->setFont("Segoe UI", 10, false, false, Qt::AlignCenter);
        textWidget->setTextColor("#8D8D8D");
        textWidget->setWordWrap(true);
        textWidget->setText(text);

        // Font Handling
        QFontMetrics fm(textWidget->font());
        errorWidgetHeight += 36;
    }

    errorWidgetHeight += 42;
    setFixedSize(342, errorWidgetHeight);

    layout->addStretch();
    layout->addWidget(illustration, 0, Qt::AlignCenter);
    layout->addSpacing(12);

    if (textWidget) {
        layout->addStretch();
        layout->addWidget(textWidget, 0, Qt::AlignCenter);
        layout->addSpacing(12);
    }

    layout->addWidget(actionBtn, 0, Qt::AlignCenter);
    layout->addStretch();

    // Theme Change Signal Slot
    connect(this, &Error::themeModeChanged, this, &Error::onThemeModeChanged);

    setDarkMode(isDarkMode);
}

void Error::onThemeModeChanged(bool enable) {
    if (illustration)
        illustration->setPixmap(QPixmap(enable ? darkIcon : lightIcon));
}

void Error::setDarkMode(bool value) {
    if (isDarkMode == value) return;
    isDarkMode = value;
    emit themeModeChanged(isDarkMode); 
}

Button* Error::actionButton() { return actionBtn; }

/* ---------------  Error Dialog Manager  --------------- */
ErrorDialogManager::ErrorDialogManager(AccountWindow *window, QObject *parent) : QObject(parent), accountWindow(window) {

    if (!accountWindow) return;

    create("InvalidCredentials", wrongCredentialsText, "Retry", wrongCredentialsIcon);
    create("MaxAttempts", maxAttemptsText, "OK", maxAttemptsIcon);
    create("SomethingWentWrong", somethingWentWrongText, "Retry", somethingWentWrongIcon);
    create("AccessDenied", accessDeniedText, "OK", accessDeniedIcon);
    create("RequestTimeout", timeoutText, "Retry", timeoutIcon);
    create("FurtherAttemptBlocked", futherAttemptBlockedText, "OK", futherAttemptBlockedIcon);
    create("NoInternet", noInternetText, "Retry", noInternetIcon);

    accountWindow->setSubWidgets(allWidgets());
}

void ErrorDialogManager::create(const QString &key, const QString &text, const QString &actionButtonText, const QString &iconPath) {
    ErrorDialog ed;
    ed.widget = new Error(text, iconPath, iconPath);
    ed.widget->actionButton()->setText(actionButtonText);
    connect(ed.widget->actionButton(), &Button::clicked, this, [this, key]() {
        emit actionTriggered(key);
        close(key);
    });

    ed.dialog = new Dialog(ed.widget, accountWindow, false);

    dialogs.insert(key, ed);
}

void ErrorDialogManager::show(const QString &key) {
    if (dialogs.contains(key) && dialogs[key].dialog) 
        dialogs[key].dialog->show();    
}

void ErrorDialogManager::close(const QString &key) {
    if (dialogs.contains(key) && dialogs[key].dialog) 
        dialogs[key].dialog->close();
}

QList<QWidget *> ErrorDialogManager::allWidgets() const {
    QList<QWidget *> widgets;
    for (const auto &d : dialogs) 
        widgets << d.widget << d.dialog;
    
    return widgets;
}