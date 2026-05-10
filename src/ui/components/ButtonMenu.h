#pragma once

#include "Button.h"

class Menu;

class ButtonMenu : public Button {
    Q_OBJECT

    public:
    explicit ButtonMenu(QWidget *parent = nullptr);
    void setDarkMode(bool enable);
    Menu* menu() const;

    private:
    // Theme flag
    bool isDarkMode = false;

    // Menu
    Menu *_menu = nullptr;
};
