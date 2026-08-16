#pragma once

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>

class Label;
class Seperator;
class QHBoxLayout;

namespace Ui::Vault {
    /* ---------------------  Statusbar ---------------------- */
    class Statusbar : public QWidget {
        Q_OBJECT

        public:
        Statusbar(QWidget *parent = nullptr);

        Label* totalItemsLabel() const;
        Label* selectedItemsLabel() const;
        Label* totalSizeLabel() const;

        private:
        void setDarkMode(bool isDarkMode);

        // Total Items 
        Label *total_items = nullptr;

        // Seperator
        Seperator *seperator = nullptr;

        // Selected Items
        Label *selected_items = nullptr;

        // Total Size 
        Label *total_size = nullptr;

        // Main Layout
        QHBoxLayout *layout = nullptr;

        // Helper
        Label* createLabel(const QString &text = "");

        protected:
        void paintEvent(QPaintEvent *event) override;
    };
};