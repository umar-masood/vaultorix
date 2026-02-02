#pragma once

#include <QWidget>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QPainter>
#include "../../components/Button.h"

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
    Button *grid_view_btn = nullptr;

    // Layout
    QHBoxLayout *layout = nullptr;
};
