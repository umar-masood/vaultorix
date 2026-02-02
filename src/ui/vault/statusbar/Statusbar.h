#pragma once
#include "../../components/Label.h"
#include "../../components/Seperator.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QPaintEvent>
#include <QPainter>

/* ---------------------  Statusbar ---------------------- */
class Statusbar : public QWidget {
    Q_OBJECT

    public:
    Statusbar(QWidget *parent = nullptr);
    void setDarkMode(bool enable);

    Label* totalItemsLabel() const;
    Label* selectedItemsLabel() const;
    Label* totalSizeLabel() const;

    private:
    // Theme Mode
    bool isDarkMode = false;

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