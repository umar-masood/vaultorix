#include "View.h"

View::View(QWidget *parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
    // Empty State Widget
    empty_state = new EmptyStateWidget;
    
    // Filter

    // Search Box
    search_box = new TextField;
    search_box->setPlaceholderText("Type to search your files…");
    search_box->setFixedSize(QSize(400, 32));
    search_box->setClearButton(true);
    search_box->setTextFieldIcon(true);
    search_box->setTextFieldIconSize(QSize(18, 18));
    search_box->setIconPaths(":/icons/vaultWindow/search.svg", ":/icons/vaultWindow/search.svg");

    // View Mode Toggle 
    view_mode = new ViewModeToggle;

    // Action Toolbar Layout
    actionToolbarLayout = new QHBoxLayout;
    actionToolbarLayout->setSpacing(0);
    actionToolbarLayout->setContentsMargins(4, 4, 4, 4);
    actionToolbarLayout->addWidget(search_box, 0, Qt::AlignHCenter);
    actionToolbarLayout->addWidget(view_mode, 0, Qt::AlignRight | Qt::AlignTop);

    // Layout
    layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(0);
    // layout->addWidget(empty_state, 0, Qt::AlignCenter); we will later uncomment it 
    layout->addLayout(actionToolbarLayout, 0);
    layout->addStretch();

    setDarkMode(isDarkMode);
}

void View::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(isDarkMode ? "#262626" : "#F9F9F9"));
    painter.drawRoundedRect(rect(), 6, 6);
}

void View::setDarkMode(bool enable) {
    isDarkMode = enable;

    // Search Box theme
    search_box->setDarkMode(enable);

    // View Mode Toggle theme
    view_mode->setDarkMode(enable);

    // Empty state widget theme
    empty_state->setDarkMode(enable);
    
    update();
}
