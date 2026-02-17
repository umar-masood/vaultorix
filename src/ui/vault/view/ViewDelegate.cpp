#include "ViewDelegate.h"
#include "../../../resources/IconManager.h"

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
                    _viewMode(ItemsViewMode::ListMode),
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
        return QSize(258, 210); // Grid View
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

    // List or Grid View Mode
    bool isListViewMode = (_viewMode == ItemsViewMode::ListMode);

    // Progress Bar 
    bool showProgressBar = index.data(ItemDelegateRoles::ShowProgress).toBool();

    /* ----------------------- Drawing Item Hover ------------------ */
    painter->setPen(Qt::NoPen);
    painter->setBrush(isDarkMode ? "#2D2D2D" : "#F2F2F2");
    if (isSelected || isMouseOver)
        painter->drawRoundedRect(itemRect.adjusted(1, 2, -1, -2), 6, 6);


    /* ----------------- Select Item Check Box -------------------- */ 
    QRect cbRect = selectItemCheckboxRect(itemRect);
    
    // Border
    QPen pen;
    pen.setWidthF(isSelected ? 1.0 : 0.5);
    pen.setColor(isSelected ? QColor("#0191DF") : (isDarkMode ? QColor("#616161") : QColor("#CCCCCC")));
    pen.setStyle(Qt::SolidLine);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);

    // Background
    QColor brushColor;
    if (isSelected) 
       brushColor = isMouseOver ?   QColor("#1BB3E6") : QColor("#0191DF");
    else if (isMouseOver) 
       brushColor = isDarkMode  ?   QColor("#323232") : QColor("#F2F2F2");
    else 
       brushColor = isDarkMode  ?   QColor("#2D2D2D") : QColor("#FBFBFB");

    painter->setBrush(brushColor);

    if (isListViewMode) {
        painter->drawRoundedRect(cbRect, 6, 6);

        // Pixmap
        if (!cbPixmap.isNull() && isSelected) {
            int xPos = cbRect.x() + (cbRect.width() - cbPixmap.width()) / 2;
            int yPos = cbRect.y() + (cbRect.height() - cbPixmap.height()) / 2;
            painter->drawPixmap(xPos, yPos, cbPixmap);
        }
    }

    /* ----------------- File Thumbnail -----------------*/
    int thumbW = isListViewMode ? 120 : (itemRect.width() - 14);
    int thumbH = isListViewMode ? (itemRect.height() - 12) : 120;
    int thumbX = isListViewMode ? (cbRect.right() + spacing) : (itemRect.x() + (itemRect.width() - thumbW) / 2);
    int thumbY = isListViewMode ? (itemRect.y() + (itemRect.height() - thumbH) / 2) : (itemRect.y() + 8);

    QRect thumbnailRect(thumbX, thumbY, thumbW, thumbH);

    // Transparent Border
    painter->setPen(Qt::transparent);

    // Background
    brushColor = isDarkMode ? "#1C1C1C" : "#FFFFFF";
    painter->setBrush(brushColor);

    // Drawing Background
    painter->drawRoundedRect(thumbnailRect, 6, 6);

    // Drawing Icon
    QSize iconSize;

    if (isListViewMode)
        iconSize = QSize(40, 46);     
    else
        iconSize = QSize(80, 92);    

    QPixmap thumbPixmap = QPixmap(index.data(ItemDelegateRoles::FileThumbnail).toString())
                      .scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    int thumbIconX = thumbnailRect.x() + (thumbnailRect.width() - iconSize.width()) / 2;
    int thumbIconY = thumbnailRect.y() + (thumbnailRect.height() - iconSize.height()) / 2;

    painter->save();
    if (!isListViewMode && showProgressBar)
        painter->setOpacity(0.1);

    painter->drawPixmap(thumbIconX, thumbIconY, thumbPixmap);
    painter->restore();

    /* ----------------- File Title -------------------- */
    QString fileTitle = index.data(ItemDelegateRoles::FileTitle).toString();

    int titleW = isListViewMode ? (itemRect.width() * 0.30) : (itemRect.width() - 16);
    int titleH = itemRect.height() - 4;
    int titleX = isListViewMode ? (thumbnailRect.right() + spacing) : (itemRect.x() + (itemRect.width() - titleW) / 2);
    int titleY = isListViewMode ? itemRect.y() : (thumbnailRect.bottom() + 14);
    
    // Font
    font.setWeight(QFont::Normal);
    painter->setFont(font);

    QFontMetrics fm(font);

    if (!isListViewMode)
        titleH = fm.height() + 2;
    
    // Title Rect
    QRect titleRect(titleX, titleY, titleW, titleH);

    // Pen Color
    painter->setPen(isDarkMode ? Qt::white : Qt::black);

    // Text Alignment Flags
    Qt::TextFlag textFlags;
    Qt::Alignment alignmentFlags;
    if (isListViewMode) {
        alignmentFlags = Qt::AlignLeft | Qt::AlignVCenter;
        textFlags = Qt::TextWordWrap;
    } else {
        alignmentFlags = Qt::AlignCenter;
        textFlags = Qt::TextSingleLine;
    }

    // Elided Text for Grid Mode
    QString elidedTitleTxt = fm.elidedText(fileTitle, Qt::ElideRight, titleW);

    // Drawing Text
    painter->drawText(titleRect, alignmentFlags | textFlags, isListViewMode ? fileTitle : elidedTitleTxt);

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

    if (isListViewMode) {
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
    }

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
    
    if (isListViewMode) {
        // Text Color
        painter->setPen("#6B7280");

        // Drawing Text
        painter->drawText(sizeTxtRect, Qt::AlignVCenter | Qt::AlignLeft, elidedFileSizeTxt);
    }

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

    if (isListViewMode) {
        // Text Color
        painter->setPen("#6B7280");

        // Drawing Text
        painter->drawText(dateModifiedTxtRect, Qt::AlignVCenter | Qt::AlignLeft, elidedDateModifiedTxt);
    }

    /* ----------------- Encryption/Decryption ------------ */
    bool isEncrypted = (index.data(ItemDelegateRoles::EncryptionStatus)).toBool();
    bool isDecrypted = (index.data(ItemDelegateRoles::DecryptionStatus)).toBool();

    QString statusText;

    // Rect 
    const int statusH = 24;
    const int statusW = 102;
    int statusX = isListViewMode ? (dateModifiedTxtRect.right() + spacing) : (itemRect.x() + (itemRect.width() - statusW) / 2);
    int statusY = isListViewMode ? (itemRect.y() + (itemRect.height() - statusH) / 2) : (titleRect.bottom() + 14); 
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

    /* --------------------- Progress Bar ---------------------------------- */

    if (showProgressBar) {
        const int progressW = 140;
        const int progressH = 26;
        int progressY = isListViewMode ? (itemRect.y() + (itemRect.height() - progressH) / 2) : (thumbnailRect.y() + (thumbnailRect.height() - progressH) / 2);
        int progressX = isListViewMode ? (statusRect.right() + spacing) : (thumbnailRect.x() + (thumbnailRect.width() - progressW)/2);

        // Main Rectangle (Progress Bar + Percentage + Text)
        QRect progressRect(progressX, progressY, progressW, progressH);

        // Progress Text Rect
        const int progressSpacing = 7;
        const int trackH = 4;
        int progressTextH = fm.height();
        int totalContentH = progressTextH + progressSpacing + trackH;
        int contentY = progressRect.y() + (progressRect.height() - totalContentH) / 2;

        QRect progressTxtRect(progressRect.x() + 2, contentY, progressRect.width(), progressTextH);

        // Text Color
        QColor progressColor =  isDecrypted ? (
                isDarkMode ? "#44FF73" : "#0ACD3B"
            ) : (
                isDarkMode ? "#FF8989" : "#C71010"
            );

        painter->setPen(progressColor);

        // Drawing Text
        QString progressText;
        if (isEncrypted) 
            progressText = "Decrypting...";
        
        if (isDecrypted)
            progressText = "Encrypting...";

        painter->drawText(progressTxtRect, isListViewMode ? (Qt::AlignLeft | Qt::AlignVCenter) : (Qt::AlignCenter), progressText);

        // Progress Track and Bar
        // Track
        painter->setPen(Qt::NoPen);

        if (isListViewMode)
            brushColor = isDarkMode ? "#1C1C1C" : "#FFFFFF";
        else
            brushColor = isDarkMode ? "#2D2D2D" : "#F2F2F2";

        painter->setBrush(brushColor);

        const int trackW = 100;
        int trackX = progressRect.x() + 2;
        int trackY = progressTxtRect.bottom() + progressSpacing;
        
        // Track Rect
        QRect trackRec(trackX, trackY, trackW, trackH);

        // Drawing Track
        painter->drawRoundedRect(trackRec, trackH / 2, trackH / 2);

        // Progress bar
        painter->setBrush(progressColor);

        // Getting current Value
        double currentValue = index.data(ItemDelegateRoles::ProgressCurrentValue).toDouble();
        currentValue = std::clamp(currentValue / 100, 0.0, 1.0);
        int barWidth = int(currentValue * trackW);
        
        QRect barRec(trackX, trackY, barWidth, trackH);
        painter->drawRoundedRect(barRec, trackH / 2, trackH / 2);

        // Progress Percentage Text
        painter->setPen(progressColor);

        QString progressPercentText = QString::number(index.data(ItemDelegateRoles::ProgressCurrentValue).toInt()) + "%";
        
        // Percentage Rect
        int percentY = trackRec.y() + (trackRec.height() - progressTextH) / 2;

        QRect progressPercentRect(trackRec.right() + progressSpacing, percentY - 1, 28, progressTextH);
        painter->drawText(progressPercentRect, Qt::AlignRight | Qt::AlignVCenter, progressPercentText);
    }

    painter->restore();
}

QRect ViewDelegate::selectItemCheckboxRect(const QRect &itemRect) const {
    int cbX = itemRect.left() + spacing;
    int cbY = itemRect.y() + (itemRect.height() - cbSize.height()) / 2;

    return QRect(cbX, cbY, cbSize.width(), cbSize.height());
}
