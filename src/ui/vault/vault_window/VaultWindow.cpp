#include "VaultWindow.h"

#include "../../../core/theme/ThemeManager.h"
#include "../../../core/config/Constants.h"

#include "../../../core/services/vault/storage/StorageService.h"
#include "../../../core/services/vault/repository/FileRepository.h"
#include "../../../core/crypto/key/KeyManager.h"

#include "../statusbar/Statusbar.h"
#include "../view/View.h"
#include "../user/User.h"
#include "../toolbar/Toolbar.h"
#include "../update/Update.h"
#include "../about/About.h"
#include "../report_bug/ReportBug.h"
#include "../empty_state/EmptyState.h"
#include "../view/ViewDelegate.h"

#include "../../dialogs/error_dialog/ErrorDialog.h"
#include "../../dialogs/confirm_dialog/ConfirmDialog.h"

#include "../../../../resources/IconManager.h"
#include "../../components/Seperator.h"
#include "../../components/Label.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QStandardPaths>
#include <QFileInfo>
#include <QListView>
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QDesktopServices>

using Ui::Vault::VaultWindow;
VaultWindow *VaultWindow::instance(QWidget *parent) {
    static VaultWindow *vw = nullptr;

    if (!vw)
        vw = new VaultWindow(parent);

    return vw;
}

