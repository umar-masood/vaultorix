#include "View.h"

View::View(QWidget *parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
    // Empty State Widget
    empty_state = new EmptyStateWidget(this);
    empty_state->hide();

    // Menu Button (for testing)
    filterButtonMenu = new ButtonMenu(this);
    filterButtonMenu->setDisplayMode(Button::IconText);
    filterButtonMenu->setIconPaths(FilterIcon, FilterIcon);
    filterButtonMenu->setRightSideIcon(ArrowDownIcon, ArrowDownIcon);
    filterButtonMenu->setFontProperties("Segoe UI", 10);
    filterButtonMenu->setText("Filter");
    filterButtonMenu->setFontXY(0, -1);
    filterButtonMenu->menu()->delegate()->setSelectionCheckIndicator(true);
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

    // Wrapper (View Action Toolbar)
    view_toolbar_wrapper = new QWidget;
    view_toolbar_wrapper->setAttribute(Qt::WA_TranslucentBackground);
    view_toolbar_wrapper->setFixedHeight(36);
    view_toolbar_wrapper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    view_toolbar_wrapper->setContentsMargins(0, 0, 0, 0);
    
    // Wrapper Action Toolbar Layout
    wrapper_layout = new QHBoxLayout(view_toolbar_wrapper);
    wrapper_layout->setSpacing(0);
    wrapper_layout->setContentsMargins(0, 0, 0, 0);
    wrapper_layout->addWidget(filterButtonMenu, 0, Qt::AlignLeft | Qt::AlignVCenter);
    wrapper_layout->addWidget(search_box, 0, Qt::AlignHCenter);
    wrapper_layout->addWidget(view_mode, 0, Qt::AlignRight | Qt::AlignVCenter);
    
    // Items View List
    _list = new QListView;

    // Items View Delegate
    _delegate = new ViewDelegate;
    
    // Vertical Scroll Bar (Items List)
    vScroll = new ScrollBar(Qt::Vertical);

    // List Properties
    _list->setModel(&_model);
    _list->setMouseTracking(true);
    _list->setItemDelegate(_delegate);
    _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _list->setFrameShape(QFrame::NoFrame);
    _list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _list->setVerticalScrollBar(vScroll);
    _list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _list->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    _list->setStyleSheet("background: transparent; margin: 0px; padding: 0px; border:none; ");

    onListViewModeSelected();

    connect(view_mode, &ViewModeToggle::list_view_selected, this, &View::onListViewModeSelected);
    connect(view_mode, &ViewModeToggle::grid_view_selected, this, &View::onGridViewModeSelected);

    // ================== TEST ITEMS ==================
    _model.appendRow(new ViewItem("Notes.txt",  IconManager::icon(Icons::File_TXT),  "TXT",  "3 KB",   "12 Feb 2026", false, false));
    _model.appendRow(new ViewItem("Resume.pdf", IconManager::icon(Icons::File_PDF),  "PDF",  "210 KB", "11 Feb 2026", false, false));
    _model.appendRow(new ViewItem("Photo.jpg",  IconManager::icon(Icons::File_JPG),  "JPEG", "1.9 MB", "10 Feb 2026", false, true));
    _model.appendRow(new ViewItem("Old project code backup is the key of success obtaining.zip", IconManager::icon(Icons::File_ZIP), "ZIP", "720 MB", "21 Jan 2026", true, false));
    _model.appendRow(new ViewItem("Company logo vector.ai", IconManager::icon(Icons::File_AI), "AI", "3.3 MB", "20 Jan 2026", false, false));
    _model.appendRow(new ViewItem("Personal journal entry in conference room at night.txt", IconManager::icon(Icons::File_TXT), "TXT", "8 KB", "19 Jan 2026", true, false));
    _model.appendRow(new ViewItem("Meeting agenda for project propesla diswj.txt", IconManager::icon(Icons::File_TXT), "TXT", "4 KB", "05 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Landscape photography.jpg", IconManager::icon(Icons::File_JPG), "JPEG", "7.2 MB", "04 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Tutorial video.mp4", IconManager::icon(Icons::File_MP4), "MP4", "340 MB", "03 Dec 2025", false, true));
    
    _model.appendRow(new ViewItem("Notes.txt",  IconManager::icon(Icons::File_TXT),  "TXT",  "3 KB",   "12 Feb 2026", false, false));
    _model.appendRow(new ViewItem("Resume.pdf", IconManager::icon(Icons::File_PDF),  "PDF",  "210 KB", "11 Feb 2026", false, false));
    _model.appendRow(new ViewItem("Photo.jpg",  IconManager::icon(Icons::File_JPG),  "JPEG", "1.9 MB", "10 Feb 2026", false, true));
    _model.appendRow(new ViewItem("Old project code backup is the key of success obtaining.zip", IconManager::icon(Icons::File_ZIP), "ZIP", "720 MB", "21 Jan 2026", true, false));
    _model.appendRow(new ViewItem("Company logo vector.ai", IconManager::icon(Icons::File_AI), "AI", "3.3 MB", "20 Jan 2026", false, false));
    _model.appendRow(new ViewItem("Personal journal entry in conference room at night.txt", IconManager::icon(Icons::File_TXT), "TXT", "8 KB", "19 Jan 2026", true, false));
    _model.appendRow(new ViewItem("Meeting agenda for project propesla diswj.txt", IconManager::icon(Icons::File_TXT), "TXT", "4 KB", "05 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Landscape photography.jpg", IconManager::icon(Icons::File_JPG), "JPEG", "7.2 MB", "04 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Tutorial video.mp4", IconManager::icon(Icons::File_MP4), "MP4", "340 MB", "03 Dec 2025", false, true));
    

    // Layout
    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(4, 4, 4, 4);
    _layout->setSpacing(0);
    _layout->addWidget(view_toolbar_wrapper, 0, Qt::AlignTop);
    _layout->addSpacing(10); // IMP : Seperator is used
    _layout->addWidget(_list);
    
    // Model Signal / Slots
    connect(&_model, &QAbstractItemModel::rowsInserted, this, &View::updateEmptyState);
    connect(&_model, &QAbstractItemModel::rowsRemoved, this, &View::updateEmptyState);
    connect(&_model, &QAbstractItemModel::modelReset, this, &View::updateEmptyState);

    // Initial Theme
    setDarkMode(isDarkMode);

    // Positioning Empty State
    updateEmptyStatePosition();
}

