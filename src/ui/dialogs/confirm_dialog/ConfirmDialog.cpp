#include "ConfirmDialog.h"

#include "../../../core/theme/ThemeManager.h"

#include "../../components/Dialog.h"
#include "../../components/Button.h"
#include "../../components/Label.h"

#include <QVBoxLayout>
#include <QSize>
#include <QString>

/* ──────────────────────── Confirm Widget ──────────────────────── */
ConfirmWidget::ConfirmWidget(const QString &text,
                 const QString &illustrationLight,
                 const QString &illustrationDark,
                 QWidget *parent)
                : QWidget(parent), 
                lightIcon(illustrationLight), 
                darkIcon(illustrationDark)
{
    setAttribute(Qt::WA_TranslucentBackground);

    layout = new QVBoxLayout(this);
    layout->setSpacing(0);

    acceptBtn = new Button;
    acceptBtn->setDisplayMode(Button::TextOnly);
    acceptBtn->setFixedSize(QSize(316, 36));
    acceptBtn->setText(tr("Yes"));
    acceptBtn->setFontProperties("Segoe UI", 10, QFont::DemiBold);

    discardBtn = new Button;
    discardBtn->setSecondary(true);
    discardBtn->setDisplayMode(Button::TextOnly);
    discardBtn->setFixedSize(QSize(316, 36));
    discardBtn->setText(tr("No"));
    discardBtn->setFontProperties("Segoe UI", 10, QFont::DemiBold);

    QPixmap pixmap(lightIcon);
    illustration = new Label(true);
    illustration->setFixedSize(pixmap.size());
    illustration->setPixmap(pixmap);

    if (!text.isEmpty()) {
        label = new Label("Segoe UI", 10);
        label->setFixedSize(QSize(290, 36));
        label->setWordWrap(true);
        label->setText(text);
    }

    layout->addStretch();
    layout->addWidget(illustration, 0, Qt::AlignCenter);
    layout->addSpacing(2);

    if (label) {
        layout->addStretch();
        layout->addWidget(label, 0, Qt::AlignCenter);
        layout->addSpacing(12);
    }

    layout->addWidget(acceptBtn, 0, Qt::AlignCenter);
    layout->addSpacing(6);
    layout->addWidget(discardBtn, 0, Qt::AlignCenter);
    layout->addStretch();

    setFixedWidth(342);
    adjustSize();

    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &ConfirmWidget::setDarkMode);
    setDarkMode(tm.isDarkMode());
}

void ConfirmWidget::setDarkMode(bool value) {
    isDarkMode = value;
    if (illustration)
        illustration->setPixmap(QPixmap(isDarkMode ? darkIcon : lightIcon));
    if (label)
        label->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");
    if (discardBtn)
        discardBtn->setDarkMode(isDarkMode);
}

Button* ConfirmWidget::acceptButton() { return acceptBtn; }
Button* ConfirmWidget::discardButton() { return discardBtn; }

/* ──────────────────── Confirm Dialog Manager ──────────────────── */

ConfirmDialogManager::ConfirmDialogManager(QObject *parent) : QObject(parent) {
    create("AccountDelete",
           tr("This action is irreversible. Are you sure you want to proceed with deleting your account?"),
           tr("Confirm"), tr("Cancel"),
           DeleteAccountIllustration,
           true
        );

    create("FileDelete",
           tr("Are you sure you want to permanently delete this file? Once deleted, it cannot be recovered."),
           tr("Confirm"), tr("Cancel"),
           DeleteAccountIllustration,
           true
        );
}

ConfirmDialogManager* ConfirmDialogManager::instance() {
    static auto *manager = new ConfirmDialogManager(qApp);
    return manager;
}

void ConfirmDialogManager::create(const QString &key,
                                  const QString &text,
                                  const QString &acceptButtonText,
                                  const QString &discardButtonText,
                                  const QString &iconPath,
                                  bool dangerZone)
{
    ConfirmDialogItem *dialog_item = new ConfirmDialogItem;

    dialog_item->widget = new ConfirmWidget(text, iconPath, iconPath, nullptr);
    dialog_item->dialog = nullptr;  

    auto discardBtn =  dialog_item->widget->discardButton();
    discardBtn->setText(discardButtonText);

    auto *acceptBtn = dialog_item->widget->acceptButton();
    acceptBtn->setText(acceptButtonText);

    if (dangerZone) {
        acceptBtn->setColor(Button::PrimaryNormal,   QColor("#F53649"));
        acceptBtn->setColor(Button::PrimaryHover,    QColor("#D92C3E"));
        acceptBtn->setColor(Button::PrimaryPressed,  QColor("#B81F30"));
        acceptBtn->setTextColor(Button::PrimaryText, QColor("#FFFFFF"));
        acceptBtn->setColor(Button::DisabledPrimary, QColor("#F5A1A8"));
        acceptBtn->setBorderTransparent(true);
    }

    connect(acceptBtn, &Button::clicked, this, [this, key]() {
        emit actionTriggered(key, tr("Accept"));
        close(key);
    });

    connect(discardBtn, &Button::clicked, this, [this, key]() {
        emit actionTriggered(key, tr("Discard"));
        close(key);
    });

    dialogs.insert(key, dialog_item);
}

void ConfirmDialogManager::show(const QString &dialogKey, const QString &windowKey) {
    // If dailog key is invalid
    if (!dialogs.contains(dialogKey))
        return;

    // If window key is invalid and no parent
    QWidget *parent = parentWindows.value(windowKey, nullptr);
    if (!parent)
        return;

    // Take dailog item from dailogs
    auto *item = dialogs.value(dialogKey);
    if (!item || !item->widget)
        return;

    item->dialog = new Dialog(item->widget, parent, false);

    // Theme Handling
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, item->dialog, &Dialog::setDarkMode);
    item->dialog->setDarkMode(tm.isDarkMode());

    // Showing
    item->dialog->show();
}

void ConfirmDialogManager::close(const QString &key) {
    if (!dialogs.contains(key))
        return;

    ConfirmDialogItem *item = dialogs[key];

    if (!item || !item->dialog)
        return;

    item->widget->setParent(nullptr);
    item->dialog->close();
    item->dialog->deleteLater();
}

void ConfirmDialogManager::registerWindow(const QString &key, QWidget *window) {
    if (window)
        parentWindows[key] = window;
}