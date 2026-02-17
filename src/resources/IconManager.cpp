#include "IconManager.h"

QString IconManager::icon(Icons icon) {
    switch (icon) {
        case Icons::AppIcon:           return ":/icons/app-icon.png";
   
        case Icons::Valid:             return ":/icons/valid.svg";
        case Icons::Invalid:           return ":/icons/invalid.svg";
   
        case Icons::Key:               return ":/icons/key.svg";
        case Icons::UserAvator:        return ":/icons/user.svg";
   
        case Icons::DarkMode:          return ":/icons/darkMode.svg";
        case Icons::LightMode:         return ":/icons/lightMode.svg";
   
        case Icons::TwoFactorAuth:     return ":/icons/twoFactorAuth.svg";
        case Icons::Backup:            return ":/icons/backup.svg";
        case Icons::Folder:            return ":/icons/folder.svg";
        case Icons::Lock:              return ":/icons/lock.svg";
        case Icons::Timer:             return ":/icons/timer.svg";
   
        case Icons::ArrowDown:         return ":/icons/arrow-down.svg";
        case Icons::ArrowRight:        return ":/icons/arrow-right.svg";

        case Icons::CheckBox_Check:    return ":/icons/checkbox-check.svg";
        case Icons::Delegate_Check:    return ":/icons/delegate-check.svg";

        case Icons::Cut:               return ":/icons/cut.svg";
        case Icons::Copy:              return ":/icons/copy.svg";
        case Icons::MenuDelete:        return ":/icons/menuDelete.svg";
        case Icons::FileDelete:        return ":/icons/fileDelete.svg";
   
        case Icons::Hide:              return ":/icons/hide.svg";
        case Icons::Show:              return ":/icons/show.svg";
   
        case Icons::Paste:             return ":/icons/paste.svg";
        case Icons::Redo:              return ":/icons/redo.svg";
        case Icons::SelectAll:         return ":/icons/select-all.svg";
        case Icons::Undo:              return ":/icons/undo.svg";
        case Icons::Clear:             return ":/icons/x.svg";
   
        case Icons::Encrypt:           return ":/icons/encrypt.svg";
        case Icons::Decrypt:           return ":/icons/decrypt.svg";
   
        case Icons::SignOut:           return ":/icons/sign-out.svg";
        case Icons::Search:            return ":/icons/search.svg";
        case Icons::Restore:           return ":/icons/restore.svg";
        case Icons::Open:              return ":/icons/open.svg";
        case Icons::Close:             return ":/icons/x.svg";
   
        case Icons::Import:            return ":/icons/import.svg";
        case Icons::Filter:            return ":/icons/filter.svg";
   
        case Icons::ListView:          return ":/icons/listView.svg";
        case Icons::ListViewChecked:   return ":/icons/listViewChecked.svg";
        
        case Icons::GridView:          return ":/icons/gridView.svg";
        case Icons::GridViewChecked:   return ":/icons/gridViewChecked.svg";

        case Icons::Dot:               return ":/icons/dot.svg";

        case Icons::XLight:            return ":/icons/xLight.svg";
        case Icons::XDark:             return ":/icons/xDark.svg";
   
        case Icons::EncryptLight:      return ":/icons/encryptLight.svg";
        case Icons::EncryptDark:       return ":/icons/encryptDark.svg";
        
        case Icons::DecryptLight:      return ":/icons/decryptLight.svg";
        case Icons::DecryptDark:       return ":/icons/decryptDark.svg";

        case Icons::Avator:            return ":/icons/avator.svg";
        case Icons::AccountSettings:   return ":/icons/account-settings.svg";
        case Icons::Bug:               return ":/icons/bug.svg";
        case Icons::PreferencesLight:  return ":/icons/preferences-light.svg";
        case Icons::PreferencesDark:   return ":/icons/preferences-dark.svg";
        case Icons::About:             return ":/icons/info.svg";
        case Icons::Subscription:      return ":/icons/subscription.svg";

        case Icons::Files:             return ":/icons/files.svg";
        case Icons::Picture:           return ":/icons/picture.svg";
        case Icons::Video:             return ":/icons/video.svg";
        case Icons::Music:             return ":/icons/music.svg";
        case Icons::Document:          return ":/icons/document.svg";

        
        case Icons::Win_CloseLight:    return ":/icons/win-close-light.svg";
        case Icons::Win_CloseDark:     return ":/icons/win-close-dark.svg";
        case Icons::Win_MinimizeLight: return ":/icons/win-minimize-light.svg";
        case Icons::Win_MinimizeDark:  return ":/icons/win-minimize-dark.svg";
        case Icons::Win_MaximizeLight: return ":/icons/win-maximize-light.svg";
        case Icons::Win_MaximizeDark:  return ":/icons/win-maximize-dark.svg";
        case Icons::Win_RestoreLight:  return ":/icons/win-restore-light.svg";
        case Icons::Win_RestoreDark:   return ":/icons/win-restore-dark.svg";

        case Icons::File_AI:           return ":/icons/fileTypes/AI.svg";
        case Icons::File_AVI:          return ":/icons/fileTypes/AVI.svg";
        case Icons::File_BMP:          return ":/icons/fileTypes/BMP.svg";
        case Icons::File_CRD:          return ":/icons/fileTypes/CRD.svg";
        case Icons::File_CSV:          return ":/icons/fileTypes/CSV.svg";
        case Icons::File_DLL:          return ":/icons/fileTypes/DLL.svg";
        case Icons::File_DOC:          return ":/icons/fileTypes/DOC.svg";
        case Icons::File_DOCX:         return ":/icons/fileTypes/DOCX.svg";
        case Icons::File_DWG:          return ":/icons/fileTypes/DWG.svg";
        case Icons::File_EPS:          return ":/icons/fileTypes/EPS.svg";
        case Icons::File_EXE:          return ":/icons/fileTypes/EXE.svg";
        case Icons::File_FLV:          return ":/icons/fileTypes/FLV.svg";
        case Icons::File_GIF:          return ":/icons/fileTypes/GIFF.svg";
        case Icons::File_HTML:         return ":/icons/fileTypes/HTML.svg";
        case Icons::File_ISO:          return ":/icons/fileTypes/ISO.svg";
        case Icons::File_JAVA:         return ":/icons/fileTypes/JAVA.svg";
        case Icons::File_JPG:          return ":/icons/fileTypes/JPG.svg";
        case Icons::File_MDB:          return ":/icons/fileTypes/MDB.svg";
        case Icons::File_MID:          return ":/icons/fileTypes/MID.svg";
        case Icons::File_MOV:          return ":/icons/fileTypes/MOV.svg";
        case Icons::File_MP3:          return ":/icons/fileTypes/MP3.svg";
        case Icons::File_MP4:          return ":/icons/fileTypes/MP4.svg";
        case Icons::File_MPEG:         return ":/icons/fileTypes/MPEG.svg";
        case Icons::File_PDF:          return ":/icons/fileTypes/PDF.svg";
        case Icons::File_PNG:          return ":/icons/fileTypes/PNG.svg";
        case Icons::File_PPT:          return ":/icons/fileTypes/PPT.svg";
        case Icons::File_PS:           return ":/icons/fileTypes/PS.svg";
        case Icons::File_PSD:          return ":/icons/fileTypes/PSD.svg";
        case Icons::File_PUB:          return ":/icons/fileTypes/PUB.svg";
        case Icons::File_RAR:          return ":/icons/fileTypes/RAR.svg";
        case Icons::File_RAW:          return ":/icons/fileTypes/RAW.svg";
        case Icons::File_RSS:          return ":/icons/fileTypes/RSS.svg";
        case Icons::File_SVG:          return ":/icons/fileTypes/SVG.svg";
        case Icons::File_TIFF:         return ":/icons/fileTypes/TIFF.svg";
        case Icons::File_TXT:          return ":/icons/fileTypes/TXT.svg";
        case Icons::File_WAV:          return ":/icons/fileTypes/WAV.svg";
        case Icons::File_WMA:          return ":/icons/fileTypes/WMA.svg";
        case Icons::File_XML:          return ":/icons/fileTypes/XML.svg";
        case Icons::File_XSL:          return ":/icons/fileTypes/XSL.svg";
        case Icons::File_ZIP:          return ":/icons/fileTypes/ZIP.svg";
    }

    qWarning("The requested icon is not found");
    return QString();
}

QPixmap IconManager::renderSvg(const QString &iconPath, const QSize &size) {
    QSvgRenderer svg(iconPath);
    svg.setAspectRatioMode(Qt::KeepAspectRatio);
    
    if (!svg.isValid()) {
        qWarning() << "SVG not valid or not found:" << iconPath;
        return QPixmap();
    }

    // --- Render SVG into 16x16 pixmap ---
    QPixmap pm(size);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    svg.render(&p, QRectF(0, 0, pm.width(), pm.height()));
    p.end();

    return pm;
}