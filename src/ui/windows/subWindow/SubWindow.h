#pragma once
#include <QtWidgets>
#include "../../components/Button.h"
#include <dwmapi.h>
#include <windowsx.h>
#include <windows.h>
#include <QWindow>

class SubWindow : public QWidget {
    Q_OBJECT

public:
    explicit SubWindow(QSize size = QSize(250, 250), QWidget *parent = nullptr, bool closeButton = true, bool minimizeButton = false);
    virtual ~SubWindow() = default;

    void setDarkMode(bool value);
   
    QWidget* contentArea() const;
    QWidget* titleBarArea() const;

    void onCloseClicked();
    void onMinimizedClicked();
   
protected: 
    void paintEvent(QPaintEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    HWND hwnd;
    void applyDWMEffects();
    void applyThemedIcons();
    void setupTitleBar();
    bool m_dragging = false;
    QPoint m_dragStartPos;
    Button* windowButton();

    Button *closeBtn = nullptr;
    Button *minimizeBtn = nullptr;

    bool isDarkMode = false;
    bool hasCloseBtn = true;
    bool hasMinimizeBtn = false;

    QWidget *titleBar = nullptr;
    QWidget *_contentArea = nullptr;
    QVBoxLayout *entireLayout = nullptr;
};