VaultWindow::VaultWindow(QWidget *parent) : Window(parent) {
    // Window Properties
    setWindowTitle("Vaultorix");
    setWindowIcon(QIcon(":/app-icon.ico"));
    setMinimumSize(QSize(1100, 600));

    // Icons
    AppIcon = IconManager::icon(Icons::AppIcon);
    DarkModeIcon = IconManager::icon(Icons::DarkMode);
    LightModeIcon = IconManager::icon(Icons::LightMode);

    // Title Bar
    // App Icon
    app_icon = new Label(true);
    app_icon->setFixedSize(QSize(26, 24));
    app_icon->setPixmap(QPixmap(AppIcon).scaled(26, 24, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    // App Name
    app_name = new Label("Segoe UI", 11, QFont::Normal, false, "Vaultorix");

    // Seperator
    seperator = new Seperator(18, 1, Qt::Vertical);
    seperator->raise();

    // Theme Mode Button
    theme_mode_btn = createButton(IconManager::icon(Icons::LightMode), IconManager::icon(Icons::LightMode));
    theme_mode_btn->setToolTip("Change theme mode");
    setInteractiveTitleBarWidget(theme_mode_btn);

    // Theme Mode Button Signal Slot
    connect(theme_mode_btn, &Button::clicked, this, [this]() {
       auto &tm = ThemeManager::instance();
       tm.setDarkMode(!tm.isDarkMode()); 
    });

    // Update Download Button
    updates_download_btn = createButton(IconManager::icon(Icons::DownloadUpdateLight), IconManager::icon(Icons::DownloadUpdateDark));
    updates_download_btn->setToolTip("Vaultorix Updates");
    connect(updates_download_btn, &Button::clicked, this, &VaultWindow::onAppUpdateBtnClicked);

    // // Preferences Button
    // preferences_btn = createButton(IconManager::icon(Icons::PreferencesLight), IconManager::icon(Icons::PreferencesDark));
    // preferences_btn->setToolTip("Preferences");
    // connect(preferences_btn, &Button::clicked, this, &VaultWindow::onPreferencesBtnClicked);

    // About Button
    about_btn = createButton(IconManager::icon(Icons::AboutLight), IconManager::icon(Icons::AboutDark));
    about_btn->setToolTip("About Vaultorix");
    connect(about_btn, &Button::clicked, this, &VaultWindow::onAboutBtnClicked);

    // Report Bug Button
    report_bug_btn = createButton(IconManager::icon(Icons::BugLight), IconManager::icon(Icons::BugDark));
    report_bug_btn->setToolTip("Report a bug");
    connect(report_bug_btn, &Button::clicked, this, &VaultWindow::onReportBugBtnClicked);

    // Title Bar Layout
    auto *titlebar_layout = new QHBoxLayout(titleBar());
    titlebar_layout->setSpacing(0);
    titlebar_layout->setContentsMargins(0, 0, 0, 0);

    titlebar_layout->addSpacing(7);
    titlebar_layout->addWidget(app_icon, 0, Qt::AlignLeft);
    titlebar_layout->addSpacing(10);
    titlebar_layout->addWidget(app_name, 0, Qt::AlignLeft);

    titlebar_layout->addStretch();

    for (auto *btn : {report_bug_btn, about_btn, updates_download_btn, /*preferences_btn*/ theme_mode_btn}) {
        titlebar_layout->addWidget(btn, 0, Qt::AlignRight);
        titlebar_layout->addSpacing(10);
    }

    titlebar_layout->addWidget(seperator, 0, Qt::AlignRight);
    titlebar_layout->addSpacing(10);

    // Content Area
    // Toolbar
    toolbar = new Ui::Vault::Toolbar;
    connect(toolbar->importButton(), &Button::clicked, this, &VaultWindow::onImportButtonClicked);
    connect(toolbar->deleteButton(), &Button::clicked, this, &VaultWindow::onDeleteBtnClicked);
    connect(toolbar->encryptButton(), &Button::clicked, this, &VaultWindow::onEncryptBtnClicked);
    connect(toolbar->decryptButton(), &Button::clicked, this, &VaultWindow::onDecryptBtnClicked);
    connect(toolbar->restoreButton(), &Button::clicked, this, &VaultWindow::onRestoreBtnClicked);
    connect(toolbar->openButton(), &Button::clicked, this, &VaultWindow::onOpenBtnClicked);

    // View
    view = new Ui::Vault::View;
    connect(view->emptyStateWidget(), &EmptyState::clicked, this, &VaultWindow::onImportButtonClicked);

    // Core
    storage_core = new Core::Vault::StorageService(this);
    connect(storage_core, &Core::Vault::StorageService::importQueued, this, &VaultWindow::onImportQueued);
    connect(storage_core, &Core::Vault::StorageService::importStatusChanged, this, &VaultWindow::onImportStatusChanged);
    connect(storage_core, &Core::Vault::StorageService::importProgressChanged, this, &VaultWindow::onImportProgressChanged);
    connect(storage_core, &Core::Vault::StorageService::deleteQueued, this, &VaultWindow::onDeleteQueued);
    connect(storage_core, &Core::Vault::StorageService::deleteStatusChanged, this, &VaultWindow::onDeleteStatusChanged);
    connect(storage_core, &Core::Vault::StorageService::deleteProgressChanged, this, &VaultWindow::onDeleteProgressChanged);
    connect(storage_core, &Core::Vault::StorageService::encryptQueued, this, &VaultWindow::onEncryptQueued);
    connect(storage_core, &Core::Vault::StorageService::encryptStatusChanged, this, &VaultWindow::onEncryptStatusChanged);
    connect(storage_core, &Core::Vault::StorageService::encryptProgressChanged, this, &VaultWindow::onEncryptProgressChanged);
    connect(storage_core, &Core::Vault::StorageService::decryptQueued, this, &VaultWindow::onDecryptQueued);
    connect(storage_core, &Core::Vault::StorageService::decryptStatusChanged, this, &VaultWindow::onDecryptStatusChanged);
    connect(storage_core, &Core::Vault::StorageService::decryptProgressChanged, this, &VaultWindow::onDecryptProgressChanged);
    connect(storage_core, &Core::Vault::StorageService::restoreQueued, this, &VaultWindow::onRestoreQueued);
    connect(storage_core, &Core::Vault::StorageService::restoreStatusChanged, this, &VaultWindow::onRestoreStatusChanged);
    connect(storage_core, &Core::Vault::StorageService::restoreProgressChanged, this, &VaultWindow::onRestoreProgressChanged);
        
    // Statusbar
    _statusbar = new Ui::Vault::Statusbar;

    // Layout
    auto *content_layout = new QVBoxLayout(contentArea());
    content_layout->setContentsMargins(6, 0, 6, 4);
    content_layout->setSpacing(0);
    content_layout->addSpacing(2);
    content_layout->addWidget(toolbar, 0, Qt::AlignTop);
    content_layout->addSpacing(4);
    content_layout->addWidget(view);
    content_layout->addSpacing(4);
    content_layout->addWidget(_statusbar);

    // Theme Mode
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &VaultWindow::setDarkMode);
    setDarkMode(tm.isDarkMode());
    
    // Error Dailog Manager Registry Window
    ErrorDialogManager::instance()->registerWindow("Vault", this);
    ConfirmDialogManager::instance()->registerWindow("Vault", this);

    // Init Key Manager in Current GUI Thread so that no other worker thread can do this to prevent illegal
    Core::Crypto::KeyManager::instance();

    // Updating Files Status on Startup
    Core::Vault::FileRepository::markEncryptingFilesImported(); // In case if app quit while file is encrypting
    Core::Vault::FileRepository::markImportingFilesAsFailed(); // In case if app quit while file is importing
    Core::Vault::FileRepository::markDecryptingFilesEncrypted(); // In case if app quit while file is decrypting

    // Fetching files metadata from Database
    auto files = Core::Vault::FileRepository::fetchFiles();
    if (files.has_value()) {
        for (auto i = 0; i < files.value().size(); i++)
            view->model().appendRow(new Ui::Vault::ViewItem(files.value()[i]));
    } else {
        ERROR_HERE("Failed to fetch files data from database.");
    }
    
    _statusbar->totalItemsLabel()->setText(QString("%1 items").arg(view->model().rowCount()));

    connect(view->list(), &QListView::clicked, this, &VaultWindow::onViewItemClicked);
    connect(&view->model(), &QAbstractItemModel::rowsInserted, this, &VaultWindow::onRowsInserted);
    connect(&view->model(), &QAbstractItemModel::rowsRemoved, this, &VaultWindow::onRowsRemoved);
}

Button *VaultWindow::createButton(const QString &iconPathLight, const QString &iconPathDark) {
    auto *btn = new Button;
    btn->setDisplayMode(Button::IconOnly);
    btn->setFixedSize(QSize(26, 26));
    btn->setSecondary(true);
    btn->setNormalBackgroundTransparent(true);
    btn->setBorderTransparent(true);
    btn->setIconSize(QSize(18, 18));
    btn->setIconPaths(iconPathLight, iconPathDark);
    btn->setCursor(Qt::PointingHandCursor);
    return btn;
}

void VaultWindow::setDarkMode(bool isDarkMode) {
    // Calling Base class member function of theme
    Window::setDarkMode(isDarkMode);

    // App name
    isDarkMode ? app_name->setStyleSheet("color: white;") : app_name->setStyleSheet("color: black;");

    // Buttons
    for (auto *btn : {theme_mode_btn, /*preferences_btn,*/ updates_download_btn, about_btn, report_bug_btn})
        btn->setDarkMode(isDarkMode);

    isDarkMode ? theme_mode_btn->setIconPaths(LightModeIcon, LightModeIcon) 
               : theme_mode_btn->setIconPaths(DarkModeIcon, DarkModeIcon);
}

void VaultWindow::onAboutBtnClicked() {
    if (!about) {
        about = new Ui::Vault::About(this);
        about->setAttribute(Qt::WA_DeleteOnClose);

        connect(about, &QObject::destroyed, this, [this](){
            about = nullptr;
            VaultWindow::instance()->updateGeometry(); 
        });
    }

    about->show();
    about->raise();
}

void VaultWindow::onImportButtonClicked() {
    QStringList filePaths = QFileDialog::getOpenFileNames(
        this,
        tr("Select file(s) to import"),
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
        "All Files (*.*)"
    );

    if (filePaths.isEmpty())
        return;
        
    for (auto path : filePaths)
        if (storage_core)
            storage_core->importFile(path);
}

void VaultWindow::onPreferencesBtnClicked() {
    // if (!pref) {
    //     pref = new Ui::Vault::Preferences(this);
    //     pref->setAttribute(Qt::WA_DeleteOnClose);   

    //     connect(pref, &QObject::destroyed, this, [this](){
    //         pref = nullptr;
    //         VaultWindow::instance()->updateGeometry(); 
    //     });
    // }

    // pref->show();
    // pref->raise();
}

void VaultWindow::onAppUpdateBtnClicked() {
    if (!appUpdateWidget) {
        appUpdateWidget = new Ui::Vault::Update(this);
        appUpdateWidget->setAttribute(Qt::WA_DeleteOnClose);

        connect(appUpdateWidget, &QObject::destroyed, this, [this](){
            appUpdateWidget = nullptr;
            VaultWindow::instance()->updateGeometry(); 
        });

        appUpdateWidget->show();
        appUpdateWidget->raise();
    } 
}

void VaultWindow::onReportBugBtnClicked() {
    if (!report_bug) {
        report_bug = new Ui::Vault::ReportBug(this);
        report_bug->setAttribute(Qt::WA_DeleteOnClose);
    }

    report_bug->show();
    report_bug->raise();
}

void VaultWindow::onDeleteBtnClicked() {
    auto CDM = ConfirmDialogManager::instance();
    CDM->show("FileDelete", "Vault");

    connect(CDM, &ConfirmDialogManager::actionTriggered, this, [this](const QString &key, const QString &actionButtonKey) {
        if (key == "FileDelete" && actionButtonKey == "Accept") {
            toolbar->deleteButton()->setEnabled(false);
            auto proxyIndex = view->list()->currentIndex();
            if (proxyIndex.isValid()) {
                auto sourceIndex = view->proxyModel()->mapToSource(proxyIndex);
                storage_core->deleteFile(sourceIndex.data(ViewItemRoles::FileId).toInt());
            }
        }
    }, Qt::SingleShotConnection);
}

void VaultWindow::onEncryptBtnClicked() {
    auto proxyIndex = view->list()->currentIndex();
    if (proxyIndex.isValid()) {
        auto sourceIndex = view->proxyModel()->mapToSource(proxyIndex);
        storage_core->encryptFile(sourceIndex.data(ViewItemRoles::FileId).toInt());
    }
}

void VaultWindow::onDecryptBtnClicked() {
    auto proxyIndex = view->list()->currentIndex();
    if (proxyIndex.isValid()) {
        auto sourceIndex = view->proxyModel()->mapToSource(proxyIndex);
        storage_core->decryptFile(sourceIndex.data(ViewItemRoles::FileId).toInt());
    }
}

void VaultWindow::onRestoreBtnClicked() {
    auto proxyIndex = view->list()->currentIndex();
    if (proxyIndex.isValid()) {
        auto sourceIndex = view->proxyModel()->mapToSource(proxyIndex);
        storage_core->restoreFile(sourceIndex.data(ViewItemRoles::FileId).toInt());
    }
}

void VaultWindow::onOpenBtnClicked() {
    int fileId = 0;
    auto proxyIndex = view->list()->currentIndex();
    if (proxyIndex.isValid()) {
        auto sourceIndex = view->proxyModel()->mapToSource(proxyIndex);
        fileId = sourceIndex.data(ViewItemRoles::FileId).toInt();
    }

    QString filePath;
    auto metadata = Core::Vault::FileRepository::fetchFile(fileId);
    if (!metadata.has_value())
        return;

    filePath = (*metadata).decryptedPath;
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void VaultWindow::onViewItemClicked(const QModelIndex &index) {
    using FS = Core::Vault::FileStatus;
    
    int _fileId = index.data(ViewItemRoles::FileId).toInt();
    auto status = index.data(ViewItemRoles::FileStatus).value<Core::Vault::FileStatus>();
    
    toolbar->encryptButton()->setEnabled(status == FS::Imported || status == FS::Decrypted);
    toolbar->decryptButton()->setEnabled(status == FS::Encrypted);
    toolbar->restoreButton()->setEnabled(status == FS::Decrypted || status == FS::Imported);
    toolbar->deleteButton()->setEnabled(status == FS::Imported || status == FS::Decrypted || status == FS::Failed);
    toolbar->openButton()->setEnabled(status == FS::Decrypted);

    _statusbar->selectedItemsLabel()->setText(QString("%1 item selected").arg(1));
    // _statusbar->totalSizeLabel()->setText(
    //     index.data(ViewItemRoles::FileSize).toString()
    // );
}

void VaultWindow::onImportQueued(const Core::Vault::FileMetadata &metadata) {
    view->model().appendRow(new Ui::Vault::ViewItem(metadata));
}

void VaultWindow::onDeleteQueued(int fileId) {
    updateModelItemData(
        fileId,
        QVariant::fromValue(Core::Vault::FileStatus::Queued),
        ViewItemRoles::FileStatus
    );
}

void VaultWindow::updateModelItemData(int fileId, const QVariant &variant, int role) {
    auto &model = view->model();

    for (auto i = 0; i < model.rowCount(); i++) {

        QModelIndex index = model.index(i, 0);
        int storedFileId = index.data(ViewItemRoles::FileId).toInt();

        if (storedFileId == fileId) {
            model.setData(index, variant, role);

            if (role == ViewItemRoles::FileStatus) {
                auto status = variant.value<Core::Vault::FileStatus>();
                if (status == Core::Vault::FileStatus::Deleted || status == Core::Vault::FileStatus::Restored) {
                    model.removeRow(index.row());
                    _statusbar->selectedItemsLabel()->setText("");
                }
            } 

            view->list()->update(index);

            // Refresh toolbar state
            auto currentIndex = view->list()->currentIndex();
            if (currentIndex.isValid())
                onViewItemClicked(currentIndex);

            break;
        }
    }
}

void VaultWindow::onImportStatusChanged(int fileId, const Core::Vault::FileStatus &status) {
    updateModelItemData(fileId, QVariant::fromValue(status), ViewItemRoles::FileStatus);
}

void VaultWindow::onImportProgressChanged(int fileId, int progress) {
    updateModelItemData(fileId, progress, ViewItemRoles::ProgressCurrentValue);
}

void VaultWindow::onDeleteStatusChanged(int fileId, const Core::Vault::FileStatus &status) {
    updateModelItemData(fileId, QVariant::fromValue(status), ViewItemRoles::FileStatus);
}

void VaultWindow::onDeleteProgressChanged(int fileId, int progress) {
    updateModelItemData(fileId, progress, ViewItemRoles::ProgressCurrentValue);
}

void VaultWindow::onEncryptQueued(int fileId) {
    updateModelItemData(
        fileId,
        QVariant::fromValue(Core::Vault::FileStatus::Queued),
        ViewItemRoles::FileStatus
    );
}

void VaultWindow::onEncryptStatusChanged(int fileId, const Core::Vault::FileStatus &status) {
    updateModelItemData(fileId, QVariant::fromValue(status), ViewItemRoles::FileStatus);
}

void VaultWindow::onEncryptProgressChanged(int fileId, int progress) {
    updateModelItemData(fileId, progress, ViewItemRoles::ProgressCurrentValue);
}

void VaultWindow::onDecryptQueued(int fileId) {
    updateModelItemData(
        fileId,
        QVariant::fromValue(Core::Vault::FileStatus::Queued),
        ViewItemRoles::FileStatus
    );
}

void VaultWindow::onDecryptStatusChanged(int fileId, const Core::Vault::FileStatus &status) {
    updateModelItemData(fileId, QVariant::fromValue(status), ViewItemRoles::FileStatus);
}

void VaultWindow::onDecryptProgressChanged(int fileId, int progress) {
    updateModelItemData(fileId, progress, ViewItemRoles::ProgressCurrentValue);
}

void VaultWindow::onRestoreQueued(int fileId) {
    updateModelItemData(
        fileId,
        QVariant::fromValue(Core::Vault::FileStatus::Queued),
        ViewItemRoles::FileStatus
    );
}

void VaultWindow::onRestoreStatusChanged(int fileId, const Core::Vault::FileStatus &status) {
    updateModelItemData(fileId, QVariant::fromValue(status), ViewItemRoles::FileStatus);
}

void VaultWindow::onRestoreProgressChanged(int fileId, int progress) {
    updateModelItemData(fileId, progress, ViewItemRoles::ProgressCurrentValue);
}

void VaultWindow::onRowsInserted(const QModelIndex &, int, int) {
    _statusbar->totalItemsLabel()->setText(QString("%1 items").arg(view->model().rowCount()));
}

void VaultWindow::onRowsRemoved(const QModelIndex &, int, int) {
    _statusbar->totalItemsLabel()->setText(QString("%1 items").arg(view->model().rowCount()));
}