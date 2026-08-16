#pragma once

#include "../../../../resources/IllustrationManager.h"

#include <QObject>
#include <QList>
#include <QPointer>
#include <QHash>
#include <QWidget>

class Dialog;
class Button;
class Label;
class QVBoxLayout;
class QSize;
class QString;

/*  ----- Error Widget for using inside dialog ------------ */
class Error : public QWidget {
    Q_OBJECT

    public:
    explicit Error( const QString &text = QString(),
                    const QString &illustrationLight = QString(),
                    const QString &illustrationDark = QString(),
                    QWidget *parent = nullptr);

    void setDarkMode(bool value);
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
    static ErrorDialogManager* instance();
    void show(const QString &dialogKey, const QString &windowKey);
    void close(const QString &key);
    void registerWindow(const QString &key, QWidget *window = nullptr);

    signals:
    void actionTriggered(const QString &key);

    private:    
    explicit ErrorDialogManager(QObject *parent = nullptr);

    // Structure for error dialogs
    struct ErrorDialogItem {
        QPointer<Error> widget = nullptr;
        QPointer<Dialog> dialog = nullptr;
    };

    // Icons
    const QString NoInternetIllustration                = IllustrationManager::illustration(Illustrations::NoInternet);    
    const QString WrongCredentialsIllustration          = IllustrationManager::illustration(Illustrations::WrongCredentials);
    const QString UnknownDeviceIllustration             = IllustrationManager::illustration(Illustrations::UnknownDevice);
    const QString SomethingWentWrongIllustration        = IllustrationManager::illustration(Illustrations::SomethingWentWrong);
    const QString MaxLimitReachedIllustration           = IllustrationManager::illustration(Illustrations::MaxLimitReached);
    const QString AccessDeniedIllustration              = IllustrationManager::illustration(Illustrations::AccessDenied); 
    const QString TimeoutIllustration                   = IllustrationManager::illustration(Illustrations::Timeout); 

    // Map to stored different Error Dialogs for efficiency
    QHash<QString, ErrorDialogItem*> dialogs;

    // Map to store parent windows
    QHash<QString, QPointer<QWidget>> parentWindows;

    // Helper Function to create error dialog box
    void create(const QString &key, 
                const QString &text, 
                const QString &actionButtonText, 
                const QString &iconPath
            );
};
