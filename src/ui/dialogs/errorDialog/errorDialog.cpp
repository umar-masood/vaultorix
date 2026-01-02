#include "errorDialog.h"

Error::Error(QSize widgetSize, const QString &text, const QString &illustrationLight, const QString &illustrationDark, QSize iconSize, QWidget *parent) : QWidget(parent), lightIcon(illustrationLight), darkIcon(illustrationDark), size(iconSize) {
    
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(widgetSize);

    layout = new QVBoxLayout(this);
    layout->setSpacing(0);

    // Action Button
    actionBtn = new Button;
    actionBtn->setDisplayMode(Button::TextOnly);
    actionBtn->setFixedSize(QSize(316, 36));
    actionBtn->setText("Retry");

    // Icon
    illustration = new QLabel;
    illustration->setAttribute(Qt::WA_TranslucentBackground);
    illustration->setFixedSize(size);
    illustration->setAlignment(Qt::AlignCenter);
    illustration->setPixmap(QPixmap(lightIcon).scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Text (only if provided)
    if (!text.isEmpty()) {
        textWidget = new Text;
        textWidget->setFixedSize(QSize(290, 36));
        textWidget->setFont("Segoe UI", 10, false, false, Qt::AlignCenter);
        textWidget->setTextColor("#8D8D8D");
        textWidget->setWordWrap(true);
        textWidget->setText(text);
    }

    layout->addStretch();
    layout->addSpacing(4);
    layout->addWidget(illustration, 0, Qt::AlignCenter);
    layout->addSpacing(16);

    if (textWidget) {
        layout->addWidget(textWidget, 0, Qt::AlignCenter);
        layout->addSpacing(16);
    }

    layout->addWidget(actionBtn, 0, Qt::AlignCenter);
    layout->addStretch();

    // Theme Change Signal Slot
    connect(this, &Error::themeModeChanged, this, &Error::onThemeModeChanged);
}

void Error::onThemeModeChanged(bool enable) {
    if (illustration)
        illustration->setPixmap(QPixmap(enable ? darkIcon : lightIcon).scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void Error::setDarkMode(bool value) {
    if (isDarkMode == value) return;
    isDarkMode = value;
    emit themeModeChanged(isDarkMode); 
}

Button * Error::actionButton() { return actionBtn; }

/* ---------------  Error Dialog Manager  --------------- */
ErrorDialogManager::ErrorDialogManager(AccountWindow *window, QObject *parent) : QObject(parent), accountWindow(window) {

    if (!accountWindow) return;

    create("InvalidCredentials", wrongCredentialsText, "Retry", wrongCredentialsIcon);
    create("MaxAttempts", maxAttemptsText, "OK", maxAttemptsIcon);
    create("SomethingWentWrong", somethingWentWrongText, "Retry", somethingWentWrongIcon);
    create("AccessDenied", accessDeniedText, "OK", accessDeniedIcon);
    create("RequestTimeout", timeoutText, "Retry", timeoutIcon);
    create("FurtherAttemptBlocked", futherAttemptBlockedText, "OK", futherAttemptBlockedIcon);

    accountWindow->setSubWidgets(allWidgets());
}

void ErrorDialogManager::create(const QString &key, const QString &text, const QString &actionButtonText, const QString &iconPath) {
    ErrorDialog ed;
    ed.widget = new Error(dialogSize, text, iconPath, iconPath, illustrationSize);
    ed.widget->actionButton()->setText(actionButtonText);

    ed.dialog = new Dialog(ed.widget, accountWindow->subWindow(), false);

    connect(ed.widget->actionButton(), &Button::clicked, this, [this, key]() {
        emit actionTriggered(key);
        close(key);
    });

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