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
    _delegate->setItemHeight(70);
    
    // Vertical & Horizontal Scroll Bar (Items List)
    vScroll = new ScrollBar(Qt::Vertical);
    hScroll = new ScrollBar(Qt::Horizontal);

    // List Properties
    _list->setModel(&_model);
    _list->setMouseTracking(true);
    _list->setItemDelegate(_delegate);
    _list->setAutoScroll(true);
    _list->setViewMode(QListView::ListMode);
    _list->setSelectionMode(QAbstractItemView::ExtendedSelection);
    _list->setFrameShape(QFrame::NoFrame);
    _list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _list->setVerticalScrollBar(vScroll);
    _list->setHorizontalScrollBar(hScroll);
    _list->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    _list->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    _list->setStyleSheet("background: transparent; margin: 0px; padding: 0px; border:none; ");

    // ================== TEST ITEMS ==================
    _model.appendRow(new ViewItem("Notes.txt",  IconManager::icon(Icons::File_TXT),  "TXT",  "3 KB",   "12 Feb 2026", false, false));
    _model.appendRow(new ViewItem("Resume.pdf", IconManager::icon(Icons::File_PDF),  "PDF",  "210 KB", "11 Feb 2026", false, false));
    _model.appendRow(new ViewItem("Photo.jpg",  IconManager::icon(Icons::File_JPG),  "JPEG", "1.9 MB", "10 Feb 2026", false, true));
    _model.appendRow(new ViewItem("Music.mp3",  IconManager::icon(Icons::File_MP3),  "MP3",  "5.4 MB", "09 Feb 2026", false, false));
    _model.appendRow(new ViewItem("Video.mp4",  IconManager::icon(Icons::File_MP4),  "MP4",  "88 MB",  "08 Feb 2026", false, false));
    _model.appendRow(new ViewItem("Meeting notes.txt", IconManager::icon(Icons::File_TXT), "TXT", "12 KB", "07 Feb 2026", true, false));
    _model.appendRow(new ViewItem("Project overview.pdf", IconManager::icon(Icons::File_PDF), "PDF", "1.1 MB", "06 Feb 2026", false, false));
    _model.appendRow(new ViewItem("Travel itinerary.pdf", IconManager::icon(Icons::File_PDF), "PDF", "380 KB", "22 Jan 2026", false, false));
    _model.appendRow(new ViewItem("Old project code backup.zip", IconManager::icon(Icons::File_ZIP), "ZIP", "720 MB", "21 Jan 2026", true, false));
    _model.appendRow(new ViewItem("Company logo vector.ai", IconManager::icon(Icons::File_AI), "AI", "3.3 MB", "20 Jan 2026", false, false));
    _model.appendRow(new ViewItem("Personal journal entry.txt", IconManager::icon(Icons::File_TXT), "TXT", "8 KB", "19 Jan 2026", true, false));
    _model.appendRow(new ViewItem("Birthday celebration photos.jpg", IconManager::icon(Icons::File_JPG), "JPEG", "5.8 MB", "18 Jan 2026", false, true));.appendRow(new ViewItem("Product catalog images.zip", IconManager::icon(Icons::File_ZIP), "ZIP", "2.3 GB", "06 Dec 2025", true, false));
    _model.appendRow(new ViewItem("Meeting agenda.txt", IconManager::icon(Icons::File_TXT), "TXT", "4 KB", "05 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Landscape photography.jpg", IconManager::icon(Icons::File_JPG), "JPEG", "7.2 MB", "04 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Tutorial video.mp4", IconManager::icon(Icons::File_MP4), "MP4", "340 MB", "03 Dec 2025", false, true));
    _model.appendRow(new ViewItem("Complete set of royalty free sound effects and background music tracks for video and multimedia projects.mp3", IconManager::icon(Icons::File_MP3), "MP3", "500 MB", "16 Jan 2026", false, false));
    _model.appendRow(new ViewItem("Research paper covering applications of artificial intelligence in healthcare with full citations and references.pdf", IconManager::icon(Icons::File_PDF), "PDF", "4.7 MB", "15 Jan 2026", false, false));
    _model.appendRow(new ViewItem("Comprehensive tutorial on 3D modeling, rendering techniques, and project files for practice.zip", IconManager::icon(Icons::File_ZIP), "ZIP", "2.5 GB", "14 Jan 2026", true, false));
    _model.appendRow(new ViewItem("Step by step video guide to advanced Adobe Photoshop techniques for professional photo editing and manipulation.mp4", IconManager::icon(Icons::File_MP4), "MP4", "730 MB", "27 Dec 2025", false, true));
    _model.appendRow(new ViewItem("Comprehensive study on renewable energy policies, sustainable practices, and technological innovations worldwide.pdf", IconManager::icon(Icons::File_PDF), "PDF", "5.7 MB", "26 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Collection of classical and contemporary paintings scanned in high resolution for digital art study and reference.zip", IconManager::icon(Icons::File_ZIP), "ZIP", "2.3 GB", "25 Dec 2025", true, false));
    _model.appendRow(new ViewItem("Advanced lecture series on robotics, automation technologies, and practical implementation projects.mp4", IconManager::icon(Icons::File_MP4), "MP4", "990 MB", "24 Dec 2025", false, true));
    _model.appendRow(new ViewItem("Compilation of architectural blueprints, design templates, and building layouts for educational purposes.pdf", IconManager::icon(Icons::File_PDF), "PDF", "4.9 MB", "23 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Collection of motivational speeches, podcasts, and personal development audio files.mp3", IconManager::icon(Icons::File_MP3), "MP3", "310 MB", "22 Dec 2025", false, true));
    _model.appendRow(new ViewItem("Meeting notes.txt", IconManager::icon(Icons::File_TXT), "TXT", "12 KB", "07 Feb 2026", true, false));
    _model.appendRow(new ViewItem("Team photos.jpg", IconManager::icon(Icons::File_JPG), "JPEG", "6.7 MB", "03 Jan 2026", false, false));
    _model.appendRow(new ViewItem("Fitness tutorial.mp4", IconManager::icon(Icons::File_MP4), "MP4", "380 MB", "02 Jan 2026", false, true));
    _model.appendRow(new ViewItem("Draft manuscript.docx", IconManager::icon(Icons::File_DOC), "DOCX", "150 KB", "01 Jan 2026", false, false));
    _model.appendRow(new ViewItem("Landscape photo.jpg", IconManager::icon(Icons::File_JPG), "JPEG", "7.2 MB", "04 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Tutorial video.mp4", IconManager::icon(Icons::File_MP4), "MP4", "340 MB", "03 Dec 2025", false, true));
    _model.appendRow(new ViewItem("Team report.docx", IconManager::icon(Icons::File_DOC), "DOCX", "112 KB", "02 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Annual calendar.psd", IconManager::icon(Icons::File_PSD), "PSD", "28 MB", "01 Dec 2025", false, false));



    // Layout
    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(4, 4, 4, 4);
    _layout->setSpacing(0);
    _layout->addWidget(view_toolbar_wrapper, 0, Qt::AlignTop);

    _layout->addSpacing(10); // IMP : Seperator is used
    
    _layout->addWidget(_list);
    //_layout->addWidget(empty_state, 0, Qt::AlignCenter);
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

    // View Items Delegate
    _delegate->setDarkMode(isDarkMode);

    // View Items List Scrollbars
    vScroll->setDarkMode(isDarkMode);
    hScroll->setDarkMode(isDarkMode);
    
    update();
}

TextField* View::searchBox() const { return search_box; }
ButtonMenu* View::filterMenu()  const { return filterButtonMenu; }
ViewModeToggle* View::viewMode() const { return view_mode; }
EmptyStateWidget* View::emptyStateWidget() const { return empty_state; }
