#include "View.h"

View::View(QWidget *parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
    // Empty State Widget
    empty_state = new EmptyStateWidget;

    // Layout
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(0);
    layout->addWidget(empty_state, 0, Qt::AlignCenter);

}

void View::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(isDarkMode ? "#252424" : "#F9F9F9"));
    painter.drawRoundedRect(rect(), 6, 6);
}

void View::setDarkMode(bool enable) {
    isDarkMode = enable;

    // Empty state widget theme
    empty_state->setDarkMode(enable);
    
    update();
}
