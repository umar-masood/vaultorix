#include "SubWindow.h"

SubWindowOverlay::SubWindowOverlay(QWidget *parent) : QWidget(parent) {
   setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
   setAttribute(Qt::WA_TranslucentBackground);
   setAttribute(Qt::WA_TransparentForMouseEvents, true);
   setMouseTracking(false);
}

void SubWindowOverlay::paintEvent(QPaintEvent *event) {
   Q_UNUSED(event);

   // Colors
   QColor BG = QColor(0,0,0,80);

   QPainter painter(this);
   painter.setRenderHints(QPainter::Antialiasing);
   painter.setBrush(BG);
   painter.setPen(Qt::NoPen);

   QPainterPath path;
   path.addRoundedRect(rect().adjusted(1.5, 1.5, -1.5, -1.5), 6, 6);
   painter.drawPath(path);
}

SubWindow::SubWindow(QSize size, QWidget *parent, bool closeButton, bool minimizeButton) : QWidget(parent), hasCloseBtn(closeButton), hasMinimizeBtn(minimizeButton) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setFixedSize(size);

    // Content Area
    _contentArea = new QWidget(this);
    _contentArea->setGeometry(0,0, width(), height());
    _contentArea->setContentsMargins(0, 0, 0, 0);
    _contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _contentArea->setAttribute(Qt::WA_TranslucentBackground);

    // Title Bar
    _titleBar = new QWidget(this);
    _titleBar->setGeometry(0, 2, width(), 30);
    _titleBar->setContentsMargins(0, 0, 0, 0);
    _titleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    _titleBar->setAttribute(Qt::WA_TranslucentBackground);

    // Titlebar Layouts
    // Main Layout
    titlebar_layout = new QHBoxLayout(_titleBar);
    titlebar_layout->setContentsMargins(4, 0, 4, 0);
    titlebar_layout->setSpacing(0);

    // Sublayout (for adding custom widgets inside titlebar)
    titlebar_sublayout = new QHBoxLayout;
    titlebar_sublayout->setContentsMargins(0, 0, 0, 0);
    titlebar_sublayout->setSpacing(0);

    // Close Button
    closeBtn = windowButton();
    if (!hasCloseBtn) {
        closeBtn->setEnabled(false);
        closeBtn->hide();
    }
    connect(closeBtn, &Button::clicked, this, &SubWindow::onCloseClicked);

    // Tooltip
    closeBtnTip = new ToolTip(closeBtn);
    closeBtnTip->setText("Close");

    // Minimize Button
    minimizeBtn = windowButton();
    if (!hasMinimizeBtn) {
        minimizeBtn->setEnabled(false);
        minimizeBtn->hide();
    }
    connect(minimizeBtn, &Button::clicked, this, &SubWindow::onMinimizedClicked);
    
    // Tooltip
    minimizeBtnTip = new ToolTip(minimizeBtn);
    minimizeBtnTip->setText("Minimize");

    // Controls layout
    win_controls_layout = new QHBoxLayout;
    win_controls_layout->setContentsMargins(0, 0, 0, 0);
    win_controls_layout->setSpacing(0);

    // Adding buttons to controls layout
    win_controls_layout->addWidget(minimizeBtn, 0, Qt::AlignRight);
    if (hasMinimizeBtn) win_controls_layout->addSpacing(4);
    win_controls_layout->addWidget(closeBtn, 0, Qt::AlignRight);

    // Adding sublayout, close and minimize button to titlebar layout
    titlebar_layout->addLayout(titlebar_sublayout);
    titlebar_layout->addLayout(win_controls_layout);
    titlebar_layout->setAlignment(win_controls_layout, Qt::AlignRight);

    // Window Handle
    hwnd = reinterpret_cast<HWND>(winId());

    // Apply Icons
    applyThemedIcons();

    // Apply DWM Effects from Native Window API
    applyDWMEffects();
}

