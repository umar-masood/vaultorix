#include "View.h"
#include "ViewDelegate.h"

#include "../../../core/theme/ThemeManager.h"
#include "../../../core/config/Constants.h"

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
#include <QFileDialog>
#include <QStandardPaths>
#include <QFileInfo>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QSortFilterProxyModel>

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

    auto menu = filterButtonMenu->menu();
    menu->setItemSize(QSize(180, 36));
    menu->delegate()->setSelectionCheckIndicator(true);
    menu->setIconic(true);
    menu->addAction({"All file types",false, "", IconManager::icon(Icons::Files), IconManager::icon(Icons::Files)});
    menu->addAction({"Document", false, "",  IconManager::icon(Icons::Document), IconManager::icon(Icons::Document)});
    menu->addAction({"Picture", false, "",  IconManager::icon(Icons::Picture), IconManager::icon(Icons::Picture)});
    menu->addAction({"Music", false, "",  IconManager::icon(Icons::Music), IconManager::icon(Icons::Music)});
    menu->addAction({"Video", false, "",  IconManager::icon(Icons::Video), IconManager::icon(Icons::Video)});
    menu->addAction({"Application", false, "",  IconManager::icon(Icons::Application), IconManager::icon(Icons::Application)});
    menu->addAction({"Archive", false, "",  IconManager::icon(Icons::Archive), IconManager::icon(Icons::Archive)});
    menu->addAction({"Other", false, "",  "", ""});
    menu->delegate()->setActiveIndex(filterButtonMenu->menu()->itemIndex("All file types")); // Setting Default one

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
    auto *wrapper_layout = new QHBoxLayout(view_toolbar_wrapper);
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
    _list->setSelectionMode(QAbstractItemView::SingleSelection);
    _list->setFrameShape(QFrame::NoFrame);
    _list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    _list->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    _list->setVerticalScrollBar(vScroll);
    _list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _list->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    _list->setStyleSheet("background: transparent; margin: 0px; padding: 0px; border:none; ");

    onListViewModeSelected();

    connect(view_mode, &ViewModeToggle::ListViewModeSelected, this, &View::onListViewModeSelected);
    connect(view_mode, &ViewModeToggle::GridViewModeSelected, this, &View::onGridViewModeSelected);

    // Filter Proxy Model
    proxy_model = new QSortFilterProxyModel(this);
    proxy_model->setSourceModel(&_model);
    proxy_model->setFilterRole(ViewItemRoles::FileTitle);
    proxy_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy_model->setDynamicSortFilter(true);
    _list->setModel(proxy_model);
    connect(search_box, &TextField::textChanged, this, [this](const QString &text){
        proxy_model->setFilterFixedString(text.trimmed());
    });

    // Layout
    auto *_layout = new QVBoxLayout(this);
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

    // Empty State Widget
    updateEmptyStatePosition();
    updateEmptyState();
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
        _delegate->setViewMode(ViewItemsMode::ListMode);
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
        _delegate->setViewMode(ViewItemsMode::GridMode);
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
}

void View::updateEmptyState() {
    bool isEmpty = (_model.rowCount() == 0);

    if (empty_state) {
        empty_state->setVisible(isEmpty);

        if (isEmpty)
            empty_state->raise();
    }
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

QListView* View::list() const { return _list; }
QStandardItemModel& View::model() { return _model; }
QSortFilterProxyModel * Ui::Vault::View::proxyModel() const { return proxy_model; }
TextField* View::searchBox() const { return search_box; }
ButtonMenu* View::filterMenu()  const { return filterButtonMenu; }
ViewModeToggle* View::viewMode() const { return view_mode; }
Ui::Vault::EmptyState* View::emptyStateWidget() const { return empty_state; }