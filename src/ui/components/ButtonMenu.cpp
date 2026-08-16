#include "ButtonMenu.h"
#include "Menu.h"

ButtonMenu::ButtonMenu(QWidget *parent) : Button(parent) {
    // Button properties
    setCursor(Qt::PointingHandCursor);
    setSecondary(true);
    setBorderTransparent(true);
    setIconSize(QSize(18, 18));
    setNormalBackgroundTransparent(true);

    // Menu
    _menu = new Menu(this);
    _menu->setIconic(false);

    connect(this, &Button::clicked, this, [this]() {
        _menu->showAt(this);
    });

    connect(_menu, &Menu::itemClicked, this, [this](){
        setText(_menu->clickedItemText());
    });

    setDarkMode(isDarkMode);
}


void ButtonMenu::setDarkMode(bool enable) {
    if (isDarkMode == enable)
        return;

    isDarkMode = enable;

    if  (_menu) _menu->setDarkMode(isDarkMode);
    Button::setDarkMode(isDarkMode);
}

Menu* ButtonMenu::menu() const {
    return _menu;
}
