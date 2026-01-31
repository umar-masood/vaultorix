#include "ToolBar.h"

Toolbar::Toolbar(QWidget *parent) : QWidget(parent) {
    setFixedHeight(64);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Import Action
    import_btn = createButton("Import");

    // ToolTip
    import_btn_tip = new ToolTip(import_btn);
    import_btn_tip->setText("Import files into the vault");

    // Encrypt Action
    encrypt_btn = createButton("Encrypt");
    // ToolTip
    encrypt_btn_tip = new ToolTip(encrypt_btn);
    encrypt_btn_tip->setText("Encrypt the selected file");

    // Decrypt Action
    decrypt_btn = createButton("Decrypt");
    // ToolTip
    decrypt_btn_tip = new ToolTip(decrypt_btn);
    decrypt_btn_tip->setText("Decrypt the selected file");

    // Open Action
    open_btn = createButton("Open");
    // ToolTip
    open_btn_tip = new ToolTip(open_btn);
    open_btn_tip->setText("Open the selected file");

    // Delete Action
    delete_btn = createButton("Delete");
    // ToolTip
    delete_btn_tip = new ToolTip(delete_btn);
    delete_btn_tip->setText("Permanently delete the selected file");

    // Restore Action
    restore_btn = createButton("Restore");
    // ToolTip
    restore_btn_tip = new ToolTip(restore_btn);
    restore_btn_tip->setText("Remove the selected file from vault and return to its original location");

    // User Profile Widget
    user_widget = new User;

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

    setDarkMode(isDarkMode);
}

void Toolbar::setDarkMode(bool enable) {
    isDarkMode = enable;

    // User Profile Widget Theme
    user_widget->setDarkMode(enable);
    
    // Actions Buttons Theme
    for (auto action : actions) 
        action->setDarkMode(enable);

    // Actions Tooltips Theme
    for (auto tip : {import_btn_tip ,encrypt_btn_tip ,decrypt_btn_tip ,open_btn_tip ,delete_btn_tip ,restore_btn_tip})
        tip->setDarkMode(enable);
}

Button* Toolbar::decryptButton() const { return decrypt_btn; }
Button* Toolbar::encryptButton() const { return encrypt_btn; }
Button* Toolbar::deleteButton() const { return delete_btn; }
Button* Toolbar::openButton() const { return open_btn; }
Button* Toolbar::importButton() const { return import_btn; }

Button* Toolbar::createButton(const QString &text) {
    Button *btn = new Button(text);
    btn->setFixedSize(QSize(70, 58));
    btn->setDisplayMode(Button::TextUnderIcon);
    btn->setIconSize(QSize(24,24));
    btn->setSecondary(true);
    btn->setFontProperties("Segoe UI", 10);
    btn->setBorderHidden(true);
    btn->setIconPaths(icons[text], icons[text]);
    btn->setNormalBackgroundTransparent(true);
    return btn;
}

void Toolbar::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    painter.setBrush(QBrush(isDarkMode ? "#252424" : "#F9F9F9"));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 6, 6);
}

