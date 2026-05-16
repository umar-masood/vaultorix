#include "ToolBar.h"

#include "../../../core/theme/ThemeManager.h"
#include "../../../core/session/SessionManager.h"

#include "../../../../resources/IconManager.h"
#include "../../components/Button.h"
#include "../user/User.h"

#include <QHBoxLayout>

using Ui::Vault::Toolbar;
Toolbar::Toolbar(QWidget *parent) : QWidget(parent) {
    setFixedHeight(64);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Icons
    DecryptIcon   = IconManager::icon(Icons::Decrypt);
    EncryptIcon   = IconManager::icon(Icons::Encrypt);
    DeleteIcon    = IconManager::icon(Icons::FileDelete);
    ImportIcon    = IconManager::icon(Icons::Import);
    OpenIcon      = IconManager::icon(Icons::Open);
    RestoreIcon   = IconManager::icon(Icons::Restore);

    // Import Action
    import_btn = createButton("Import", ImportIcon);
    import_btn->setEnabled(true);
    import_btn->setToolTip("Import files into the vault");

    // Encrypt Action
    encrypt_btn = createButton("Encrypt", EncryptIcon);
    encrypt_btn->setToolTip("Encrypt the selected file");

    // Decrypt Action
    decrypt_btn = createButton("Decrypt", DecryptIcon);
    decrypt_btn->setToolTip("Decrypt the selected file");

    // Open Action
    open_btn = createButton("Open", OpenIcon);
    open_btn->setToolTip("Open the selected file");

    // Delete Action
    delete_btn = createButton("Delete", DeleteIcon);
    delete_btn->setToolTip("Permanently delete the selected file");

    // Restore Action
    restore_btn = createButton("Restore", RestoreIcon);
    restore_btn->setToolTip("Remove the selected file from vault and return to its original location");

    // User Profile Widget
    user_widget = new Ui::Vault::User();
    user_widget->setName(SessionManager::instance().fullName());
    connect(&SessionManager::instance(), &SessionManager::userInfoChanged, this, [this](){
        user_widget->setName(SessionManager::instance().fullName());
    });

    // Layout
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(4, 0, 4, 0);
    layout->setSpacing(0);

    // Adding actions to QList
    actions = {import_btn ,encrypt_btn ,decrypt_btn ,open_btn ,delete_btn ,restore_btn};

    for (auto action : actions) {
        layout->addWidget(action, 0, Qt::AlignVCenter);
        layout->addSpacing(4);
    }

    layout->addStretch();
    layout->addWidget(user_widget, 0, Qt::AlignVCenter | Qt::AlignRight);

    // Theme
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &Toolbar::setDarkMode);
    setDarkMode(tm.isDarkMode());
}

void Toolbar::setDarkMode(bool isDarkMode) {
    // Actions Buttons Theme
    for (auto action : actions) 
        action->setDarkMode(isDarkMode);
    
    update();
}

Button* Toolbar::decryptButton() const { return decrypt_btn; }
Button* Toolbar::encryptButton() const { return encrypt_btn; }
Button* Toolbar::deleteButton()  const { return delete_btn; }
Button* Toolbar::openButton()    const { return open_btn; }
Button* Toolbar::importButton()  const { return import_btn; }
Button* Toolbar::restoreButton() const { return restore_btn; }

Button* Toolbar::createButton(const QString &text, const QString &iconPath) {
    auto *btn = new Button(text);
    btn->setFixedSize(QSize(70, 58));
    btn->setCursor(Qt::PointingHandCursor);
    btn->setDisplayMode(Button::TextUnderIcon);
    btn->setIconSize(QSize(24,24));
    btn->setSecondary(true);
    btn->setFontProperties("Segoe UI", 10);
    btn->setBorderTransparent(true);
    btn->setIconPaths(iconPath, iconPath);
    btn->setNormalBackgroundTransparent(true);
    btn->setEnabled(false);
    return btn;
}

void Toolbar::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // Background
    painter.setBrush(QBrush(ThemeManager::instance().isDarkMode() ? "#262626" : "#F9F9F9"));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 6, 6);
}