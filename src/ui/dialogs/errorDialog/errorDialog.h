#pragma once

#include "../../components/Dialog.h"
#include "../dialogsUtils.h"
#include "../../accountWindow/AccountWindow.h"
#include "../../windows/subWindow/SubWindow.h"
#include "../../components/Button.h"
#include "../../components/Text.h"

#include <QObject>
#include <QMap>
#include <QList>

using namespace DialogUtils;

class Error : public QWidget {
    Q_OBJECT

    public:
    explicit Error (QSize widgetSize = QSize(300,300),
                    const QString &text = QString(),
                    const QString &illustrationLight = QString(),
                    const QString &illustrationDark = QString(),
                    QSize iconSize = QSize(203, 201),
                    QWidget *parent = nullptr);

    Q_INVOKABLE void setDarkMode(bool value);
    Button* actionButton();

    signals:
    void themeModeChanged(bool enable);

    private slots:
    void onThemeModeChanged(bool enable);

    private:
    bool isDarkMode = false;

    Text *textWidget = nullptr;
    Button *actionBtn = nullptr;
    QLabel *illustration = nullptr;
    QVBoxLayout *layout = nullptr;

    QString lightIcon;
    QString darkIcon;
    QSize size;
};


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
    struct ErrorDialog {
        Error *widget = nullptr;
        Dialog *dialog = nullptr;
    };

    AccountWindow *accountWindow = nullptr;
    QMap<QString, ErrorDialog> dialogs;

    void create(
        const QString &key,
        const QString &text,
        const QString &actionButtonText,
        const QString &iconPath
    );
};
