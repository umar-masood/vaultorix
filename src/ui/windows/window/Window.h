#pragma once
#include "../../components/Button.h"
#include "../../../resources/IconManager.h"

#include <dwmapi.h>
#include <windowsx.h>
#include <windows.h>
#include <QWindow>
#include <QHBoxLayout>
#include <QObject>
#include <QPoint>
#include <QSet>
#include <QResizeEvent>
#include <QWidget>

class Window : public QWidget {
    Q_OBJECT

    public:
    explicit Window( QWidget *parent = nullptr);
    virtual ~Window() = default;

    enum class ResizeRegion {
        None,
        Left,
        Top,
        Right,
        Bottom,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };


    void setDarkMode(bool value);
    void setInteractiveTitleBarWidget(QWidget *widget);

    QWidget* titleBar() const;
    QWidget* contentArea() const;

    protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event);

    private:
    // Window Border
    bool showBorder = false;
    
    // Maximize / Restore
    QRect normalGeometry; 

    // Icons
    QString closeIconLight = IconManager::icon(Icons::Win_CloseLight);
    QString closeIconDark = IconManager::icon(Icons::Win_CloseDark);
    QString minimizeIconLight = IconManager::icon(Icons::Win_MinimizeLight);
    QString minimizeIconDark = IconManager::icon(Icons::Win_MinimizeDark);
    QString maximizeIconLight = IconManager::icon(Icons::Win_MaximizeLight);
    QString maximizeIconDark = IconManager::icon(Icons::Win_MaximizeDark);
    QString restoreIconLight = IconManager::icon(Icons::Win_RestoreLight);
    QString restoreIconDark = IconManager::icon(Icons::Win_RestoreDark);

    // Set Window Controls Icons
    void setWindowControlsIcons();
    void updateMaximizeIcon();

    // Window Resizing 
    void updateCursorForRegion(ResizeRegion region);
    ResizeRegion detectResizeRegion(const QPoint &pos);
    
    ResizeRegion currentResizeRegion = ResizeRegion::None;
    const int resizeMargin = 4;

    // Check weather the current coordinates lie inside titlebar interactive widgets
    bool isPointInsideInteractiveTitleBarWidgets(int x, int y);

    // Helper function to create window control 
    Button* createWindowButton();

    // Set to store unique interactive widgets
    QSet<QWidget *> interactiveWidgets;

    // Theme Mode flag
    bool isDarkMode;

    // Buttons
    Button *closeBtn = nullptr;
    Button *minimizeBtn = nullptr;
    Button *maximizeBtn = nullptr;
    
    // Main Title Bar
    QWidget *_mainTitleBar = nullptr;
    QHBoxLayout *_mainTitleBarLayout = nullptr;

    // Sub Title Bar (Custom Title Bar)
    QWidget *_subTitleBar = nullptr;
    QHBoxLayout *_subTitleBarLayout = nullptr;

    // Main Content Area
    QWidget *_contentArea = nullptr;
    QVBoxLayout *entireLayout = nullptr;

    private slots:
    void onCloseClicked();
    void onMaximizeClicked();
    void onMinimizeClicked();
};


