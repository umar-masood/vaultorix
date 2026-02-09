#pragma once

#include "../emptyStateWidget/EmptyStateWidget.h"
#include "../../components/TextField.h"
#include "../../components/ButtonMenu.h"
#include "../../components/ViewModeToggle.h"
#include "../../../resources/IconManager.h"
#include "./ViewDelegate.h"
#include "../../components/ScrollBar.h"

#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>

class View : public QWidget {
    Q_OBJECT

    public:
    explicit View(QWidget *parent = nullptr);
    void setDarkMode(bool enable);

    TextField* searchBox() const;
    ButtonMenu* filterMenu() const;
    ViewModeToggle* viewMode() const;
    EmptyStateWidget* emptyStateWidget() const; 

    protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    private:
    // Flags
    bool isDarkMode = false;

    // Empty State Widget
    EmptyStateWidget *empty_state = nullptr;

    // Seperator Color, Height
    QColor separatorColor;
    int separatorHeight = 2;

    // Wrapper (View Action Toolbar)
    QWidget *view_toolbar_wrapper = nullptr;
    // Wrapper Action Toolbar Layout
    QHBoxLayout *wrapper_layout = nullptr;

    // View Mode Toggle
    ViewModeToggle *view_mode = nullptr;

    // Filter Button Menu
    ButtonMenu *filterButtonMenu = nullptr;

    // Icons
    const QString FilterIcon    = IconManager::icon(Icons::Filter);
    const QString ArrowDownIcon = IconManager::icon(Icons::ArrowDown);
    const QString SearchIcon    = IconManager::icon(Icons::Search);

    // Search Box
    TextField *search_box = nullptr;
    
    // Items View List
    QListView *_list = nullptr;

    // Model
    QStandardItemModel _model;
    
    // Scroll Bars
    ScrollBar *vScroll = nullptr;

    // Items View Delegate
    ViewDelegate *_delegate = nullptr;

    // Main Layout
    QVBoxLayout *_layout = nullptr;

    QTimer *progressTimer = nullptr;


    // Helpers
    void updateEmptyState();
    void updateEmptyStatePosition();
    void updateGridLayout();

    // Slots
    void onListViewModeSelected();
    void onGridViewModeSelected();
};