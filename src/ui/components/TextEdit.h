#pragma once

#include <QTextEdit>
#include <QPainter>
#include <QFocusEvent>
#include <QEnterEvent>
#include <QMap>
#include <QContextMenuEvent>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#include "Menu.h"
#include "ScrollBar.h"

class TextEdit : public QTextEdit {
    Q_OBJECT

    public:
    explicit TextEdit(QWidget *parent = nullptr);

    void setDarkMode(bool enable);
    void setBorderRadius(int radius);
    void setPadding(int padding);

    protected:
    void paintEvent(QPaintEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    private:
    void loadDefaultColors();
    void updateStyle();

    private:
    // States
    bool isDarkMode = false;
    bool isFocused = false;
    bool isHover   = false;

    // Scroll Bar
    ScrollBar *scroll_bar = nullptr;
    int borderRadius = 6;

    // Context Menu
    Menu *menu = nullptr;

    // Icons
    QString CopyIcon , CutIcon, PasteIcon , DeleteIcon , SelectAllIcon , UndoIcon , RedoIcon , ShowIcon , HideIcon, ClearIcon;

    // Colors
    QMap<QString, QColor> colors;
};