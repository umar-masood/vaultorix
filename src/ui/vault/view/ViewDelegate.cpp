#include "ViewDelegate.h"

ViewItem::ViewItem(const QString &title,
                   const QString &thumbnailPath,
                   const QString &type,
                   const QString &sizem,
                   const QString &lastModifiedDate,
                   bool isEncrypted,
                   bool isDecrypted)
{
    setData(title, FileTitle);

    setData(thumbnailPath, FileThumbnail);
    setData(type, FileType);
    setData(sizem, FileSize);
    setData(lastModifiedDate, DateModified);

    setData(isEncrypted, EncryptionStatus);
    setData(isDecrypted, DecryptionStatus);
}

ViewDelegate::ViewDelegate(QObject *parent) :
                    QStyledItemDelegate(parent), 
                    cbSize(20, 20),
                    statusIconSize(16, 16),
                    fileThumbIconSize(40,46),
                    cbPixmap(QPixmap((IconManager::icon(Icons::CheckBox_Check))).scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation)) {}

void ViewDelegate::setDarkMode(bool enable) {
    if (isDarkMode == enable)
        return;

    isDarkMode = enable;
}

void ViewDelegate::setItemHeight(int itemHeight) {
    _itemHeight = itemHeight;
}

void ViewDelegate::setViewMode(ItemsViewMode viewMode) {
    _viewMode = viewMode;
}

QSize ViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (_viewMode == ItemsViewMode::ListMode) 
        return QSize(option.rect.width(), _itemHeight); // List View
    else 
        return QSize(140, 80); // Grid View
}

void ViewDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    // Item Rectangle
    QRect itemRect(option.rect);
    
    // Retrieving current states from data
    bool isSelected = option.state & QStyle::State_Selected;
    bool isMouseOver = option.state & QStyle::State_MouseOver;

    // Main Font
    QFont font("Segoe UI", 10);

    /* ----------------------- Drawing Item Hover ------------------ */
    painter->setPen(Qt::NoPen);
    painter->setBrush(isDarkMode ? "#2D2D2D" : "#F0F0F0");
    if (isSelected || isMouseOver)
        painter->drawRoundedRect(itemRect.adjusted(1, 2, -1, -2), 6, 6);


    /* ----------------- Select Item Check Box -------------------- */ 
    QRect cbRect = selectItemCheckboxRect(itemRect);
    
    // Border
    QPen pen;
    pen.setWidthF(isSelected ? 1.0 : 0.5);
    pen.setColor(isSelected ? QColor("#0191DF") : (isDarkMode ? QColor("#4D4D4D") : QColor("#CCCCCC")));
    pen.setStyle(Qt::SolidLine);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);

    // Background
    QColor brushColor;
    if (isSelected) 
       brushColor = isMouseOver ?   QColor("#1BB3E6") : QColor("#0191DF");
    else if (isMouseOver) 
       brushColor = isDarkMode  ?   QColor("#323232") : QColor("#F0F0F0");
    else 
       brushColor = isDarkMode  ?   QColor("#2D2D2D") : QColor("#FBFBFB");

    painter->setBrush(brushColor);
    painter->drawRoundedRect(cbRect, 6, 6);

    // Pixmap
    if (!cbPixmap.isNull() && isSelected) {
        int xPos = cbRect.x() + (cbRect.width() - cbPixmap.width()) / 2;
        int yPos = cbRect.y() + (cbRect.height() - cbPixmap.height()) / 2;
        painter->drawPixmap(xPos, yPos, cbPixmap);
    }

    /* ----------------- File Thumbnail -----------------*/
    const int thumbW = 120;
    int thumbH = itemRect.height() - 12;
    int thumbX = cbRect.right() + spacing;
    int thumbY = itemRect.y() + (itemRect.height() - thumbH) / 2;

    QRect thumbnailRect(thumbX, thumbY, thumbW, thumbH);

    // Transparent Border
    painter->setPen(Qt::transparent);

    // Background
    brushColor = isDarkMode ? "#1C1C1C" : "#FFFFFF";
    painter->setBrush(brushColor);

    // Drawing Background
    painter->drawRoundedRect(thumbnailRect, 6, 6);

    // Drawing Icon
    QPixmap thumbPixmap = QPixmap(index.data(ItemDelegateRoles::FileThumbnail).toString())
                          .scaled(fileThumbIconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    int thumbIconX = thumbnailRect.x() + (thumbnailRect.width() - fileThumbIconSize.width()) / 2;
    int thumbIconY = thumbnailRect.y() + (thumbnailRect.height() - fileThumbIconSize.height()) / 2;

    painter->drawPixmap(thumbIconX, thumbIconY, thumbPixmap);

    /* ----------------- File Title -------------------- */
    QString fileTitle = index.data(ItemDelegateRoles::FileTitle).toString();
    // Title Rect
    int titleW = itemRect.width() * 0.40;
    int titleH = itemRect.height() - 4;
    int titleX = thumbnailRect.right() + spacing;
    int titleY = itemRect.y();
    QRect titleRect(titleX, titleY, titleW, titleH);

    // Font
    font.setWeight(QFont::Normal);
    painter->setFont(font);

    // Pen Color
    painter->setPen(isDarkMode ? Qt::white : Qt::black);

    // Drawing Text
    painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, fileTitle);

    /* ----------------- File Type Tag ------------ */
    QString type = (index.data(ItemDelegateRoles::FileType).toString()).toUpper();
    
    // Rect
    const int typeH = 20;
    const int typeW = 50;
    int typeX = titleRect.right() + spacing;
    int typeY = itemRect.y() + (itemRect.height() - typeH) / 2;
    QRect typeRect(typeX, typeY, typeW, typeH);

    // Border
    painter->setPen(Qt::NoPen);

    // Background
    painter->setBrush(isDarkMode ? "#1F3A5F" : "#D1E7FF");
    painter->drawRoundedRect(typeRect, 6, 6);
    
    // Font
    font.setWeight(QFont::DemiBold);
    painter->setFont(font);

    // Text Color
    painter->setPen(isDarkMode ? "#8CC1FF" : "#3A7BD5");

    // Drawing Text
    QRect typeTxtRect(typeX + 2, typeRect.y(), typeW - 4, typeH - 2);
    painter->drawText(typeTxtRect, Qt::AlignVCenter | Qt::AlignHCenter, type);

    /* ----------------- File Size ------------ */
    QString fileSize = index.data(ItemDelegateRoles::FileSize).toString().toUpper();

    int sizeX = typeRect.right() + spacing;
    int sizeY = itemRect.y();
    const int sizeW = 60;
    int sizeH = itemRect.height() - 4;

    // Text Rect
    QRect sizeTxtRect(sizeX, sizeY, sizeW, sizeH);

    // Elided Text
    QString elidedFileSizeTxt = QFontMetrics(font).elidedText(fileSize, Qt::ElideRight, sizeW);

    // Text Color
    painter->setPen("#6B7280");

    // Drawing Text
    painter->drawText(sizeTxtRect, Qt::AlignVCenter | Qt::AlignLeft, elidedFileSizeTxt);

    /* ----------------- File Date Modified ------------ */
    QString dateModified = index.data(ItemDelegateRoles::DateModified).toString();

    int dateModifiedX = sizeTxtRect.right() + spacing;
    int dateModifiedY = itemRect.y();
    const int dateModifiedW = 80;
    int dateModifiedH = itemRect.height() - 4;

    // Text Rect
    QRect dateModifiedTxtRect(dateModifiedX, dateModifiedY, dateModifiedW, dateModifiedH);

    // Elided Text
    QString elidedDateModifiedTxt = QFontMetrics(font).elidedText(dateModified, Qt::ElideRight, dateModifiedW);

    // Text Color
    painter->setPen("#6B7280");

    // Drawing Text
    painter->drawText(dateModifiedTxtRect, Qt::AlignVCenter | Qt::AlignLeft, elidedDateModifiedTxt);

    /* ----------------- Encryption/Decryption ------------ */
    bool isEncrypted = (index.data(ItemDelegateRoles::EncryptionStatus)).toBool();
    bool isDecrypted = (index.data(ItemDelegateRoles::DecryptionStatus)).toBool();

    QString statusText;

    // Rect 
    const int statusH = 24;
    const int statusW = 102;
    int statusX = dateModifiedTxtRect.right() + spacing;
    int statusY = itemRect.y() + (itemRect.height() - statusH) / 2;
    QRect statusRect(statusX, statusY, statusW, statusH);

    // Border
    painter->setPen(Qt::NoPen);

    // Background
    if (isEncrypted) 
        brushColor = isDarkMode ? "#0F2F1A" : "#D6FFE8";
    else if (isDecrypted)
        brushColor = isDarkMode ? "#561616" : "#FFD9D9";
    else
        brushColor = isDarkMode ? "#4B3F00" : "#FFF8E1";

    painter->setBrush(brushColor);
    painter->drawRoundedRect(statusRect, 6, 6);
    
    // Icon
    QString currentIconPath;
    if (isEncrypted) 
        currentIconPath = isDarkMode ? IconManager::icon(Icons::EncryptDark) : IconManager::icon(Icons::EncryptLight);
    else if (isDecrypted)
        currentIconPath = isDarkMode ? IconManager::icon(Icons::DecryptDark) : IconManager::icon(Icons::DecryptLight);
    else
        currentIconPath = isDarkMode ? IconManager::icon(Icons::XDark) : IconManager::icon(Icons::XLight);

    QPixmap statusPixmap = QPixmap(currentIconPath).scaled(QSize(16, 16), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    int statusIconX = statusRect.x() + 8;
    int statusIconY = statusRect.y() + (statusRect.height() - statusIconSize.height()) / 2;
    painter->drawPixmap(statusIconX, statusIconY, statusPixmap);
    
    // Font
    font.setWeight(QFont::DemiBold);
    painter->setFont(font);

    // Text Color
    QColor textColor;
    if (isEncrypted) {
        textColor = isDarkMode ? "#44FF73" : "#0ACD3B";
        statusText = "Encrypted";
    } else if (isDecrypted) {
        textColor = isDarkMode ? "#FF8989" : "#C71010";
        statusText = "Decrypted";
    } else {
        textColor = isDarkMode ? "#FCD34D" : "#B45309";
        statusText = "Not Safe";
    }

    painter->setPen(textColor);

    // Text Rect
    int statusTxtX = statusIconX + statusIconSize.width() + 8;
    int statusTxtY = statusRect.y();
    const int statusTxtW = 62;
    int statusTxtH = statusH - 2;

    QRect statusTxtRect(statusTxtX, statusTxtY, statusTxtW, statusTxtH);

    // Drawing Text
    painter->drawText(statusTxtRect, Qt::AlignVCenter | Qt::AlignLeft, statusText);

    painter->restore();
}

QRect ViewDelegate::selectItemCheckboxRect(const QRect &itemRect) const {
    int cbX = itemRect.left() + spacing;
    int cbY = itemRect.y() + (itemRect.height() - cbSize.height()) / 2;

    return QRect(cbX, cbY, cbSize.width(), cbSize.height());
}
