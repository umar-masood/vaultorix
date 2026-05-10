#pragma once


#include "../../../resources/IconManager.h"
#include "TextField.h"

#include <QVector>
#include <QString>
#include <QStandardItemModel>
#include <QAbstractItemView>

class QObject;
class QEvent;
class QPoint;
class QScreen;
class QKeyEvent;
class QModelIndex;
class QMouseEvent;
class QContextMenuEvent;
class QResizeEvent;

class QStandardItem;
class QCompleter;
class QListView;
class QVBoxLayout;

class Button;
class SmoothOpacity;
class SmoothShadow;
class RoundedBox;
class Delegate;
class ScrollBar;
class Popup;

class ComboBox : public TextField {
    Q_OBJECT

    public:
    explicit ComboBox(QWidget *parent = nullptr);

    struct ComboItem { 
        QString text; 
        QString lightIcon = ""; 
        QString darkIcon = ""; 
    };

    void setIconic(bool value);
    void setEditable(bool value);
    void setDarkMode(bool value);
    void setCurrentItem(int index);
    void setFieldSize(const QSize &fieldSize);
    void setPopupWidth(int width);

    void addItem(const QString &text, const QString &lightIcon = "", const QString &darkIcon = "");
    void addItems(const QVector<ComboItem> comboItems);

    void deleteItem(int index);
    void clearAll();

    QString currentText() const;
    int currentIndex() const;

    void setMaxVisibleItems(int items);
    int maxVisibleItems() const;

    private slots:
    void onDropDownButtonClicked();
    void onComboItemClicked(const QModelIndex &index);

    protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event);

    private:
    QVector<ComboItem> items;

    void init();
    void positionDropDownButton();
    void positionPopup();
    void updateItemIcons();
    void setDropDownButton();

    // Flags
    bool isEditable = false;
    bool isIconic = false;
    bool isDarkMode = false;

    // Max Visible Items
    int _maxVisibleItems = 0;
  
    // Dropdown Button
    Button *dropdown = nullptr;
    const QString ArrowDown = IconManager::icon(Icons::ArrowDown);

    // Rounded Popup
    Popup *popup = nullptr;

    // List & Delegate for data items inside Popup
    Delegate *delegate = nullptr;
    QStandardItemModel model;

    // Auto Completer
    QCompleter *completer = nullptr;
};