#include "Statusbar.h"

Statusbar::Statusbar(QWidget *parent) : QWidget(parent) {
    setFixedHeight(30);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Total Items
    total_items = createLabel("256 items");

    // Seperator 
    seperator = new Seperator(nullptr, 18);

    // Selected Items
    selected_items = createLabel("69 items selected");

    // Total Size
    total_size = createLabel("56,789 bytes");

    // Layout
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 0, 10, 0);
    layout->setSpacing(0);
    layout->addWidget(total_items, 0, Qt::AlignVCenter);
    layout->addSpacing(10);
    layout->addWidget(seperator, 0, Qt::AlignVCenter);
    layout->addSpacing(10);
    layout->addWidget(selected_items, 0, Qt::AlignVCenter);
    layout->addSpacing(10);
    layout->addWidget(total_size, 0, Qt::AlignVCenter);
    layout->addStretch();

    // Initial Theme
    setDarkMode(isDarkMode);
}

void Statusbar::setDarkMode(bool enable) {
    isDarkMode = enable;
    
    // Labels
    for (auto label : {total_items, selected_items, total_size})
        label->setStyleSheet(QString("color: %1;").arg(isDarkMode ? "white" : "black"));

    update();
}

Label* Statusbar::totalItemsLabel() const { return total_items; }
Label* Statusbar::selectedItemsLabel() const { return selected_items; }
Label* Statusbar::totalSizeLabel() const { return total_size; }

Label* Statusbar::createLabel(const QString &text) {
    return (new Label("Segoe UI", 10, QFont::Normal, false, text));
}

void Statusbar::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // Background
    painter.setBrush(QBrush(isDarkMode ? "#262626" : "#F9F9F9"));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 6, 6);
}

