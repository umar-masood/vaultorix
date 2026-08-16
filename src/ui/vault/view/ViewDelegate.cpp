#include "ViewDelegate.h"

#include "../../../../resources/IconManager.h"

#include <QTimer>
#include <QIcon>
#include <QVariant>
#include <QLocale>

using Ui::Vault::ViewItem;
using Ui::Vault::ViewDelegate;
using Ui::Vault::ViewItemsMode;
using Ui::Vault::ViewItemRoles;

static Icons iconForExtension(const QString &ext) {
    QString e = ext.toUpper();

    if (e == "AI")   return Icons::File_AI;
    if (e == "AVI")  return Icons::File_AVI;
    if (e == "BMP")  return Icons::File_BMP;
    if (e == "CRD")  return Icons::File_CRD;
    if (e == "CSV")  return Icons::File_CSV;
    if (e == "DLL")  return Icons::File_DLL;
    if (e == "DOC")  return Icons::File_DOC;
    if (e == "DOCX") return Icons::File_DOCX;
    if (e == "DWG")  return Icons::File_DWG;
    if (e == "EPS")  return Icons::File_EPS;
    if (e == "EXE")  return Icons::File_EXE;
    if (e == "FLV")  return Icons::File_FLV;
    if (e == "GIF")  return Icons::File_GIF;
    if (e == "HTML" || e == "HTM") return Icons::File_HTML;
    if (e == "ISO")  return Icons::File_ISO;
    if (e == "JAVA") return Icons::File_JAVA;
    if (e == "JPG" || e == "JPEG") return Icons::File_JPG;
    if (e == "MDB")  return Icons::File_MDB;
    if (e == "MID" || e == "MIDI") return Icons::File_MID;
    if (e == "MOV")  return Icons::File_MOV;
    if (e == "MP3")  return Icons::File_MP3;
    if (e == "MP4")  return Icons::File_MP4;
    if (e == "MPEG" || e == "MPG") return Icons::File_MPEG;
    if (e == "PDF")  return Icons::File_PDF;
    if (e == "PNG")  return Icons::File_PNG;
    if (e == "PPT" || e == "PPTX") return Icons::File_PPT;
    if (e == "PS")   return Icons::File_PS;
    if (e == "PSD")  return Icons::File_PSD;
    if (e == "PUB")  return Icons::File_PUB;
    if (e == "RAR")  return Icons::File_RAR;
    if (e == "RAW")  return Icons::File_RAW;
    if (e == "RSS")  return Icons::File_RSS;
    if (e == "SVG")  return Icons::File_SVG;
    if (e == "TIFF" || e == "TIF") return Icons::File_TIFF;
    if (e == "TXT")  return Icons::File_TXT;
    if (e == "WAV")  return Icons::File_WAV;
    if (e == "WMA")  return Icons::File_WMA;
    if (e == "XML")  return Icons::File_XML;
    if (e == "XSL" || e == "XLS" || e == "XLSX") return Icons::File_XSL;
    if (e == "ZIP")  return Icons::File_ZIP;

    return Icons::File_TXT; // fallback
}

ViewItem::ViewItem(const Core::Vault::FileMetadata &metadata) {
    setData(metadata.actualName, Ui::Vault::FileTitle);
    setData(metadata.extension, Ui::Vault::FileType);
    setData(QLocale().formattedDataSize(metadata.size, 2, QLocale::DataSizeTraditionalFormat), Ui::Vault::FileSize);
    setData(metadata.createdAt.toString("dd-MM-yyyy"), Ui::Vault::FileCreatedAt);
    setData(IconManager::icon(iconForExtension(metadata.extension)), Ui::Vault::FileThumbnail);
    setData(QVariant::fromValue(metadata.status), Ui::Vault::FileStatus);
    setData(metadata.fileId, Ui::Vault::FileId);  
}

ViewDelegate::ViewDelegate(QObject *parent) :
                    QStyledItemDelegate(parent), 
                    cbSize(20, 20),
                    statusIconSize(16, 16),
                    _viewMode(ViewItemsMode::ListMode),
                    cbPixmap(QPixmap((IconManager::icon(Icons::CheckBox_Check))).scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation)) {}

void ViewDelegate::setDarkMode(bool enable) {
    if (isDarkMode == enable)
        return;

    isDarkMode = enable;
}

void ViewDelegate::setItemHeight(int itemHeight) {
    _itemHeight = itemHeight;
}

void ViewDelegate::setViewMode(ViewItemsMode viewMode) {
    _viewMode = viewMode;
}

