#pragma once

#include "Button.h"
#include "Menu.h"

#include <QStandardItemModel>
#include <QVector>
#include <QModelIndex>
#include <QEvent>
#include <QApplication>
#include <QListView>
#include <QMouseEvent>
#include <QScreen>
#include <algorithm>

class ButtonMenu : public Button {
    Q_OBJECT

    public:
    explicit ButtonMenu(QWidget *parent = nullptr);
    void setDarkMode(bool enable);

    Menu* menu() const;

    private:
    // Theme flags
    bool isDarkMode = false;

    // Menu
    Menu *_menu = nullptr;
};
