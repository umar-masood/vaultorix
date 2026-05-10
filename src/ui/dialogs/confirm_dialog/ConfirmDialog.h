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

/*  ----- Confirm Widget for using inside dialog ------------ */
class ConfirmWidget : public QWidget {
    Q_OBJECT

    public:
    ConfirmWidget(const QString &text = QString(),
                  const QString &illustrationLight = QString(),
                  const QString &illustrationDark = QString(),
                  QWidget *parent = nullptr
                );

    void setDarkMode(bool value);
    Button* acceptButton();
    Button* discardButton();

    private:
    // Theme Mode Flag
    bool isDarkMode = false;

    // Text Widget
    Label *label = nullptr;

    // Action Buttons
    Button *discardBtn = nullptr;
    Button *acceptBtn = nullptr;

    // Illustration
    Label *illustration = nullptr;

    // Main Layout
    QVBoxLayout *layout = nullptr;

    // Icon Paths
    QString lightIcon, darkIcon;
    QSize size;
};

/* ---------- Confirm Dialog Manager for handling multiple errors ------------ */
class ConfirmDialogManager : public QObject {
    Q_OBJECT

    public:
    static ConfirmDialogManager* instance();
    void show(const QString &dialogKey, const QString &windowKey);
    void close(const QString &key);
    void registerWindow(const QString &key, QWidget *window = nullptr);

    signals:
    void actionTriggered(const QString &key, const QString &actionButtonKey);

    private:    
    explicit ConfirmDialogManager(QObject *parent = nullptr);

    // Structure for error dialogs
    struct ConfirmDialogItem {
        QPointer<ConfirmWidget> widget;
        QPointer<Dialog> dialog;
    };

    // Icons
    const QString DeleteAccountIllustration  = IllustrationManager::illustration(Illustrations::DeleteAccount);

    // Hash to stored different Confirm Dialogs for efficiency
    QHash<QString, ConfirmDialogItem*> dialogs;

    // Hash to store parent windows
    QHash<QString, QPointer<QWidget>> parentWindows;

    // Helper Function to create confirm dialog box
    void create(const QString &key, 
                const QString &text, 
                const QString &acceptButtonText, 
                const QString &discardButtonText,
                const QString &iconPath,
                bool dangerZone = false
    );
};
