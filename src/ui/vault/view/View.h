#pragma once

#include "../emptyStateWidget/EmptyStateWidget.h"
#include "../../components/TextField.h"
#include "../../components/ButtonMenu.h"
#include "../../components/ViewModeToggle.h"
#include "../../../resources/IconManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>

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

    private:
    // Flags
    bool isDarkMode = false;

    // Empty State Widget
    EmptyStateWidget *empty_state = nullptr;

    // Action Toolbar Layout
    QHBoxLayout *actionToolbarLayout = nullptr;

    // Seperator Color
    QColor separatorColor;
    int separatorHeight = 2;

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
    
    // Main Layout
    QVBoxLayout *_layout = nullptr;
};