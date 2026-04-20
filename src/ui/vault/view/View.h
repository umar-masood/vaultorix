#pragma once

#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QResizeEvent>

class ViewModeToggle;
class TextField;
class ButtonMenu;
class ScrollBar;

namespace Ui::Vault {
    class EmptyState;
    class ViewDelegate;
};

namespace Ui::Vault {
    class View : public QWidget {
        Q_OBJECT

        public:
        explicit View(QWidget *parent = nullptr);

        TextField* searchBox() const;
        ButtonMenu* filterMenu() const;
        ViewModeToggle* viewMode() const;
        Ui::Vault::EmptyState* emptyStateWidget() const; 

        protected:
        void paintEvent(QPaintEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;

        private:
        void setDarkMode(bool isDarkMode);

        // Empty State Widget
        Ui::Vault::EmptyState *empty_state = nullptr;

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
        Ui::Vault::ViewDelegate *_delegate = nullptr;

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
};