#pragma once

#include "../../windows/subWindow/SubWindow.h"
#include "../../components/Button.h"
#include "../../components/Text.h"

class Error : public QWidget {
    Q_OBJECT

public:
    explicit Error(
                QSize widgetSize = QSize(300,300),
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
