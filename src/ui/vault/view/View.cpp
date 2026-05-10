#include "View.h"
#include "ViewDelegate.h"

#include "../../../core/theme/ThemeManager.h"

#include "../../../../resources/IconManager.h"
#include "../empty_state/EmptyState.h"

#include "../../components/ViewModeToggle.h"
#include "../../components/TextField.h"
#include "../../components/ButtonMenu.h"
#include "../../components/ScrollBar.h"
#include "../../components/Delegate.h"
#include "../../components/Menu.h"

#include <QTimer>
#include <QRandomGenerator>
#include <QListView>
#include <QVBoxLayout>
#include <QHBoxLayout>

using Ui::Vault::View;
using Ui::Vault::ViewItem;

View::View(QWidget *parent) : QWidget(parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  
    // Icons
    FilterIcon    = IconManager::icon(Icons::Filter);
    ArrowDownIcon = IconManager::icon(Icons::ArrowDown);
    SearchIcon    = IconManager::icon(Icons::Search);

    // Empty State Widget
    empty_state = new Ui::Vault::EmptyState(this);
    empty_state->hide();

    // Menu Button (for testing)
    filterButtonMenu = new ButtonMenu(this);
    filterButtonMenu->setDisplayMode(Button::IconText);
    filterButtonMenu->setIconPaths(FilterIcon, FilterIcon);
    filterButtonMenu->setRightSideIcon(ArrowDownIcon, ArrowDownIcon);
    filterButtonMenu->setFontProperties("Segoe UI", 10);
    filterButtonMenu->setText(tr("Filter"));
    filterButtonMenu->setFontXY(0, -1);
    filterButtonMenu->menu()->setItemSize(QSize(180, 36));
    filterButtonMenu->menu()->delegate()->setSelectionDotIndicator(true);
    filterButtonMenu->menu()->setIconic(true);
    filterButtonMenu->menu()->addAction({"All file types",false, "", IconManager::icon(Icons::Files), IconManager::icon(Icons::Files)});
    filterButtonMenu->menu()->addAction({"Documents", false, "",  IconManager::icon(Icons::Document), IconManager::icon(Icons::Document)});
    filterButtonMenu->menu()->addAction({"Pictures", false, "",  IconManager::icon(Icons::Picture), IconManager::icon(Icons::Picture)});
    filterButtonMenu->menu()->addAction({"Music", false, "",  IconManager::icon(Icons::Music), IconManager::icon(Icons::Music)});
    filterButtonMenu->menu()->addAction({"Videos", false, "",  IconManager::icon(Icons::Video), IconManager::icon(Icons::Video)});
    filterButtonMenu->menu()->delegate()->setActiveIndex(filterButtonMenu->menu()->itemIndex("All file types")); // Setting Default one

    // Search Box
    search_box = new TextField;
    search_box->setPlaceholderText(tr("Type to search your files…"));
    search_box->setFixedSize(QSize(400, 32));
    search_box->setClearButton(true);
    search_box->setIconic(true);
    search_box->setIconSize(QSize(18, 18));
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
    _delegate = new Ui::Vault::ViewDelegate;
    
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
    _model.appendRow(new ViewItem("Music.mp3",  IconManager::icon(Icons::File_MP3),  "MP3",  "5.4 MB", "09 Feb 2026", false, false));
    _model.appendRow(new ViewItem("Video.mp4",  IconManager::icon(Icons::File_MP4),  "MP4",  "88 MB",  "08 Feb 2026", false, false));
    _model.appendRow(new ViewItem("Meeting notes.txt", IconManager::icon(Icons::File_TXT), "TXT", "12 KB", "07 Feb 2026", true, false));
    _model.appendRow(new ViewItem("Project overview.pdf", IconManager::icon(Icons::File_PDF), "PDF", "1.1 MB", "06 Feb 2026", false, false));
    _model.appendRow(new ViewItem("Travel itinerary.pdf", IconManager::icon(Icons::File_PDF), "PDF", "380 KB", "22 Jan 2026", false, false));
    _model.appendRow(new ViewItem("Old project code backup.zip", IconManager::icon(Icons::File_ZIP), "ZIP", "720 MB", "21 Jan 2026", true, false));
    _model.appendRow(new ViewItem("Company logo vector.ai", IconManager::icon(Icons::File_AI), "AI", "3.3 MB", "20 Jan 2026", false, false));
    _model.appendRow(new ViewItem("Personal journal entry.txt", IconManager::icon(Icons::File_TXT), "TXT", "8 KB", "19 Jan 2026", true, false));
    _model.appendRow(new ViewItem("Birthday celebration photos.jpg", IconManager::icon(Icons::File_JPG), "JPEG", "5.8 MB", "18 Jan 2026", false, true));
    _model.appendRow(new ViewItem("Meeting agenda.txt", IconManager::icon(Icons::File_TXT), "TXT", "4 KB", "05 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Landscape photography.jpg", IconManager::icon(Icons::File_JPG), "JPEG", "7.2 MB", "04 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Tutorial video.mp4", IconManager::icon(Icons::File_MP4), "MP4", "340 MB", "03 Dec 2025", false, true));
    _model.appendRow(new ViewItem("Meeting notes.txt", IconManager::icon(Icons::File_TXT), "TXT", "12 KB", "07 Feb 2026", true, false));
    _model.appendRow(new ViewItem("Team photos.jpg", IconManager::icon(Icons::File_JPG), "JPEG", "6.7 MB", "03 Jan 2026", false, false));
    _model.appendRow(new ViewItem("Fitness tutorial.mp4", IconManager::icon(Icons::File_MP4), "MP4", "380 MB", "02 Jan 2026", false, true));
    _model.appendRow(new ViewItem("Draft manuscript.docx", IconManager::icon(Icons::File_DOC), "DOCX", "150 KB", "01 Jan 2026", false, false));
    _model.appendRow(new ViewItem("Landscape photo.jpg", IconManager::icon(Icons::File_JPG), "JPEG", "7.2 MB", "04 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Tutorial video.mp4", IconManager::icon(Icons::File_MP4), "MP4", "340 MB", "03 Dec 2025", false, true));
    _model.appendRow(new ViewItem("Team report.docx", IconManager::icon(Icons::File_DOC), "DOCX", "112 KB", "02 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Annual calendar.psd", IconManager::icon(Icons::File_PSD), "PSD", "28 MB", "01 Dec 2025", false, false));
    _model.appendRow(new ViewItem("Notes.txt",  IconManager::icon(Icons::File_TXT),  "TXT",  "3 KB",   "12 Feb 2026", false, false));
    _model.appendRow(new ViewItem("Resume.pdf", IconManager::icon(Icons::File_PDF),  "PDF",  "210 KB", "11 Feb 2026", false, false));
    _model.appendRow(new ViewItem("Photo.jpg",  IconManager::icon(Icons::File_JPG),  "JPEG", "1.9 MB", "10 Feb 2026", false, true));

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

    // Theme
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &View::setDarkMode);
    setDarkMode(tm.isDarkMode());

    // Positioning Empty State
    updateEmptyStatePosition();


    //  QTimer *timer = new QTimer(this);

    // connect(timer, &QTimer::timeout, this, [this]() {

    //     if (_model.rowCount() == 0)
    //         return;

    //     // Pick random row
    //     int randomRow = QRandomGenerator::global()->bounded(_model.rowCount());
    //     QModelIndex index = _model.index(randomRow, 0);

    //     // Get current progress
    //     int currentProgress = index.data(ItemDelegateRoles::ProgressCurrentValue).toInt();

    //     // If not started, initialize it
    //     if (!index.data(ItemDelegateRoles::ShowProgress).toBool()) {

    //         _model.setData(index, true, ItemDelegateRoles::ShowProgress);

    //        // Randomly pick one operation: 0=Encrypt, 1=Decrypt, 2=Restore, 3=Delete, 4=Import
    //         int op = QRandomGenerator::global()->bounded(5);
                    
    //         // Reset all flags first
    //         _model.setData(index, false, ItemDelegateRoles::EncryptionStatus);
    //         _model.setData(index, false, ItemDelegateRoles::DecryptionStatus);
    //         _model.setData(index, false, ItemDelegateRoles::RestoreStatus);
    //         _model.setData(index, false, ItemDelegateRoles::DeleteStatus);
    //         _model.setData(index, false, ItemDelegateRoles::ImportStatus);
                    
    //         switch(op) {
    //             case 0: _model.setData(index, true, ItemDelegateRoles::EncryptionStatus); break;
    //             case 1: _model.setData(index, true, ItemDelegateRoles::DecryptionStatus); break;
    //             case 2: _model.setData(index, true, ItemDelegateRoles::RestoreStatus); break;
    //             case 3: _model.setData(index, true, ItemDelegateRoles::DeleteStatus); break;
    //             case 4: _model.setData(index, true, ItemDelegateRoles::ImportStatus); break;
    //         }
    //         return;
    //     }

    //     // Increase progress randomly
    //     int step = QRandomGenerator::global()->bounded(5, 15);
    //     currentProgress += step;

    //     if (currentProgress >= 100) {
    //         currentProgress = 100;

    //         // Stop progress when complete
    //         _model.setData(index, false, ItemDelegateRoles::ShowProgress);
    //         _model.setData(index, false, ItemDelegateRoles::EncryptionStatus);
    //         _model.setData(index, false, ItemDelegateRoles::DecryptionStatus);
    //         _model.setData(index, false, ItemDelegateRoles::RestoreStatus);
    //         _model.setData(index, false, ItemDelegateRoles::DeleteStatus);
    //         _model.setData(index, false, ItemDelegateRoles::ImportStatus);
    //     }

    //     _model.setData(index, currentProgress, ItemDelegateRoles::ProgressCurrentValue);

        // Force repaint
        // _list->update(index);
    // });

    // timer->start(100); // update every 300ms
}

