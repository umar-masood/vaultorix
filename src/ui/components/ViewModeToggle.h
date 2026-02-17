#pragma once

#include <QWidget>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QPainter>

#include "ToolTip.h"
#include "Button.h"

class ViewModeToggle : public QWidget {
    Q_OBJECT

    public:
    explicit ViewModeToggle(QWidget *parent = nullptr);
    void setDarkMode(bool enable);

    protected:
    void paintEvent(QPaintEvent *event) override;

    signals:
    void list_view_selected();
    void grid_view_selected();
    
    private:
    Button* createButton();

    // Theme Flag
    bool isDarkMode = true;

    // Button Group
    QButtonGroup *group = nullptr;

    // Buttons
    Button *list_view_btn = nullptr;
    // ToolTip
    ToolTip *list_view_tip = nullptr;

    Button *grid_view_btn = nullptr;
    // ToolTip
    ToolTip *grid_view_tip = nullptr;

    // Icons
    QString ListViewIcon, ListViewCheckedIcon, GridViewIcon, GridViewCheckedIcon; 

    // Layout
    QHBoxLayout *layout = nullptr;
};
