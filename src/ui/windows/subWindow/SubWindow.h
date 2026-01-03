#pragma once
#include "../../components/Button.h"
#include "../../components/ToolTip.h"

#include <dwmapi.h>
#include <windowsx.h>
#include <windows.h>
#include <QWindow>
#include <QVBoxLayout>

class SubWindow : public QWidget {
    Q_OBJECT

public:
    explicit SubWindow(QSize size = QSize(250, 250), QWidget *parent = nullptr, bool closeButton = true, bool minimizeButton = false);
    virtual ~SubWindow() = default;

    void setDarkMode(bool value);
   
    QWidget* contentArea() const;
    QWidget* titleBarArea() const;

protected: 
    void paintEvent(QPaintEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onCloseClicked();
    void onMinimizedClicked();

private:
    void applyDWMEffects();
    void applyThemedIcons();
    void setupTitleBar();
    Button* windowButton();

    // Window Handle (Unique Identifier)
    HWND hwnd;

    // Dragging
    bool m_dragging = false;
    QPoint m_dragStartPos;

    // Window Controls    
    bool hasCloseBtn = true;
    Button *closeBtn = nullptr;
    ToolTip *closeBtnTip = nullptr;

    bool hasMinimizeBtn = false;
    Button *minimizeBtn = nullptr;
    ToolTip *minimizeBtnTip = nullptr;

    // Theme Mode
    bool isDarkMode = false;

    QWidget *_titleBarArea = nullptr;
    QWidget *_contentArea = nullptr;
    QVBoxLayout *entireLayout = nullptr;
};
