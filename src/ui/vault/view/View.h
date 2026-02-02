#pragma once

#include "../emptyStateWidget/EmptyStateWidget.h"
#include "../../components/TextField.h"
#include "../../components/ComboBox.h"
#include "../../components/Button.h"
#include "../../components/Popup.h"
#include "./ViewModeToggle.h"
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

    // View Mode Toggle
    ViewModeToggle *view_mode = nullptr;

    // Search Box
    TextField *search_box = nullptr;
    
    // Main Layout
    QVBoxLayout *layout = nullptr;
};