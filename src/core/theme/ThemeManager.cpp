#include "ThemeManager.h"
#include "../config/Constants.h"

ThemeManager& ThemeManager::instance() {
    static ThemeManager instance;
    return instance;
}

bool ThemeManager::isDarkMode() const {
    return _isDarkMode;
}

void ThemeManager::setDarkMode(bool value) {
    if (_isDarkMode == value)
        return;

    _isDarkMode = value;
    
    emit themeChanged(isDarkMode());

    // Saving current theme
    saveCurrentTheme();
}

ThemeManager::ThemeManager(){
    // Applying saved current theme
    setSavedCurrentTheme();
}

void ThemeManager::saveCurrentTheme() {
    QString value;

    if (_isDarkMode)
        value = "Dark";
    else 
        value = "Light";

    settings.setValue(DEVICE_THEME_KEY, value);
    settings.sync();
}

void ThemeManager::setSavedCurrentTheme() {
    if (!settings.contains(DEVICE_ID_SETTINGS_KEY))
        return;
    
    QString value = settings.value(DEVICE_THEME_KEY).toString();

    if (value == "Dark") 
        setDarkMode(true);
    else
        setDarkMode(false);
}