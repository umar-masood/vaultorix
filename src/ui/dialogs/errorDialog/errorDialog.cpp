#include "ErrorDialog.h"

/* -------------- Error Widget ----------------- */
Error::Error(const QString &text, const QString &illustrationLight,
            const QString &illustrationDark, QWidget *parent) : 
            QWidget(parent), lightIcon(illustrationLight), darkIcon(illustrationDark) 
{
    
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
    actionBtn->setFontProperties("Segoe UI", 10, QFont::DemiBold);
    errorWidgetHeight += 36;

    // Illustration
    QPixmap pixmap(lightIcon);
    errorWidgetHeight += pixmap.height();
    
    illustration = new Label(true);
    illustration->setFixedSize(pixmap.size() + QSize(20, 20));
    illustration->setPixmap(pixmap);

    // Text (only if provided)
    if (!text.isEmpty()) {
        label = new Label("Segoe UI", 10);
        label->setFixedSize(QSize(290, 36));
        label->setWordWrap(true);
        label->setText(text);

        // Font Handling
        QFontMetrics fm(label->font());
        errorWidgetHeight += 36;
    }

    errorWidgetHeight += 42;
    setFixedSize(342, errorWidgetHeight);

    layout->addStretch();
    layout->addWidget(illustration, 0, Qt::AlignCenter);
    layout->addSpacing(12);

    if (label) {
        layout->addStretch();
        layout->addWidget(label, 0, Qt::AlignCenter);
        layout->addSpacing(12);
    }

    layout->addWidget(actionBtn, 0, Qt::AlignCenter);
    layout->addStretch();

    // Theme Mode
    setDarkMode(isDarkMode);
}

void Error::setDarkMode(bool value) {
    isDarkMode = value;
    
    if (illustration)
        illustration->setPixmap(QPixmap(isDarkMode ? darkIcon : lightIcon));
    
    if (label)
        label->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");
}

Button* Error::actionButton() { return actionBtn; }

/* ---------------  Error Dialog Manager  --------------- */
ErrorDialogManager::ErrorDialogManager(AuthWindow *instance, QObject *parent) : QObject(parent), authWindow(instance) {
    create("InvalidCredentials",       "The email or password you entered is incorrect. Please try again.",                         "Retry",    WrongCredentialsIllustration);
    create("MaxAttempts",              "Maximum invalid attempts limit reached. Please try again after 24 hours.",                  "OK",       MaxLimitReachedIllustration);
    create("SomethingWentWrong",       "Something went wrong. Please try again.",                                                   "Retry",    SomethingWentWrongIllustration);
    create("AccessDenied",             "We have blocked access to your account from this device for security reasons.",             "OK",       AccessDeniedIllustration);
    create("RequestTimeout",           "Request tmeout. Please check your internet connection and try again.",                      "Retry",    TimeoutIllustration);
    create("FurtherAttemptBlocked",    "You cannot create multiple accounts on same device.",                                       "OK",       AccessDeniedIllustration);
    create("NoInternet",               "You're offline. We couldn't connect to the internet. Check your connection and try again.", "Retry",    NoInternetIllustration);

    authWindow->setSubWidgets(allWidgets());
}

void ErrorDialogManager::create(const QString &key, const QString &text, const QString &actionButtonText, const QString &iconPath) {
    auto *ed = new ErrorDialog;
    ed->widget = new Error(text, iconPath, iconPath);
    ed->widget->actionButton()->setText(actionButtonText);
    connect(ed->widget->actionButton(), &Button::clicked, this, [this, key]() {
        emit actionTriggered(key);
        close(key);
    });

    ed->dialog = new Dialog(ed->widget, authWindow, false);

    dialogs.insert(key, ed);
}

void ErrorDialogManager::show(const QString &key) {
    if (dialogs.contains(key) && dialogs[key]->dialog) 
        dialogs[key]->dialog->show();    
}

void ErrorDialogManager::close(const QString &key) {
    if (dialogs.contains(key) && dialogs[key]->dialog) 
        dialogs[key]->dialog->close();
}

QList<QWidget *> ErrorDialogManager::allWidgets() const {
    QList<QWidget *> widgets;
    for (const auto &d : dialogs) 
        widgets << d->widget << d->dialog;
    
    return widgets;
}