#pragma once

#include "Button.h"
#include "TextField.h"
#include "SmoothShadow.h"
#include "SmoothOpacity.h"
#include "RoundedBox.h"
#include "ScrollBar.h"
#include "Delegate.h"
#include <QObject>
#include <QWidget>
#include <QEvent>
#include <QApplication>
#include <QPoint>
#include <QSize>
#include <QRect>
#include <QScreen>
#include <QTimer>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsEffect>
#include <QListView>
#include <QAbstractItemView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QCompleter>
#include <QVBoxLayout>
#include <QFrame>
#include <QIcon>
#include <QStringList>
#include <algorithm>

class ComboBox : public TextField {
    Q_OBJECT

public:
    ComboBox(QWidget *parent = nullptr);

    void setIconic(bool value);
    void setEditable(bool value);
    void setDarkMode(bool value);
    void setDropDownButton();
    void addItems(QStringList items);
    void addIcons(QStringList Light = {}, QStringList dark = {});
    void Popup();
    void updatePopupListHeight();
    void deleteItem(int index);
    QString currentText();
    int currentIndex();
    void setCurrentItem(int index);
    void updateModel();
    void clearAll();
    void popupPos();
    void setSize(QSize s);
    void positionDropDownButton();
    void setPlaceholderText(const QString &text);
    
    void setMaxVisibleItems(int items);
    int getMaxVisibleItems();

    void fadeInAnimation();
    void fadeOutAnimation();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    bool isEditable = false;
    bool isIconic = false;
    bool isDarkMode = false;

    int maxVisibleItems = 0;

    Button *dropdown = nullptr;
    RoundedBox *popup = nullptr;
    QListView *list = nullptr;
    Delegate *d = nullptr;
    QStandardItemModel model;
    QVBoxLayout *layout = nullptr;

    QStringList _items;
    QStringList _itemsIconsLight;
    QStringList _itemsIconsDark;

    QCompleter *completer = nullptr;

    QPropertyAnimation *animation = nullptr;

    ScrollBar *vScroll = nullptr;
    ScrollBar *hScroll = nullptr;
};