void SubWindow::setModal(bool enable) {
    if (_useOverlay == enable)
        return;

    _useOverlay = enable;

    _useOverlay ? createOverlay() : destroyOverlay();
}

void SubWindow::createOverlay() {
    if (!parentWidget() || overlay)
        return;

    overlay = new SubWindowOverlay(parentWidget());
    overlay->setGeometry(parentWidget()->rect());
    overlay->hide();

    parentWidget()->installEventFilter(this);
}

void SubWindow::destroyOverlay() {
    if (!overlay)
        return;

    overlay->deleteLater();
    overlay = nullptr;

    if (parentWidget())
        parentWidget()->removeEventFilter(this);
}

void SubWindow::centerInParent() {
    if (parentWidget()) {
        QScreen *screen = QApplication::screenAt(QCursor::pos());
        if (!screen) 
            screen = QApplication::primaryScreen();

        QRect parentRect = parentWidget()->geometry();
        int x = parentRect.x() + (parentRect.width() - width()) / 2;
        int y = parentRect.y() + (parentRect.height() - height()) / 2;
        move(x, y);
    }
}

void SubWindow::setDarkMode(bool value) {
    isDarkMode = value; 

    if (closeBtn) {
        closeBtn->setDarkMode(isDarkMode);
        closeBtnTip->setDarkMode(isDarkMode);
    }

    if (minimizeBtn) {
       minimizeBtn->setDarkMode(isDarkMode); 
       minimizeBtnTip->setDarkMode(isDarkMode);
    } 

    update();
    applyThemedIcons();
}

void SubWindow::applyDWMEffects() {
    hwnd = reinterpret_cast<HWND>(winId());
    /*
     * LONG style = GetWindowLong(hwnd, GWL_STYLE);
     *
     * - LONG: A 32-bit signed integer type (typedef long). In the Win32 API,
     *         window style flags are represented as a LONG bitmask.
     *
     * - GetWindowLong(hwnd, GWL_STYLE): Retrieves information about a window.
     *   hwnd: The handle to the window.
     *   GWL_STYLE: A constant (-16) that means "get the window's style flags".
     *   These flags define how the window behaves (has a caption, border, etc.).
     */
    LONG style = GetWindowLong(hwnd, GWL_STYLE);

    /*
     * style |= WS_CAPTION;
     *
     * - WS_CAPTION: A predefined constant (0x00C00000) in WinUser.h.
     *   It enables a title bar for the window. Even though we’re using
     *   a frameless Qt window, setting WS_CAPTION is useful to let Windows
     *   handle resizing and dragging logic when we simulate a custom title bar.
     *
     * - |= operator: Bitwise OR assignment.
     *   Adds the WS_CAPTION bit to the style without removing other flags.
     */
    style |= WS_CAPTION;

    /*
     * SetWindowLong(hwnd, GWL_STYLE, style);
     *
     * - SetWindowLong: Changes information about a window.
     *   hwnd: Handle to the window we want to modify.
     *   GWL_STYLE: Index specifying we’re updating style flags.
     *   style: The new LONG bitmask with WS_CAPTION included.
     *
     * - This applies the modified style back to the window.
     */
    SetWindowLong(hwnd, GWL_STYLE, style);

    /*
     * const DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;
     *
     * - DWORD: A 32-bit unsigned integer type (typedef unsigned long).
     *   Frequently used in Windows APIs for flags, enums, or indices.
     *
     * - DWMWA_WINDOW_CORNER_PREFERENCE: A constant attribute index
     *   used with DwmSetWindowAttribute().
     *   It tells Windows we want to control how corners are drawn.
     */
    const DWORD DWMWA_WINDOW_CORNER_PREFERENCE = 33;

    enum DWM_WINDOW_CORNER_PREFERENCE {
        DWMWCP_ROUND      = 2,
        DWMWCP_ROUNDSMALL = 3
    };

    DWM_WINDOW_CORNER_PREFERENCE pref = DWMWCP_ROUND;

    /*
     * DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
     *
     *   DWMWA_WINDOW_CORNER_PREFERENCE: The attribute index we’re setting (33).
     *   &pref: Pointer to the value we want to set (our enum instance).
     *   sizeof(pref): Size in bytes of the value being passed.
     */
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));

    /*
     * SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
     *              SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
     *
     * - SetWindowPos: Changes the size, position, or Z-order of a window.
     *   hwnd: Handle to the window.
     *   nullptr: No insert-after window (we’re not changing Z-order).
     *   0,0,0,0: Since we’re not moving/resizing, these values are ignored.
     *
     * - Flags:
     *   SWP_NOZORDER: Don’t change stacking order.
     *   SWP_NOMOVE: Don’t move the window.
     *   SWP_NOSIZE: Don’t resize the window.
     *   SWP_FRAMECHANGED: Recalculate the window’s non-client area
     *                     (caption, borders) based on new style flags.
     *
     * - Net effect: Forces Windows to refresh/redraw the frame without
     *   actually resizing or moving the window.
     */
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                 SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
}