void View::onListViewModeSelected() {
    if (_delegate) {
        _delegate->setViewMode(ItemsViewMode::ListMode);
        _delegate->setItemHeight(70);
    }

    if (_list) {
        _list->setViewMode(QListView::ListMode);
        _list->setSpacing(0);
        _list->setWrapping(false);
        _list->setUniformItemSizes(true);
        _list->setResizeMode(QListView::Fixed);  
    }
};

void View::onGridViewModeSelected() {
    if (_delegate) {
        _delegate->setViewMode(ItemsViewMode::GridMode);
    }

    if (_list) {
        _list->setViewMode(QListView::IconMode);
        _list->setSpacing(4);
        _list->setUniformItemSizes(true);  
        _list->setResizeMode(QListView::Adjust);
        _list->setWrapping(true);  
        _list->setDragEnabled(false); 
        _list->setAcceptDrops(false);          
        _list->setDragDropMode(QAbstractItemView::NoDragDrop);  
        _list->setDefaultDropAction(Qt::IgnoreAction); 
    }
};

void View::updateEmptyState() {
    bool isEmpty = (_model.rowCount() == 0);

    if (empty_state)
        empty_state->setVisible(isEmpty); 
}

void View::updateEmptyStatePosition() {
    if (empty_state) {
        int x = (width() - empty_state->width()) / 2;
        int y = ((height() - empty_state->height()) / 2) + 10;
        empty_state->raise();
        empty_state->move(x, y);
    }
}

void View::resizeEvent(QResizeEvent *event) {
   updateEmptyStatePosition();
   QWidget::resizeEvent(event);
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

    // View Items Delegate
    _delegate->setDarkMode(isDarkMode);

    // View Items List Scrollbar
    vScroll->setDarkMode(isDarkMode);
    
    update();
}

TextField* View::searchBox() const { return search_box; }
ButtonMenu* View::filterMenu()  const { return filterButtonMenu; }
ViewModeToggle* View::viewMode() const { return view_mode; }
EmptyStateWidget* View::emptyStateWidget() const { return empty_state; }
