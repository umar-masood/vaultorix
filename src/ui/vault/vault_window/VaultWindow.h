#pragma once

#include "../../windows/window/Window.h"
#include "../../../core/errors/ImportError.h"

class Seperator;
class Label;
class QHBoxLayout;
class QVBoxLayout;
class QVariant;
class QModelIndex;

namespace Core::Vault {
    class StorageService;
    enum class FileStatus;
    struct FileMetadata;
};

namespace Ui::Vault {
    class Statusbar;
    class View;
    class User;
    class Toolbar;
    class Update;
    class About;
    class ReportBug;
}

namespace Ui::Vault { 
    /* --------------------- Vault Main Window --------------------------- */
    class VaultWindow : public Window {
        Q_OBJECT

        public:
        static VaultWindow* instance(QWidget *parent = nullptr);

        private:    
        explicit VaultWindow(QWidget *parent = nullptr);
        Button* createButton(const QString &iconPathLight, const QString &iconPathDark);
        void updateModelItemData(int fileId, const QVariant & variant, int role);
        void setDarkMode(bool isDarkMode);

        // Title Bar
        // App name & icon
        Label *app_icon = nullptr;
        Label *app_name = nullptr;

        // Seperator for titlebar controls
        Seperator *seperator = nullptr;

        // Theme Mode Button   
        Button *theme_mode_btn = nullptr;

        // Icons   
        QString AppIcon, DarkModeIcon, LightModeIcon;

        // Settings Button
        Button *preferences_btn  = nullptr;

        // Preferences Settings
        // Ui::Vault::Preferences *pref = nullptr;

        // Updates Download Button
        Button *updates_download_btn = nullptr;
           
        // Updates Download Button
        Button *about_btn = nullptr;

        // App Updates 
        Ui::Vault::Update *appUpdateWidget = nullptr;
            
        // App About
        Ui::Vault::About *about = nullptr;

        // Report a bug Button
        Button *report_bug_btn = nullptr;

        // Report Bug
        Ui::Vault::ReportBug *report_bug = nullptr;

        // Content Area
        // Toolbar
        Ui::Vault::Toolbar *toolbar = nullptr;
        
        // View
        Ui::Vault::View *view = nullptr;
        // Core
        Core::Vault::StorageService *storage_core = nullptr;

        // Statusbar
        Ui::Vault::Statusbar *_statusbar = nullptr;

        private slots:
        void onAboutBtnClicked();
        void onImportButtonClicked();
        void onPreferencesBtnClicked();
        void onAppUpdateBtnClicked();
        void onReportBugBtnClicked();
        
        void onDeleteBtnClicked();
        void onEncryptBtnClicked();
        void onDecryptBtnClicked();
        void onRestoreBtnClicked();
        void onOpenBtnClicked();

        void onViewItemClicked(const QModelIndex &index);
        void onRowsInserted(const QModelIndex &, int, int);
        void onRowsRemoved(const QModelIndex &, int, int);

        void onImportQueued(const Core::Vault::FileMetadata &metadata);
        void onImportStatusChanged(int fileId, const Core::Vault::FileStatus &status);
        void onImportProgressChanged(int fileId, int progress);

        void onDeleteQueued(int fileId);
        void onDeleteStatusChanged(int fileId, const Core::Vault::FileStatus &status);
        void onDeleteProgressChanged(int fileId, int progress);

        void onEncryptQueued(int fileId);
        void onEncryptStatusChanged(int fileId, const Core::Vault::FileStatus &status);
        void onEncryptProgressChanged(int fileId, int progress);

        void onDecryptQueued(int fileId);
        void onDecryptStatusChanged(int fileId, const Core::Vault::FileStatus &status);
        void onDecryptProgressChanged(int fileId, int progress);

        void onRestoreQueued(int fileId);
        void onRestoreStatusChanged(int fileId, const Core::Vault::FileStatus &status);
        void onRestoreProgressChanged(int fileId, int progress);
    
    };
};