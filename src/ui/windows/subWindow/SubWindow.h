#pragma once
#include "../../components/Button.h"
#include "../../components/ToolTip.h"
#include "../../../resources/IconManager.h"

#include <QWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>

class SubWindowOverlay : public QWidget {
   public:
   explicit SubWindowOverlay(QWidget *parent = nullptr);

   protected:
   void paintEvent(QPaintEvent *event) override;
};

class SubWindow : public QWidget {
    Q_OBJECT

    public:
    explicit SubWindow(QSize size = QSize(250, 250), QWidget *parent = nullptr, bool closeButton = true, bool minimizeButton = false);
    virtual ~SubWindow() = default;

    void setModal(bool enable);
    void setDarkMode(bool value);

    QWidget* contentArea() const;
    QHBoxLayout* titlebarLayout() const;

    protected: 
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    private slots:
    void onCloseClicked();
    void onMinimizedClicked();

    private:
    // ===== SubWindowOverlay for modal window =====
    SubWindowOverlay *overlay = nullptr;
    bool _useOverlay = false;
    void createOverlay();
    void centerInParent();
    void destroyOverlay();

    // Setters
    void applyThemedIcons();

    // Getter
    Button* windowButton();

    // Layouts
    QHBoxLayout *titlebar_layout = nullptr;
    QHBoxLayout *titlebar_sublayout = nullptr;
    QHBoxLayout *win_controls_layout = nullptr;

    // Window Controls    
    // Icons
    QString closeIconLight = IconManager::icon(Icons::Win_CloseLight);
    QString closeIconDark = IconManager::icon(Icons::Win_CloseDark);
    QString minimizeIconLight = IconManager::icon(Icons::Win_MinimizeLight);
    QString minimizeIconDark = IconManager::icon(Icons::Win_MinimizeDark);

    // Close Button
    bool hasCloseBtn = true;
    Button *closeBtn = nullptr;
    ToolTip *closeBtnTip = nullptr;

    // Minimize Button
    bool hasMinimizeBtn = false;
    Button *minimizeBtn = nullptr;
    ToolTip *minimizeBtnTip = nullptr;

    // Theme Mode
    bool isDarkMode = false;

    // Title Bar
    QWidget *_titleBar = nullptr;

    // Content Area
    QWidget *_contentArea = nullptr;

    // Entire Layout
    QVBoxLayout *entireLayout = nullptr;
};
