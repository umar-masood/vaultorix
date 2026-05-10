#include "Update.h"
#include "../../../core/theme/ThemeManager.h"

#include "../../dialogs/error_dialog/ErrorDialog.h"
#include "../../components/LineProgress.h"
#include "../../components/SpinnerProgress.h"
#include "../../components/Button.h"
#include "../../components/Seperator.h"
#include "../../components/Label.h"

#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

using Ui::Vault::Update;
using Ui::Vault::AppUpdate;
/* ---------------------------------------------------------------
                         Update Struct
   --------------------------------------------------------------- */
Update::Update(const QString &currentVersion, 
               const QString &newVersion,
               const QString &size, 
               const QDateTime &releasedDate, 
               const QString &updateNotes) : 
               _currentVersion(currentVersion),
               _newVersion(newVersion),
               _size(size),
               _releasedDate(releasedDate),
               _updateNotes(updateNotes){}

/* ---------------------------------------------------------------
                        App Update 
   --------------------------------------------------------------- */
AppUpdate::AppUpdate(QWidget *parent) : SubWindow(QSize(300, 300), parent) {
    setFocusPolicy(Qt::StrongFocus);
    setModal(true);

    // ---------------- Titlebar Layout ----------------
    QHBoxLayout *_titlebarLayout = this->titlebarLayout();

    // Window Title
    winTitle = new Label("Segoe UI", 10, QFont::Normal, false, "Vaultorix Updates", Qt::AlignLeft);

    // Adding to titlebar layout
    _titlebarLayout->addSpacing(6);
    _titlebarLayout->addWidget(winTitle, 0, Qt::AlignLeft | Qt::AlignVCenter);
    _titlebarLayout->addStretch();

    // ---------------- Content Area Layout ----------------
    win_content_area_layout = new QVBoxLayout(contentArea());
    win_content_area_layout->setSpacing(0);
    win_content_area_layout->setContentsMargins(0,0,0,0);

    // ---------------- Stacked Widget ----------------------
    stacked_widget = new QStackedWidget;

    // Adding stacked layout to window content layout
    win_content_area_layout->addWidget(stacked_widget);

    /* ===============================================================
                            PAGE 1 - CHECKING
       =============================================================== */
    checking_page = new QWidget;
    auto *checking_layout = new QVBoxLayout(checking_page);
    checking_layout->setSpacing(8);

    // Spinner Wrapper
    spinner_wrapper = new QWidget;
    spinner_wrapper->setFixedSize(160,160);
    spinner_wrapper->setAttribute(Qt::WA_TranslucentBackground);

    // Spinner Layout
    auto *spinner_layout = new QVBoxLayout(spinner_wrapper);
    spinner_layout->setContentsMargins(0,0,0,0);
    spinner_layout->setSpacing(8);

    // Spinner
    spinner = new SpinnerProgress;
    spinner->setIndeterminate(true);
    spinner->setFixedSize(QSize(100, 140));
    spinner->setColor(SpinnerProgress::BackgroundLight, Qt::transparent);
    spinner->setColor(SpinnerProgress::BackgroundDark, Qt::transparent);

    // Spinner Label
    spinner_label = new Label("Segoe UI", 10, QFont::Normal, false, tr("Checking for update"));

    // Adding to spinner layout
    spinner_layout->addWidget(spinner, 0, Qt::AlignHCenter);
    spinner_layout->addWidget(spinner_label, 0, Qt::AlignHCenter);

    // Adding to checking layout
    checking_layout->addStretch();
    checking_layout->addWidget(spinner_wrapper, 0, Qt::AlignCenter);
    checking_layout->addStretch();

    // Adding check_page widget to stacked widget
    stacked_widget->addWidget(checking_page);

    /* ===============================================================
                            PAGE 2 - NO UPDATE
       =============================================================== */
    no_update_page = new QWidget;

    // Layout
    auto *no_update_layout = new QVBoxLayout(no_update_page);
    no_update_layout->setSpacing(8);
    no_update_layout->setContentsMargins(10, 34, 14, 10);

    // No Update Label
    no_update_label = new Label("Segoe UI", 10, QFont::Normal, false, tr("Something went wrong."));
    no_update_label->setMinimumWidth(250);
    no_update_label->setWordWrap(true);

    // Adding no update label to layout
    no_update_layout->addStretch();
    no_update_layout->addWidget(no_update_label, 0, Qt::AlignCenter);
    no_update_layout->addStretch();

    // Adding widget to stacked layout
    stacked_widget->addWidget(no_update_page);

    /* ===============================================================
                    PAGE 3 - UPDATE AVAILABLE
   =============================================================== */
    update_page = new QWidget;
    
    // Layout
    auto *update_layout = new QVBoxLayout(update_page);
    update_layout->setSpacing(8);
    update_layout->setContentsMargins(14, 34, 14, 10);

    // Seperator
    main_sep = new Seperator(1, width() - 28, Qt::Horizontal);

    // Adding updateInfoWidget, main seperator to update layout
    update_layout->addWidget(updateInfoWidget(), 0, Qt::AlignHCenter);
    update_layout->addWidget(main_sep, 0, Qt::AlignHCenter | Qt::AlignBottom);

    /* ---------------- Action Stack (Button / Progress) ---------------- */
    action_stack = new QStackedWidget;

    /* ---------- Button Page ---------- */
    action_button_page = new QWidget;

    // Layout
    auto *btn_layout = new QVBoxLayout(action_button_page);
    btn_layout->setContentsMargins(0,0,0,0);

    // Update now button
    update_btn = new Button(tr("Update now"));
    update_btn->setDisplayMode(Button::TextOnly);
    update_btn->setFixedSize(QSize(width() - 28, 36));
    update_btn->setGradientColors("#008EDE", "#15F2FF", "#008EDE");
    update_btn->setFontProperties("Segoe UI", 11, QFont::Normal);
    update_btn->setCursor(Qt::PointingHandCursor);

    // Adding button to button layout
    btn_layout->addWidget(update_btn, 0, Qt::AlignHCenter);

    /* ---------- Progress Page ---------- */
    action_progress_page = new QWidget;

    // Layout
    auto *progress_layout = new QVBoxLayout(action_progress_page);
    progress_layout->setSpacing(6);
    progress_layout->setContentsMargins(0,0,0,0);

    // Downloading Label
    downloading_label = new Label("Segoe UI", 10, QFont::Normal, false, tr("Downloading"));

    // Download Line Progress
    download_progressbar = new LineProgress;
    download_progressbar->setFixedSize(QSize(width() - 28, 20));

    // Closing app label
    closing_app_label = new Label("Segoe UI", 10, QFont::Normal, false, tr("Download completed. We're closing this app to install update"));
    closing_app_label->setWordWrap(true);
    closing_app_label->hide();
   
    // Adding downloading, closing label to progress label    
    progress_layout->addWidget(downloading_label, 0, Qt::AlignHCenter);
    progress_layout->addWidget(download_progressbar, 0, Qt::AlignHCenter | Qt::AlignBottom);
    progress_layout->addWidget(closing_app_label, 0, Qt::AlignCenter);

    /* ---------- Add To Action Stack ---------- */
    action_stack->addWidget(action_button_page);
    action_stack->addWidget(action_progress_page);

    /* ---------- Add To Update Layout ---------- */
    update_layout->addWidget(action_stack, 0, Qt::AlignHCenter | Qt::AlignBottom);

    /* ---------- Add Update Page To Main Stack ---------- */
    stacked_widget->addWidget(update_page);

    /* ---------- Default Action Page ---------- */
    action_stack->setCurrentWidget(action_button_page);

    /* ===============================================================
                            Connections
       =============================================================== */
    connect(update_btn, &Button::clicked, this, [this] {
        action_stack->setCurrentWidget(action_progress_page);
    });

    connect(download_progressbar, &LineProgress::completed, this, [this]{
        downloading_label->hide();
        download_progressbar->hide();
        closing_app_label->show();
    });

    // Initial page
    stacked_widget->setCurrentWidget(checking_page);

    // Theme
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &AppUpdate::setDarkMode);
    setDarkMode(tm.isDarkMode());

    // Registering window for Error Dialog Manager
    ErrorDialogManager::instance()->registerWindow("AppUpdate", this);
}