void View::updateGridLayout() {
    if (!_list || _list->viewMode() != QListView::IconMode)
        return;

    int viewportWidth = _list->viewport()->width();
    if (viewportWidth <= 0)
        return;
    
    const int minItemWidth = 258;
    const int itemHeight = 210;
    int spacing = _list->spacing();

    int cols = qMax(1, viewportWidth / (minItemWidth + spacing)); // tells us how many cols of specificed width can be inserted
    int totalSpacing = spacing * (cols - 1); // there will be no space after the last column;
    int finalItemW = (viewportWidth - totalSpacing) / cols; // ditributing equally width to all cols

    _list->setGridSize(QSize(finalItemW, itemHeight));
}

void View::onListViewModeSelected() {
    if (_delegate) {
        _delegate->setViewMode(ItemsViewMode::ListMode);
        _delegate->setItemHeight(70);
    }

    if (_list) {
        _list->setGridSize(QSize());
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
        _list->setSpacing(2);
        _list->setUniformItemSizes(true);  
        _list->setResizeMode(QListView::Adjust);
        _list->setWrapping(true);  
        _list->setDragEnabled(false); 
        _list->setAcceptDrops(false);          
        _list->setDragDropMode(QAbstractItemView::NoDragDrop);  
        _list->setDefaultDropAction(Qt::IgnoreAction); 
        updateGridLayout();
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
   updateGridLayout();
   QWidget::resizeEvent(event);
}

void View::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // Background
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(ThemeManager::instance().isDarkMode() ? "#262626" : "#F9F9F9"));
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 6, 6);

    // Seperator
    painter.setPen(QPen(separatorColor, separatorHeight));
    painter.drawLine(0, 44, width(), 44);
}

void View::setDarkMode(bool isDarkMode) {
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
Ui::Vault::EmptyState* View::emptyStateWidget() const { return empty_state; }