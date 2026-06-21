#pragma once

#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>
#include <QStandardItem>
#include <algorithm>
#include <QtMath>
#include <QPixmap>
#include <QRect>
#include <QSize>
#include <QUuid>

#include "../../../core/services/vault/models/FileMetadata.h"
#include "../../../core/services/vault/models/FileStatus.h"

class QIcon;
class QTimer;

namespace Ui::Vault {
    enum ViewItemRoles {
        FileTitle               =   Qt::DisplayRole,
        FileThumbnail           =   Qt::UserRole + 1,
        FileType ,               
        FileSize,                
        FileCreatedAt,            
        FileStatus,              
        FileId,
        ProgressCurrentValue,                
    };

    enum ViewItemsMode {
        ListMode = 0x1,
        GridMode = 0x2,
    };

    class ViewItem : public QStandardItem {
        public:
        ViewItem(const Core::Vault::FileMetadata &metadata);
    };

    class ViewDelegate : public QStyledItemDelegate {
        Q_OBJECT

        public:
        explicit ViewDelegate(QObject *parent = nullptr);
        void setDarkMode(bool enable);
        void setItemHeight(int itemHeight);
        void setViewMode(ViewItemsMode mode);

        protected:
        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

        private:
        // Theme Mode
        bool isDarkMode = false;

        // Items Size
        int _itemHeight;

        // Items View Mode
        ViewItemsMode _viewMode;

        // Spacing
        const int spacing = 30;

        // Select Item Checkbox
        QSize cbSize;
        QPixmap cbPixmap;
        QRect selectItemCheckboxRect(const QRect &itemRect) const;

        // Encryption Status Icon
        QSize statusIconSize;
    };
};