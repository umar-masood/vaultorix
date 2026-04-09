#pragma once

#include "../empty_state/EmptyState.h"
#include "../../components/ViewModeToggle.h"
#include "../../components/TextField.h"
#include "../../components/ButtonMenu.h"
#include "../../components/ScrollBar.h"

#include "ViewDelegate.h"

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

    TextField* searchBox() const;
    ButtonMenu* filterMenu() const;
    ViewModeToggle* viewMode() const;
    EmptyState* emptyStateWidget() const; 

    protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    private:
    void setDarkMode(bool isDarkMode);

    // Empty State Widget
    EmptyState *empty_state = nullptr;

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
    QString FilterIcon, ArrowDownIcon, SearchIcon;   

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