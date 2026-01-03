#pragma once

#include "../../components/Dialog.h"
#include "../dialogUtils/DialogsUtils.h"
#include "../../accountWindow/AccountWindow.h"
#include "../../windows/subWindow/SubWindow.h"
#include "../../components/Button.h"
#include "../../components/Text.h"

#include <QObject>
#include <QMap>
#include <QList>

using namespace DialogUtils;

/*  ----- Error Widget for using inside dialog ------------ */
class Error : public QWidget {
    Q_OBJECT

    public:
    explicit Error(QSize widgetSize = QSize(300,300),
                    const QString &text = QString(),
                    const QString &illustrationLight = QString(),
                    const QString &illustrationDark = QString(),
                    QSize iconSize = QSize(203, 201),
                    QWidget *parent = nullptr);

    Q_INVOKABLE void setDarkMode(bool value);
    Button* actionButton();

    signals:
    void themeModeChanged(bool enable);

    private:
    // Theme Mode Flag
    bool isDarkMode = false;

    // Text Widget
    Text *textWidget = nullptr;

    // Action Button
    Button *actionBtn = nullptr;

    // Illustration
    QLabel *illustration = nullptr;

    // Main Layout
    QVBoxLayout *layout = nullptr;

    // Icon Paths
    QString lightIcon;
    QString darkIcon;
    QSize size;

    // Slot
    void onThemeModeChanged(bool enable);
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

    // Stores the current accountWindow
    AccountWindow *accountWindow = nullptr;

    // Map to stored different Error Dialogs for efficiency
    QMap<QString, ErrorDialog> dialogs;

    // Helper Function to create error dialog box
    void create(const QString &key, const QString &text, const QString &actionButtonText, const QString &iconPath);
};
