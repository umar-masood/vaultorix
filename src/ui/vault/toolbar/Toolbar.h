#pragma once

#include "../../components/Button.h"
#include "../../components/ToolTip.h"
#include "../../../resources/IconManager.h"
#include "../user/User.h"

#include <QHBoxLayout>
#include <QList>
#include <QMap>
#include <QWidget>

/* ---------------------  Toolbar ---------------------- */
class Toolbar : public QWidget {
    Q_OBJECT

    public:
    Toolbar(QWidget *parent = nullptr);
    void setDarkMode(bool enable);

    Button* importButton() const;
    Button* restoreButton() const;
    Button* openButton() const;
    Button* deleteButton() const;
    Button* encryptButton() const;
    Button* decryptButton() const;

    private:
    // Theme Mode
    bool isDarkMode = false;

    // Main Layout
    QHBoxLayout *layout = nullptr;

    // Import Action
    Button *import_btn = nullptr;
    // ToolTip
    ToolTip *import_btn_tip = nullptr;

    // Encrypt Action
    Button *encrypt_btn = nullptr;
    // ToolTip
    ToolTip *encrypt_btn_tip = nullptr;

    // Decrypt Action
    Button *decrypt_btn = nullptr;
    // ToolTip
    ToolTip *decrypt_btn_tip = nullptr;

    // Open Action
    Button *open_btn = nullptr;
    // ToolTip
    ToolTip *open_btn_tip = nullptr;

    // Delete Action
    Button *delete_btn = nullptr;
    // ToolTip
    ToolTip *delete_btn_tip = nullptr;

    // Restore Action
    Button *restore_btn = nullptr;
    // ToolTip
    ToolTip *restore_btn_tip = nullptr;

    // User Profile Widget
    User *user_widget = nullptr;
    
    // Helpers
    Button* createButton(const QString &text, const QString &iconPath);

    // Icons
    const QString DecryptIcon   = IconManager::icon(Icons::Decrypt);
    const QString EncryptIcon   = IconManager::icon(Icons::Encrypt);
    const QString DeleteIcon    = IconManager::icon(Icons::FileDelete);
    const QString ImportIcon    = IconManager::icon(Icons::Import);
    const QString OpenIcon      = IconManager::icon(Icons::Open);
    const QString RestoreIcon   = IconManager::icon(Icons::Restore);

    // List of Actions
    QList<Button *> actions;

    protected:
    void paintEvent(QPaintEvent *event) override;
};