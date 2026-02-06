#pragma once

#include <QStyledItemDelegate>
#include <QPixmap>
#include <QPainter>
#include <QRect>
#include <QModelIndex>
#include <QIcon>
#include <QStandardItem>

#include "../../../resources/IconManager.h"

enum ItemDelegateRoles {
    FileTitle           =   Qt::DisplayRole,
    FileThumbnail       =   Qt::UserRole + 1,
    FileType            =   Qt::UserRole + 2,
    FileSize            =   Qt::UserRole + 3,
    DateModified        =   Qt::UserRole + 4,
    EncryptionStatus    =   Qt::UserRole + 5,
    DecryptionStatus    =   Qt::UserRole + 6,
};

enum ItemsViewMode {
    ListMode = 0x1,
    GridMode = 0x2,
};

class ViewItem : public QStandardItem {
    public:
    ViewItem(const QString &title, 
            const QString &thumbnailPath,
            const QString &type,
            const QString &sizem,
            const QString &lastModifiedDate,
            bool isEncrypted = false,
            bool isDecrypted = false);
};

class ViewDelegate : public QStyledItemDelegate {
    Q_OBJECT

    public:
    explicit ViewDelegate(QObject *parent = nullptr);
    void setDarkMode(bool enable);
    void setItemHeight(int itemHeight);
    void setViewMode(ItemsViewMode mode);

    protected:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    private:
    // Theme Mode
    bool isDarkMode = false;
    
    // Items Size
    int _itemHeight;

    // Items View Mode
    ItemsViewMode _viewMode;

    // Spacing
    const int spacing = 30;

    // Select Item Checkbox
    QSize cbSize;
    QPixmap cbPixmap;
    QRect selectItemCheckboxRect(const QRect &itemRect) const;

    // Encryption Status Icon
    QSize statusIconSize;

    // File Thumbnail Icon
    QSize fileThumbIconSize;
};