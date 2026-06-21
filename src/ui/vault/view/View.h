#pragma once

#include <QWidget>
#include <QStandardItemModel>

class ViewModeToggle;
class TextField;
class ButtonMenu;
class ScrollBar;
class QHBoxLayout;
class QVBoxLayout;
class QListView;
class QPainter;
class QResizeEvent;
class QPaintEvent;
class QSortFilterProxyModel;

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
        QStandardItemModel& model();
        QSortFilterProxyModel* proxyModel() const;
        QListView *list() const;

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

        // View Mode Toggle
        ViewModeToggle *view_mode = nullptr;

        // Filter Button Menu
        ButtonMenu *filterButtonMenu = nullptr;

        // Icons
        QString FilterIcon, ArrowDownIcon, SearchIcon;   

        // Search Box
        TextField *search_box = nullptr;
        QSortFilterProxyModel *proxy_model;

        // Items View List
        QListView *_list = nullptr;

        // Model
        QStandardItemModel _model;

        // Scroll Bar
        ScrollBar *vScroll = nullptr;

        // Items View Delegate
        Ui::Vault::ViewDelegate *_delegate = nullptr;

        // Helpers
        void updateEmptyState();
        void updateEmptyStatePosition();
        void updateGridLayout();

        private slots:
        void onListViewModeSelected();
        void onGridViewModeSelected();
    };
};