QWidget* AppUpdate::updateInfoWidget() {
    QWidget *main_widget = new QWidget;
    main_widget->setAttribute(Qt::WA_TranslucentBackground);

    // Release date
    update_ver_date_label = new Label("Segoe UI", 10);
    update_ver_date_label->setAlignment(Qt::AlignLeft);

    // Seperator
    update_sep = new Seperator(1, width(), Qt::Horizontal);

    // Whats new 
    whats_new_label = new Label("Segoe UI", 10, QFont::Medium, false, tr("What's new:"), Qt::AlignLeft);

    // Release notes
    update_notes_label = new Label("Segoe UI", 10);
    update_notes_label->setWordWrap(true);
    update_notes_label->setAlignment(Qt::AlignLeft);

    // Layout
    auto *layout = new QVBoxLayout(main_widget);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);

    layout->addWidget(update_ver_date_label);
    layout->addSpacing(8);
    layout->addWidget(update_sep);
    layout->addSpacing(8);
    layout->addWidget(whats_new_label);
    layout->addSpacing(8);
    layout->addWidget(update_notes_label);
    layout->addStretch();

    return main_widget;
}

void AppUpdate::setDarkMode(bool isDarkMode) {
    // Seperators
    for (auto *sep : {main_sep, update_sep})
        sep->setColor(isDarkMode ? "#334155" : "#E5E7EB");

    // Spinner
    if (spinner)
        spinner->setDarkMode(isDarkMode);

    // Update Progress Bar
    if (download_progressbar)
        download_progressbar->setDarkMode(isDarkMode);

    // Window Title, Spinner & No Update Labels
    for (auto *label : {winTitle, spinner_label, no_update_label, downloading_label, closing_app_label, whats_new_label}) 
        label->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");

    // Update Widget labels
    for (auto *label : { update_ver_date_label, update_notes_label }) 
        if (label)
            label->setTextColor(isDarkMode ? "#94A3B8" : "#6B7280");
    
    // Window Theme
    SubWindow::setDarkMode(isDarkMode);
}