QSize ViewDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (_viewMode == ViewItemsMode::ListMode) 
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
    
    Core::Vault::FileStatus status = index.data(ViewItemRoles::FileStatus).value<Core::Vault::FileStatus>();
    bool isImporting  = status == Core::Vault::FileStatus::Importing;
    bool isEncrypting = status == Core::Vault::FileStatus::Encrypting;
    bool isDecrypting = status == Core::Vault::FileStatus::Decrypting;
    bool isRestoring  = status == Core::Vault::FileStatus::Restoring;
    bool isDeleting   = status == Core::Vault::FileStatus::Deleting;
    bool isQueued     = status == Core::Vault::FileStatus::Queued;
    bool isEncrypted  = status == Core::Vault::FileStatus::Encrypted;
    bool isDecrypted  = status == Core::Vault::FileStatus::Decrypted;
    bool isFailed     = status == Core::Vault::FileStatus::Failed;

    bool showProgressBar = isImporting  || isEncrypting || isDecrypting || isRestoring  || isDeleting;

    // Main Font
    QFont font("Segoe UI", 10);

    // List or Grid View Mode
    bool isListViewMode = (_viewMode == ViewItemsMode::ListMode);

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

    QPixmap thumbPixmap = QPixmap(index.data(ViewItemRoles::FileThumbnail).toString())
                      .scaled(iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    int thumbIconX = thumbnailRect.x() + (thumbnailRect.width() - iconSize.width()) / 2;
    int thumbIconY = thumbnailRect.y() + (thumbnailRect.height() - iconSize.height()) / 2;

    painter->save();
    if (!isListViewMode && showProgressBar)
        painter->setOpacity(0.1);

    painter->drawPixmap(thumbIconX, thumbIconY, thumbPixmap);
    painter->restore();

    /* ----------------- File Title -------------------- */
    QString fileTitle = index.data(ViewItemRoles::FileTitle).toString();

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
    QString type = (index.data(ViewItemRoles::FileType).toString()).toUpper();
    
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
    QString fileSize = index.data(ViewItemRoles::FileSize).toString().toUpper();

    int sizeX = typeRect.right() + spacing;
    int sizeY = itemRect.y();
    const int sizeW = 70;
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
    QString dateModified = index.data(ViewItemRoles::FileCreatedAt).toString();

    int dateModifiedX = sizeTxtRect.right() + spacing;
    int dateModifiedY = itemRect.y();
    const int dateModifiedW = 70;
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

    /* ----------------- Encryption/Decryption/Import/Delete/Restore ------------ */
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
    if (isEncrypted || isDecrypting) 
        brushColor = isDarkMode ? "#0F2F1A" : "#D6FFE8";
    else if (isDecrypted || isFailed)
        brushColor = isDarkMode ? "#561616" : "#FFD9D9";
    else if (isQueued)
        brushColor = isDarkMode ? "#10243E" : "#DBEAFE";
    else
        brushColor = isDarkMode ? "#4B3F00" : "#FFF8E1";

    painter->setBrush(brushColor);

    if (!isImporting)
        painter->drawRoundedRect(statusRect, 6, 6);
    
    // Icon
    QString currentIconPath;
    if (isEncrypted || isDecrypting) 
        currentIconPath = isDarkMode ? IconManager::icon(Icons::EncryptDark) : IconManager::icon(Icons::EncryptLight);
    else if (isDecrypted)
        currentIconPath = isDarkMode ? IconManager::icon(Icons::DecryptDark) : IconManager::icon(Icons::DecryptLight);
    else if (isFailed)
        currentIconPath = isDarkMode ? IconManager::icon(Icons::XDark) : IconManager::icon(Icons::XLight);
    else if (isQueued)
        currentIconPath = isDarkMode ? IconManager::icon(Icons::QueuedDark) : IconManager::icon(Icons::QueuedLight);
    else
        currentIconPath = isDarkMode ? IconManager::icon(Icons::XDark) : IconManager::icon(Icons::XLight);

    QPixmap statusPixmap = QPixmap(currentIconPath).scaled(QSize(16, 16), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    int statusIconX = statusRect.x() + 8;
    int statusIconY = statusRect.y() + (statusRect.height() - statusIconSize.height()) / 2;
    
    if (!isImporting)
        painter->drawPixmap(statusIconX, statusIconY, statusPixmap);
    
    // Font
    font.setWeight(QFont::DemiBold);
    painter->setFont(font);

    // Text Color
    QColor textColor;
    if (isEncrypted || isDecrypting) {
        textColor = isDarkMode ? "#44FF73" : "#0ACD3B";
        statusText = "Encrypted";
    } else if (isDecrypted) {
        textColor = isDarkMode ? "#FF8989" : "#C71010";
        statusText = "Decrypted";
    } else if (isQueued) {
        textColor = isDarkMode ? "#93C5FD" : "#2563EB";
        statusText = "Queued";
    } else if (isFailed) {
        textColor = isDarkMode ? "#FF8989" : "#C71010";
        statusText = "Failed";
    } else {
        textColor = isDarkMode ? "#FCD34D" : "#B45309";
        statusText = "Not Safe";
    }

    painter->setPen(!isImporting ? textColor : Qt::transparent);

    // Text Rect
    int statusTxtX = statusIconX + statusIconSize.width() + 8;
    int statusTxtY = statusRect.y();
    const int statusTxtW = 62;
    int statusTxtH = statusH - 2;

    QRect statusTxtRect(statusTxtX, statusTxtY, statusTxtW, statusTxtH);

    // Drawing Text
    if (!isImporting)
        painter->drawText(statusTxtRect, Qt::AlignVCenter | Qt::AlignLeft, statusText);

    /* --------------------- Progress Bar ---------------------------------- */
    if (showProgressBar) {
        const int spinnerSize = 24;
        int progressY = isListViewMode
            ? (itemRect.y() + (itemRect.height() - spinnerSize) / 2)
            : (thumbnailRect.y() + (thumbnailRect.height() - spinnerSize) / 2);
        int progressX = isListViewMode
            ? (statusRect.right() + spacing)
            : (thumbnailRect.x() + (thumbnailRect.width() - spinnerSize - spacing - 80) / 2);

        // Progress color
        QColor progressColor;
        if      (isEncrypting)                  progressColor = isDarkMode ? "#44FF73" : "#0ACD3B";
        else if (isDecrypting || isDeleting)    progressColor = isDarkMode ? "#FF8989" : "#C71010";
        else if (isImporting  || isRestoring)   progressColor = "#109AC7";
        else                                    progressColor = isDarkMode ? "#FFFFFF" : "#000000";

        double currentValue = index.data(ViewItemRoles::ProgressCurrentValue).toDouble();
        currentValue = std::clamp(currentValue / 100.0, 0.0, 1.0);

        QRect spinnerRect(progressX, progressY, spinnerSize, spinnerSize);
        QRect arcRect = spinnerRect.adjusted(2, 2, -2, -2);

        // Track (background circle)
        QPen trackPen;
        trackPen.setWidthF(3.0);
        trackPen.setColor(isDarkMode ? QColor("#3A3A3A") : QColor("#E0E0E0"));
        trackPen.setCapStyle(Qt::RoundCap);
        painter->setPen(trackPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(arcRect);

        // Arc (actual progress)
        QPen arcPen;
        arcPen.setWidthF(3.0);
        arcPen.setColor(progressColor);
        arcPen.setCapStyle(Qt::RoundCap);
        painter->setPen(arcPen);
        painter->drawArc(arcRect, 90 * 16, -int(currentValue * 360 * 16));

        // Status text
        QString progressText;
        if      (isImporting)   progressText = "Importing...";
        else if (isRestoring)   progressText = "Restoring...";
        else if (isDeleting)    progressText = "Deleting...";
        else if (isEncrypting)  progressText = "Encrypting...";
        else if (isDecrypting)  progressText = "Decrypting...";

        // Percentage text (spinner ke baad)
        font.setWeight(QFont::DemiBold);
        painter->setFont(font);
        painter->setPen(progressColor);
        painter->drawText(
            QRect(spinnerRect.right() + 6, progressY, 36, spinnerSize),
            Qt::AlignVCenter | Qt::AlignLeft,
            QString::number(index.data(ViewItemRoles::ProgressCurrentValue).toInt()) + "%");

        // Label text (percent ke baad)
        font.setWeight(QFont::Normal);
        painter->setFont(font);
        painter->drawText(
            QRect(spinnerRect.right() + 6 + 36 + 4, progressY, 80, spinnerSize),
            Qt::AlignVCenter | Qt::AlignLeft,
            progressText);
    }

    painter->restore();
}

QRect ViewDelegate::selectItemCheckboxRect(const QRect &itemRect) const {
    int cbX = itemRect.left() + spacing;
    int cbY = itemRect.y() + (itemRect.height() - cbSize.height()) / 2;

    return QRect(cbX, cbY, cbSize.width(), cbSize.height());
}
