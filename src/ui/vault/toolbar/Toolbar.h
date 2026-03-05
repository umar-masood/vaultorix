#pragma once

#include "../../components/Button.h"
#include "../user/User.h"

#include <QHBoxLayout>
#include <QList>
#include <QMap>

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

    // Encrypt Action
    Button *encrypt_btn = nullptr;

    // Decrypt Action
    Button *decrypt_btn = nullptr;

    // Open Action
    Button *open_btn = nullptr;

    // Delete Action
    Button *delete_btn = nullptr;

    // Restore Action
    Button *restore_btn = nullptr;

    // User Profile Widget
    User *user_widget = nullptr;
    
    // Helpers
    Button* createButton(const QString &text, const QString &iconPath);

    // Icons
    QString DecryptIcon, EncryptIcon , DeleteIcon , ImportIcon , OpenIcon, RestoreIcon; 

    // List of Actions
    QList<Button *> actions;

    protected:
    void paintEvent(QPaintEvent *event) override;
};