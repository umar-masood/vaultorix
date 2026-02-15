#include "Window.h"

Window::Window(QWidget *parent) : QWidget(nullptr), isDarkMode(false) {
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setupWindow();
}

void Window::setDarkMode(bool value) {
    isDarkMode = value; 
    for (Button *b : {closeBtn, minimizeBtn, maximizeBtn}) 
        b->setDarkMode(isDarkMode);

    applyThemedIcons();
    applyStyleSheet();
    update(); 
}

void Window::setInteractiveTitleBarWidget(QWidget *widget) {
    if (!widget) 
        return;
 
    if (interactiveWidgets.contains(widget)) 
        return;

    interactiveWidgets.insert(widget);
}

void Window::applyThemedIcons() {
    closeBtn->setIconPaths(closeIconLight, closeIconDark);
    minimizeBtn->setIconPaths(minimizeIconLight, minimizeIconDark);

    if (::IsZoomed(hwnd))
        maximizeBtn->setIconPaths(restoreIconLight, restoreIconDark);
    else
        maximizeBtn->setIconPaths(maximizeIconLight, maximizeIconDark);

    update();
}

void Window::applyStyleSheet() {
    QString stylesheet = QString("background-color: %1;").arg(isDarkMode ? "#1F1F1F" : "#FFFFFF");
    _mainTitleBar->setStyleSheet(stylesheet);
    _contentArea->setStyleSheet(stylesheet);
}


void Window::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QColor brushColor = isDarkMode ? QColor("#1F1F1F") : QColor("#FFFFFF");
    QColor penColor = isDarkMode ? QColor("#505050") : QColor("#5f5f5f");

    painter.setBrush(brushColor);
    painter.setPen(QPen(penColor, 0.5));
    painter.drawRoundedRect(rect().adjusted(2, 2, -2, -2), 6, 6);
}

void Window::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (_mainTitleBar->geometry().contains(event->pos()) &&
            !isPointInsideInteractiveTitleBarWidgets(event->position().x(), event->position().y())) 
        {
            if (windowHandle())
                windowHandle()->startSystemMove(); 
        }
    }

    QWidget::mousePressEvent(event);
}

Button * Window::createWindowButton() {
    Button *b = new Button;
    b->setSecondary(true);
    b->setIconSize(QSize(18, 18));
    b->setDisplayMode(Button::IconOnly);
    b->setCursor(Qt::PointingHandCursor);
    b->setBorderTransparent(true);
    b->setNormalBackgroundTransparent(true);
    b->setDisplayMode(Button::IconOnly);
    b->setFixedSize(QSize(26, 26));
    return b;
}

void Window::setupWindow() {
    /* Title Bar */
    _mainTitleBar = new QWidget(this);
    _mainTitleBar->setFixedHeight(36);
    _mainTitleBar->setContentsMargins(0, 0, 0, 0);
    _mainTitleBar->setAttribute(Qt::WA_TranslucentBackground);
    _mainTitleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    
    /* Sub Title Bar */
    _subTitleBar = new QWidget;
    _subTitleBar->setFixedHeight(36);
    _subTitleBar->setContentsMargins(0, 0, 0, 0);
    _subTitleBar->setAttribute(Qt::WA_TranslucentBackground);
    _subTitleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed); 
    
    /* Title Bar Main Layout*/
    _mainTitleBarLayout = new QHBoxLayout(_mainTitleBar);
    _mainTitleBarLayout->setContentsMargins(0, 0, 0, 0);
    _mainTitleBarLayout->setSpacing(0);
    _mainTitleBarLayout->addWidget(_subTitleBar);

    /* Window Controls*/
    closeBtn = createWindowButton();
    setInteractiveTitleBarWidget(closeBtn);
    connect(closeBtn, &Button::clicked, this, &Window::onCloseClicked);
    
    minimizeBtn = createWindowButton();
    setInteractiveTitleBarWidget(minimizeBtn);
    connect(minimizeBtn, &Button::clicked, this, &Window::onMinimizeClicked);
    
    maximizeBtn = createWindowButton();    
    setInteractiveTitleBarWidget(maximizeBtn);
    connect(maximizeBtn, &Button::clicked, this, &Window::onMaximizeClicked);

    _mainTitleBarLayout->addWidget(minimizeBtn, 0, Qt::AlignRight);
    _mainTitleBarLayout->addSpacing(4);
    _mainTitleBarLayout->addWidget(maximizeBtn, 0, Qt::AlignRight);
    _mainTitleBarLayout->addSpacing(4);
    _mainTitleBarLayout->addWidget(closeBtn, 0, Qt::AlignRight);
    _mainTitleBarLayout->addSpacing(6);

    /* Window Handle ID */
    hwnd = reinterpret_cast<HWND>(winId());

   /* Content Area */
   _contentArea = new QWidget(this);
   _contentArea->setContentsMargins(0, 0, 0, 0);
   _contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   setInteractiveTitleBarWidget(_contentArea);
   
   /* Entire Layout */
   entireLayout = new QVBoxLayout;
   entireLayout->setContentsMargins(4, 4, 4, 4);
   entireLayout->setSpacing(0);
   entireLayout->addWidget(_mainTitleBar, 0, Qt::AlignTop);
   entireLayout->addWidget(_contentArea, 0);
   setLayout(entireLayout);

   /* Apply Styles */
   applyStyleSheet();
   applyThemedIcons();
}

void Window::onCloseClicked() { ::SendMessage(hwnd, WM_CLOSE, 0, 0); }
void Window::onMinimizeClicked() { ::ShowWindow(hwnd, SW_MINIMIZE); }
void Window::onMaximizeClicked() {
    if (::IsZoomed(hwnd)) 
        ::ShowWindow(hwnd, SW_RESTORE);
    else 
        ::ShowWindow(hwnd, SW_MAXIMIZE);
    
    applyThemedIcons();
}

bool Window::isPointInsideInteractiveTitleBarWidgets(int x, int y) {
    QPoint globalPos(x, y);

    for (QWidget *widget : interactiveWidgets) {
        if (!widget)
            continue;

        QPoint localPos = widget->mapFromGlobal(globalPos);
        if (widget->rect().contains(localPos)) 
            return true;
    }
    return false;
}

QWidget* Window::titleBar() const { return _subTitleBar; }
QWidget* Window::contentArea() const { return _contentArea; }
