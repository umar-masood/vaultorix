#include "Subwindow.h"
#include "../window/Window.h"

#include "../../components/Button.h"

#include <QString>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QPaintEvent>
#include <QSize>
#include <QShowEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QScreen>
#include <QEvent>

SubWindowOverlay::SubWindowOverlay(QWidget *parent) : QWidget(parent) {
   setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
   setAttribute(Qt::WA_TranslucentBackground);
   setAttribute(Qt::WA_TransparentForMouseEvents, false);
   setMouseTracking(false);
}

void SubWindowOverlay::setRadius(int radius) {
    _radius = radius;
    update();
}

void SubWindowOverlay::paintEvent(QPaintEvent *event) {
   Q_UNUSED(event);

   // Colors
   QColor BG = QColor(0,0,0,120);

   QPainter painter(this);
   painter.setRenderHints(QPainter::Antialiasing);
   painter.setBrush(BG);
   painter.setPen(Qt::NoPen);

   QPainterPath path;
   path.addRoundedRect(_radius == 0 ? rect() : rect().adjusted(2, 2, -2, -2), _radius, _radius);
   painter.drawPath(path);
}

SubWindow::SubWindow(QSize size, QWidget *parent, bool closeButton, bool minimizeButton) : 
           QWidget(parent), hasCloseBtn(closeButton), hasMinimizeBtn(minimizeButton) 
{
    setFixedSize(size);
    init();
}

void SubWindow::init() {
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // Content Area
    _contentArea = new QWidget(this);
    _contentArea->setGeometry(0,0, width(), height());
    _contentArea->setContentsMargins(0, 0, 0, 0);
    _contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _contentArea->setAttribute(Qt::WA_TranslucentBackground);

    // Title Bar
    _titleBar = new QWidget(this);
    _titleBar->setGeometry(0, 5, width(), 30);
    _titleBar->setContentsMargins(4, 0, 4, 0);
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
    closeBtn->setToolTip("Close");

    if (!hasCloseBtn) {
        closeBtn->setEnabled(false);
        closeBtn->hide();
    }
    connect(closeBtn, &Button::clicked, this, &SubWindow::onCloseClicked);

    // Minimize Button
    minimizeBtn = windowButton();
    minimizeBtn->setToolTip("Minimize");

    if (!hasMinimizeBtn) {
        minimizeBtn->setEnabled(false);
        minimizeBtn->hide();
    }
    connect(minimizeBtn, &Button::clicked, this, &SubWindow::onMinimizedClicked);

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

    // Apply Icons
    applyThemedIcons();
}

void SubWindow::setModal(bool enable) {
    if (_useOverlay == enable)
        return;

    _useOverlay = enable;
    _useOverlay ? createOverlay() : destroyOverlay();
}

void SubWindow::setInteractionBlocked(bool enable) {
    _interactionBlocked = enable;
}

bool SubWindow::event(QEvent *event) {

    if (_interactionBlocked) {

        switch (event->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::MouseButtonDblClick:
        case QEvent::Wheel:
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
            return true; // Fully block
        default:
            break;
        }
    }

    return QWidget::event(event);
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

    if (closeBtn) 
        closeBtn->setDarkMode(isDarkMode);    

    if (minimizeBtn) 
       minimizeBtn->setDarkMode(isDarkMode); 

    update();
    applyThemedIcons();
}

void SubWindow::applyThemedIcons() {
    if (closeBtn)
        closeBtn->setIconPaths(closeIconLight, closeIconDark);

    if (minimizeBtn) 
        minimizeBtn->setIconPaths(minimizeIconLight, minimizeIconDark);
}

void SubWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QColor brushColor = isDarkMode ? QColor("#1F1F1F") : QColor("#FFFFFF");
    QColor penColor = "#109AC7";

    painter.setBrush(brushColor);
    painter.setPen(QPen(penColor, 0.5));
    painter.drawRoundedRect(rect().adjusted(2, 2, -2, -2), 6, 6);
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

void SubWindow::onCloseClicked() { close(); }
void SubWindow::onMinimizedClicked() { showMinimized();}

bool SubWindow::eventFilter(QObject *obj, QEvent *event) {
    if (_useOverlay && obj == parentWidget() && overlay) {
        if (event->type() == QEvent::Resize || event->type() == QEvent::Move) {
            overlay->setGeometry(parentWidget()->rect());
        }
    }

    return QWidget::eventFilter(obj, event);
}

void SubWindow::showEvent(QShowEvent *event) {
    if (_useOverlay) {
        if (!overlay && parentWidget())
            createOverlay();

        if (overlay) {
            overlay->setGeometry(parentWidget()->rect());
            overlay->show();
            overlay->raise();
        }
    }

    centerInParent();
    raise();

    if (parentWidget()) {
        if (auto *w = qobject_cast<Window *>(parentWidget())) {
            if (!w->isWindowNormal())
                modalOverlay()->setRadius(0);
            w->setInteractionBlocked(true);
        }

        if (auto *w = qobject_cast<SubWindow *>(parentWidget()))
            w->setInteractionBlocked(true);
    }

    QWidget::showEvent(event);
}

void SubWindow::closeEvent(QCloseEvent *event) {
    if (overlay)
        overlay->hide();

    if (parentWidget()) {
        if (auto w = qobject_cast<Window*>(parentWidget()))
            w->setInteractionBlocked(false);

        if (auto *w = qobject_cast<SubWindow *>(parentWidget())) 
            w->setInteractionBlocked(false);
    }

    QWidget::closeEvent(event);
}

void SubWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (_titleBar && _titleBar->geometry().contains(event->pos())) {
            if (windowHandle()) 
                windowHandle()->startSystemMove(); 
        }
    }

    QWidget::mousePressEvent(event);
}

QHBoxLayout* SubWindow::titlebarLayout() const { return titlebar_sublayout; }
SubWindowOverlay *SubWindow::modalOverlay() const { return overlay; }
QWidget *SubWindow::contentArea() const { return _contentArea; }
