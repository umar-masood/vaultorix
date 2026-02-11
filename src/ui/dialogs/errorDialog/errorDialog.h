#pragma once

#include "../../components/Dialog.h"
#include "../../account/accountWindow/AccountWindow.h"
#include "../../windows/subWindow/SubWindow.h"
#include "../../components/Button.h"
#include "../../components/Label.h"
#include "../../../resources/IllustrationManager.h"

#include <QObject>
#include <QMap>
#include <QList>

/*  ----- Error Widget for using inside dialog ------------ */
class Error : public QWidget {
    Q_OBJECT

    public:
    explicit Error( const QString &text = QString(),
                    const QString &illustrationLight = QString(),
                    const QString &illustrationDark = QString(),
                    QWidget *parent = nullptr);

    Q_INVOKABLE void setDarkMode(bool value);
    Button* actionButton();

    private:
    // Theme Mode Flag
    bool isDarkMode = false;

    // Text Widget
    Label *label = nullptr;

    // Action Button
    Button *actionBtn = nullptr;

    // Illustration
    Label *illustration = nullptr;

    // Main Layout
    QVBoxLayout *layout = nullptr;

    // Icon Paths
    QString lightIcon, darkIcon;
    QSize size;
};

/* ---------- Error Dialog Manager for handling multiple errors ------------ */
class ErrorDialogManager : public QObject {
    Q_OBJECT

    public:
    explicit ErrorDialogManager(AccountWindow *window, QObject *parent = nullptr);
    void show(const QString &key);
    void close(const QString &key);
    QList<QWidget *> allWidgets() const;

    signals:
    void actionTriggered(const QString &key);

    private:
    // Structure for error dialogs
    struct ErrorDialog {
        Error *widget = nullptr;
        Dialog *dialog = nullptr;
    };

    // Icons
    const QString NoInternetIllustration                = IllustrationManager::illustration(Illustrations::NoInternet);    
    const QString WrongCredentialsIllustration          = IllustrationManager::illustration(Illustrations::WrongCredentials);
    const QString UnknownDeviceIllustration             = IllustrationManager::illustration(Illustrations::UnknownDevice);
    const QString SomethingWentWrongIllustration        = IllustrationManager::illustration(Illustrations::SomethingWentWrong);
    const QString MaxLimitReachedIllustration           = IllustrationManager::illustration(Illustrations::MaxLimitReached);
    const QString AccessDeniedIllustration              = IllustrationManager::illustration(Illustrations::AccessDenied); 
    const QString TimeoutIllustration                   = IllustrationManager::illustration(Illustrations::Timeout); 

    // Stores the current accountWindow
    AccountWindow *accountWindow = nullptr;

    // Map to stored different Error Dialogs for efficiency
    QMap<QString, ErrorDialog*> dialogs;

    // Helper Function to create error dialog box
    void create(const QString &key, const QString &text, const QString &actionButtonText, const QString &iconPath);
};
