#pragma once
#include <QObject>
#include <QSettings>

class ThemeManager : public QObject {
    Q_OBJECT

    public:
    static ThemeManager& instance();

    bool isDarkMode() const;
    void setDarkMode(bool value);

    signals:
    void themeChanged(bool isDarkMode);

    private:
    ThemeManager();

    void saveCurrentTheme();
    void setSavedCurrentTheme();

    bool _isDarkMode = false;
    QSettings settings;
};