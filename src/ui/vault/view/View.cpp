#include "View.h"

View::View(QWidget *parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
    // Empty State Widget
    empty_state = new EmptyStateWidget;

    // Menu Button (for testing)
    filterButtonMenu = new ButtonMenu(this);
    filterButtonMenu->setDisplayMode(Button::IconText);
    filterButtonMenu->setIconPaths(FilterIcon, FilterIcon);
    filterButtonMenu->setRightSideIcon(ArrowDownIcon, ArrowDownIcon);
    filterButtonMenu->setFontProperties("Segoe UI", 10);
    filterButtonMenu->setText("Filter");
    filterButtonMenu->setFontXY(0, -1);
    filterButtonMenu->menu()->delegate()->setSelectionDotIndicator(true);
    filterButtonMenu->menu()->addAction({"All file types"});
    filterButtonMenu->menu()->addAction({"Documents"});
    filterButtonMenu->menu()->addAction({"Pictures"});
    filterButtonMenu->menu()->addAction({"Music"});
    filterButtonMenu->menu()->addAction({"Videos"});
    filterButtonMenu->menu()->delegate()->setActiveIndex(filterButtonMenu->menu()->itemIndex("All file types")); // Setting Default one

    // Search Box
    search_box = new TextField;
    search_box->setPlaceholderText("Type to search your files…");
    search_box->setFixedSize(QSize(400, 32));
    search_box->setClearButton(true);
    search_box->setTextFieldIcon(true);
    search_box->setTextFieldIconSize(QSize(18, 18));
    search_box->setIconPaths(SearchIcon, SearchIcon);

    // View Mode Toggle 
    view_mode = new ViewModeToggle;

    // Action Toolbar Layout
    actionToolbarLayout = new QHBoxLayout;
    actionToolbarLayout->setSpacing(0);
    actionToolbarLayout->setContentsMargins(0, 0, 0, 0);
    actionToolbarLayout->addWidget(filterButtonMenu, 0, Qt::AlignLeft | Qt::AlignVCenter);
    actionToolbarLayout->addWidget(search_box, 0, Qt::AlignHCenter);
    actionToolbarLayout->addWidget(view_mode, 0, Qt::AlignRight | Qt::AlignVCenter);
    
    // Layout
    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(4, 4, 4, 4);
    _layout->setSpacing(0);

    _layout->addLayout(actionToolbarLayout, 0);
    _layout->setAlignment(actionToolbarLayout, Qt::AlignTop);

    _layout->addSpacing(10); // IMP : Seperator is used
    
    _layout->addWidget(empty_state, 0, Qt::AlignCenter);
    //_layout->addStretch();

    // Initial Theme
    setDarkMode(isDarkMode);
}

void View::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // Background
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(isDarkMode ? "#262626" : "#F9F9F9"));
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 6, 6);

    // Seperator
    painter.setPen(QPen(separatorColor, separatorHeight));
    painter.drawLine(0, 44, width(), 44);
}

void View::setDarkMode(bool enable) {
    isDarkMode = enable;

    // Filter Menu Button
    filterButtonMenu->setDarkMode(isDarkMode);

    // Search Box theme
    search_box->setDarkMode(isDarkMode);

    // View Mode Toggle theme
    view_mode->setDarkMode(isDarkMode);

    // Empty state widget theme
    empty_state->setDarkMode(isDarkMode);

    // Seperator color
    separatorColor = isDarkMode ? QColor("#1F1F1F") : QColor("#FFFFFF");
    
    update();
}

TextField* View::searchBox() const { return search_box; }
ButtonMenu* View::filterMenu()  const { return filterButtonMenu; }
ViewModeToggle* View::viewMode() const { return view_mode; }
EmptyStateWidget* View::emptyStateWidget() const { return empty_state; }
