#include "SubWindow.h"

SubWindow::SubWindow(QSize size, QWidget *parent, bool closeButton, bool minimizeButton) : QWidget(parent), hasCloseBtn(closeButton), hasMinimizeBtn(minimizeButton)
{
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setFixedSize(size);
    setupTitleBar();
    applyDWMEffects();
}

void SubWindow::setDarkMode(bool value) {
    isDarkMode = value; 
    if (closeBtn) closeBtn->setDarkMode(isDarkMode);
    if (minimizeBtn) minimizeBtn->setDarkMode(isDarkMode);

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
    if (closeBtn)  closeBtn->setUnicodeIcon("\uE8BB", 10);
    if (minimizeBtn) minimizeBtn->setUnicodeIcon("\uE921", 10);
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
    b->setIconSize(QSize(16,16));
    b->setDisplayMode(Button::IconOnly);
    b->setSize(QSize(26, 26));
    return b;
}

void SubWindow::setupTitleBar() {
    // Content Area
    _contentArea = new QWidget(this);
    _contentArea->setGeometry(0,0, width(), height());
    _contentArea->setContentsMargins(0, 0, 0, 0);
    _contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _contentArea->setAttribute(Qt::WA_TranslucentBackground);

    // Title Bar
    titleBar = new QWidget(this);
    titleBar->setGeometry(0, 3, width(), 30);
    titleBar->setContentsMargins(0, 0, 0, 0);
    titleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    titleBar->setAttribute(Qt::WA_TranslucentBackground);

    // Close Button
    if (hasCloseBtn) {
        closeBtn = windowButton();
        closeBtn->setParent(titleBar);
        connect(closeBtn, &Button::clicked, this, &SubWindow::onCloseClicked);
    }

    // Minimze Button
    if (hasMinimizeBtn) {
        minimizeBtn = windowButton();
        minimizeBtn->setParent(titleBar);
        connect(minimizeBtn, &Button::clicked, this, &SubWindow::onMinimizedClicked);
    }

    hwnd = reinterpret_cast<HWND>(winId());

    // Apply Icons
    applyThemedIcons();

    // Buttons Position
    int x = titleBar->width() - 26 - 5;
    int y = (titleBar->height() - 26) / 2;

    titleBar->raise();

    if (closeBtn) {
        closeBtn->move(x, y);
        closeBtn->raise();
    }

    if (minimizeBtn) {
        minimizeBtn->move(x - minimizeBtn->width() - 5, y);
        minimizeBtn->raise();
    }
}

void SubWindow::onCloseClicked() {
    ::SendMessage(hwnd, WM_CLOSE, 0, 0);
}

void SubWindow::onMinimizedClicked() {
    ::SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
}

bool SubWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
{
    MSG *msg = (MSG *)message;

    switch (msg->message) {
    case WM_NCCALCSIZE:
        *result = 0;
        return true;

    default:
        break;
    }

    return false;
}

void SubWindow::showEvent(QShowEvent *event)
{
    applyDWMEffects();
}

void SubWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (titleBar && titleBar->geometry().contains(event->pos())) {
            for (auto *widget : {closeBtn, minimizeBtn}) {
                if (widget && widget->geometry().contains(event->pos())) return;
            }
    
            m_dragging = true;
            m_dragStartPos = event->globalPos() - frameGeometry().topLeft();
        }
    }

    QWidget::mousePressEvent(event);
}

void SubWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - m_dragStartPos);
    }

    QWidget::mouseMoveEvent(event);
}

void SubWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }

    QWidget::mouseReleaseEvent(event);
}

QWidget* SubWindow::titleBarArea() const { return titleBar; }
QWidget* SubWindow::contentArea() const { return _contentArea; }
