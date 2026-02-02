#pragma once

#include "../emptyStateWidget/EmptyStateWidget.h"
#include "../../components/TextField.h"
#include "../../components/ButtonMenu.h"
#include "../../components/ViewModeToggle.h"

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
    const QString filterIcon   = ":/icons/vaultWindow/filter.svg";
    const QString dropdownIcon = ":/icons/components/arrow-down.svg";

    // Search Box
    TextField *search_box = nullptr;
    
    // Main Layout
    QVBoxLayout *_layout = nullptr;
};