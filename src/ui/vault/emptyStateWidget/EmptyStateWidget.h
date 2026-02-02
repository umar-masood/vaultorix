#pragma once

#include "../../components/Label.h"

#include <QVBoxLayout>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>

class EmptyStateWidget : public QWidget {
    Q_OBJECT

    public:
    explicit EmptyStateWidget(QWidget *parent = nullptr);
    void setDarkMode(bool enable);

    protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

    signals:
    void clicked();
    
    private:
    // Flags
    bool isDarkMode = false;
    bool isPressed = false;
    bool isHover = false;

    // Illustration
    const QString illustrationPathLight = ":/icons/vaultWindow/folder-illustration-light.svg";
    const QString illustrationPathDark = ":/icons/vaultWindow/folder-illustration-dark.svg";
    Label *illustration = nullptr;

    // Text
    Label *text = nullptr;

    // Layout
    QVBoxLayout *layout = nullptr;
};