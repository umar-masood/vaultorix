#include "About.h"

#include "../vault_window/VaultWindow.h"
#include "../../components/Dialog.h"
#include "../../components/Label.h"

About* About::instance(QWidget *parent) {
    static About *about = nullptr;

    if (!about)
        about = new About(parent);

    return about;
}

void About::show() {
    if (dialog)
        dialog->show();
}

void About::setDarkMode(bool enable) {
    isDarkMode = enable;

    for (auto *label : labels) 
        if (label)
            label->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");

    if (dialog)
        dialog->setDarkMode(isDarkMode);
}

About::About(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);

    // Icon
    auto *app_icon = new Label(true);
    app_icon->setFixedSize(QSize(80, 76));
    app_icon->setPixmap(QPixmap((IconManager::icon(Icons::AppIcon)))
                        .scaled(80, 76, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    // Labels
    app_name = new Label("Inter", 14, QFont::Bold, false, "Vaultorix");

    app_version = new Label("Segoe UI", 12, QFont::Normal, false,
                            "Version " + QCoreApplication::applicationVersion());

    tagline = new Label("Segoe UI", 12, QFont::Normal, false,
                        "Secure and Reliable File Encryption & Management");
    tagline->setWordWrap(true);

    developer = new Label("Segoe UI", 10, QFont::Light, false,
                          "Developed & Maintained by Umar Masood");
                          
    copyright = new Label("Segoe UI", 10, QFont::Light, false,
                          "© 2026 Umar Creations. All Rights Reserved");

    labels = {app_name, app_version, tagline, developer, copyright};

    // Layout
    auto *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 20, 0, 20);
    layout->addWidget(app_icon, 0, Qt::AlignHCenter);
    layout->addSpacing(18);
    layout->addWidget(app_name, 0, Qt::AlignHCenter);
    layout->addSpacing(6);
    layout->addWidget(app_version, 0, Qt::AlignHCenter);
    layout->addSpacing(18);
    layout->addWidget(tagline, 0, Qt::AlignHCenter);
    layout->addSpacing(18);
    layout->addWidget(developer, 0, Qt::AlignHCenter);
    layout->addSpacing(6);
    layout->addWidget(copyright, 0, Qt::AlignHCenter);
    layout->addStretch();

    setLayout(layout);

    // Adjust size
    setFixedWidth(320); // width fixed
    adjustSize();       // height auto-adjusts

    // Dialog
    dialog = new Dialog(this, VaultWindow::instance(), true);

    // Initial Theme
    setDarkMode(isDarkMode);
}