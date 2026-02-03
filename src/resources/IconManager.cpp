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
        case Icons::Check:             return ":/icons/check.png";
   
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

    }

    qWarning("The requested icon is not found");
    return QString();
}