void AppUpdate::setUpdateDetails(const Update &update) {
    if (!update_ver_date_label || !update_notes_label)
        return;

    update_ver_date_label->setText(
        QString("Update available\n"
                "Version %1 → %2\n"
                "%3 • Released %4")
        .arg(update._currentVersion)
        .arg(update._newVersion)
        .arg(update._size)
        .arg(update._releasedDate.toString("dd-MMM-yyyy"))
    );

    update_notes_label->setText(update._updateNotes);
}

void AppUpdate::showEvent(QShowEvent *event) {
    SubWindow::showEvent(event);

    stacked_widget->setCurrentWidget(checking_page);

    if (spinner)
        spinner->start();
}

LineProgress* AppUpdate::downloadProgressBar() const {
    return download_progressbar;
}

Button* AppUpdate::updateButton() const {
    return update_btn;
}

void AppUpdate::setUpdateState(const UpdateState &state) {
    switch (state) {
        case UpdateState::Available:
        spinner->stop();
        stacked_widget->setCurrentWidget(update_page);
        break;

        case UpdateState::NotAvailable:
        spinner->stop();
        no_update_label->setText(tr("Your application is up to date."));
        stacked_widget->setCurrentWidget(no_update_page);
        break;

        case UpdateState::NoInternet:
        spinner->stop();
        no_update_label->setText(tr("Unable to check for updates. Please check your internet connection."));
        stacked_widget->setCurrentWidget(no_update_page);
        break;

        case UpdateState::SomethingWentWrong:
        spinner->stop();
        no_update_label->setText(tr("Oops! Something went wrong."));
        stacked_widget->setCurrentWidget(no_update_page);
        break;
    }
}