void SubWindow::applyThemedIcons() {
    if (closeBtn)
        closeBtn->setIconPaths(closeIconLight, closeIconDark);

    if (minimizeBtn) 
        minimizeBtn->setIconPaths(minimizeIconLight, minimizeIconDark);
}

void SubWindow::paintEvent(QPaintEvent *event) {
    QColor BG = isDarkMode ? QColor("#1F1F1F") : QColor("#FFFFFF");
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.fillRect(rect(), BG);
}

Button* SubWindow::windowButton() {
    Button *b = new Button;
    b->setSecondary(true);
    b->setCursor(Qt::PointingHandCursor);
    b->setIconSize(QSize(18, 18));
    b->setDisplayMode(Button::IconOnly);
    b->setNormalBackgroundTransparent(true);
    b->setBorderTransparent(true);
    b->setFixedSize(QSize(26, 26));
    return b;
}

void SubWindow::onCloseClicked() { ::SendMessage(hwnd, WM_CLOSE, 0, 0); }
void SubWindow::onMinimizedClicked() { ::SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0); }
bool SubWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
    MSG *msg = (MSG *)message;

    switch (msg->message) {
        case WM_NCCALCSIZE:
            *result = 0;
            return true;

        default: break;
    }

    return false;
}

bool SubWindow::eventFilter(QObject *obj, QEvent *event) {
    if (_useOverlay && obj == parentWidget() && overlay) {
        if (event->type() == QEvent::Resize ||
            event->type() == QEvent::Move)
        {
            overlay->setGeometry(parentWidget()->rect());
        }
    }

    return QWidget::eventFilter(obj, event);
}
void SubWindow::showEvent(QShowEvent *event) { 
    applyDWMEffects();

    if (_useOverlay && overlay && parentWidget()) {
        overlay->setGeometry(parentWidget()->rect());
        overlay->show();
        overlay->raise();
    }
    
    centerInParent();
    this->raise();
    QWidget::showEvent(event);
}

void SubWindow::closeEvent(QCloseEvent *event) {
    if (overlay)
        overlay->hide();

    QWidget::closeEvent(event);
}

void SubWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (_titleBar && _titleBar->geometry().contains(event->pos())) {
            m_dragging = true;
            m_dragStartPos = event->globalPos() - frameGeometry().topLeft();
        }
    }

    QWidget::mousePressEvent(event);
}

void SubWindow::mouseMoveEvent(QMouseEvent *event) {
    if (m_dragging && (event->buttons() & Qt::LeftButton)) 
        move(event->globalPos() - m_dragStartPos);
    
    QWidget::mouseMoveEvent(event);
}

void SubWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) 
        m_dragging = false;
    
    QWidget::mouseReleaseEvent(event);
}

QHBoxLayout* SubWindow::titlebarLayout() const { return titlebar_sublayout; }
QWidget* SubWindow::contentArea() const { return _contentArea; }
