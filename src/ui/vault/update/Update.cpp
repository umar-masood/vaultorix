#include "Update.h"
#include "../../../core/theme/ThemeManager.h"

/* ---------------------------------------------------------------
                         Update Info Widget
   --------------------------------------------------------------- */
Update::Update(const QString &currentVersion, 
               const QString &newVersion,
               const QString &size, 
               const QDateTime releasedDate, 
               const QString &updateNotes) : 
               _currentVersion(currentVersion),
               _newVersion(newVersion),
               _size(size),
               _releasedDate(releasedDate),
               _updateNotes(updateNotes){}


UpdateInfo::UpdateInfo(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_TranslucentBackground);

    // Update Version Date Info Label
    update_ver_date_label = new Label("Segoe UI", 10);
    update_ver_date_label->setAlignment(Qt::AlignLeft);

    // Seperator
    sep = new Seperator( 1, width(), Qt::Horizontal);

    // What's new label
    whats_new_label = new Label("Segoe UI", 10, QFont::Medium, false, "What's new:", Qt::AlignLeft);

    // Update notes label
    update_notes_label = new Label("Segoe UI", 10);
    update_notes_label->setWordWrap(true);
    update_notes_label->setAlignment(Qt::AlignLeft);

    // Layout
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(update_ver_date_label, 0, Qt::AlignLeft);
    layout->addSpacing(8);
    layout->addWidget(sep, 0, Qt::AlignHCenter);
    layout->addSpacing(8);
    layout->addWidget(whats_new_label, 0, Qt::AlignLeft);
    layout->addSpacing(8);
    layout->addWidget(update_notes_label, 0, Qt::AlignLeft);
    layout->addStretch();
    
    // Setting Default Update
    setUpdate({"1.0.0", "1.1.0", "56 MB", QDateTime::currentDateTime(),  "Added dark mode\nImproved performance\nFixed login bug"});

    // Initial Theme
    setDarkMode(isDarkMode);
}

void UpdateInfo::setDarkMode(bool enable) {
    isDarkMode = enable;

    if (whats_new_label)
        whats_new_label->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");

    if (sep)
        sep->setColor(isDarkMode ? "#334155" : "#E5E7EB");

    for (auto *label : {update_ver_date_label, update_notes_label})
        label->setTextColor(isDarkMode ? "#94A3B8" : "#6B7280");
}

void UpdateInfo::setUpdate(const Update &update) {
    if (update_ver_date_label)
        update_ver_date_label->setText(QString("Update available\
                                              \nVersion %1  →  %2\
                                              \n%3  •  Released %4\
                                        ").arg(update._currentVersion)
                                          .arg(update._newVersion)
                                          .arg(update._size)
                                          .arg(update._releasedDate.toString("dd-MMM-yyyy")));

    if (update_notes_label) 
        update_notes_label->setText(update._updateNotes);
    
    adjustSize();
}

void UpdateInfo::setUpdateAvailable(bool isAvailable) {
    emit isUpdateAvailable(isAvailable);
}

/* ---------------------------------------------------------------
                        App Update Subwindow
   --------------------------------------------------------------- */

AppUpdates::AppUpdates(QWidget *parent) : SubWindow(QSize(300, 300), parent) {
    setFocusPolicy(Qt::StrongFocus);
    setModal(true);

    // ---------------- Titlebar Layout ----------------
    QHBoxLayout *_titlebarLayout = this->titlebarLayout();

    winTitle = new Label("Segoe UI", 10, QFont::Normal, false, "Vaultorix Updates", Qt::AlignLeft);

    _titlebarLayout->addSpacing(6);
    _titlebarLayout->addWidget(winTitle, 0, Qt::AlignLeft | Qt::AlignVCenter);
    _titlebarLayout->addStretch();

    // ---------------- Content Area Layout ----------------
    win_content_area_layout = new QVBoxLayout(contentArea());
    win_content_area_layout->setSpacing(0);
    win_content_area_layout->setContentsMargins(0,0,0,0);

    // ---------------- Stacked Widget ----------------
    stacked_widget = new QStackedWidget;
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

    auto *spinner_layout = new QVBoxLayout(spinner_wrapper);
    spinner_layout->setContentsMargins(0,0,0,0);
    spinner_layout->setSpacing(8);

    spinner = new SpinnerProgress;
    spinner->setIndeterminate(true);
    spinner->setFixedSize(QSize(100, 140));
    spinner->setColor(SpinnerProgress::BackgroundLight, Qt::transparent);
    spinner->setColor(SpinnerProgress::BackgroundDark, Qt::transparent);

    spinner_label = new Label("Segoe UI", 10, QFont::Normal, false, "Checking for update");

    spinner_layout->addWidget(spinner, 0, Qt::AlignHCenter);
    spinner_layout->addWidget(spinner_label, 0, Qt::AlignHCenter);

    checking_layout->addStretch();
    checking_layout->addWidget(spinner_wrapper, 0, Qt::AlignCenter);
    checking_layout->addStretch();

    stacked_widget->addWidget(checking_page);

    /* ===============================================================
                            PAGE 2 - NO UPDATE
       =============================================================== */

    no_update_page = new QWidget;
    auto *no_update_layout = new QVBoxLayout(no_update_page);
    no_update_layout->setSpacing(8);
    no_update_layout->setContentsMargins(14, 34, 14, 14);

    no_update_label = new Label("Segoe UI", 10, QFont::Normal, false, "There is no update available.");

    no_update_layout->addStretch();
    no_update_layout->addWidget(no_update_label, 0, Qt::AlignCenter);
    no_update_layout->addStretch();

    stacked_widget->addWidget(no_update_page);

    /* ===============================================================
                    PAGE 3 - UPDATE AVAILABLE
   =============================================================== */
    update_page = new QWidget;
    auto *update_layout = new QVBoxLayout(update_page);
    update_layout->setSpacing(8);
    update_layout->setContentsMargins(14, 34, 14, 10);

    update_details_widget = new UpdateInfo;
    sep = new Seperator(1, width() - 28, Qt::Horizontal);

    update_layout->addWidget(update_details_widget, 0, Qt::AlignHCenter);
    update_layout->addWidget(sep, 0, Qt::AlignHCenter | Qt::AlignBottom);

    /* ---------------- Action Stack (Button / Progress) ---------------- */

    action_stack = new QStackedWidget;

    /* ---------- Button Page ---------- */

    action_button_page = new QWidget;
    auto *btn_layout = new QVBoxLayout(action_button_page);
    btn_layout->setContentsMargins(0,0,0,0);

    update_btn = new Button("Update now");
    update_btn->setDisplayMode(Button::TextOnly);
    update_btn->setFixedSize(QSize(width() - 28, 36));
    update_btn->setGradientColors("#008EDE", "#15F2FF", "#008EDE");
    update_btn->setFontProperties("Segoe UI", 11, QFont::Normal);
    update_btn->setCursor(Qt::PointingHandCursor);

    btn_layout->addWidget(update_btn, 0, Qt::AlignHCenter);

    /* ---------- Progress Page ---------- */

    action_progress_page = new QWidget;
    auto *progress_layout = new QVBoxLayout(action_progress_page);
    progress_layout->setSpacing(6);
    progress_layout->setContentsMargins(0,0,0,0);

    downloading_label = new Label("Segoe UI", 10, QFont::Normal, false, "Downloading");

    update_progressbar = new LineProgress;
    update_progressbar->setFixedSize(QSize(width() - 28, 20));

    progress_layout->addWidget(downloading_label, 0, Qt::AlignHCenter);
    progress_layout->addWidget(update_progressbar, 0, Qt::AlignHCenter | Qt::AlignBottom);

    // Closing app after downloading
    closing_app_label = new Label("Segoe UI", 10, QFont::Normal, false, "Download completed. We're closing this app to install update");
    closing_app_label->setWordWrap(true);
    closing_app_label->hide();
   
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
                            Timer
       =============================================================== */

    timer = new QTimer;
    timer->setInterval(30);

    connect(timer, &QTimer::timeout, this, [this]{
        if (v >= 100)
            v = 0;

        v++;
        update_progressbar->setValue(v);
        update_progressbar->setText(QString::number(v));
    });

    /* ===============================================================
                            Connections
       =============================================================== */

    connect(update_details_widget, &UpdateInfo::isUpdateAvailable, this, [this](bool isAvailable) {
        spinner->stop();

        if (isAvailable)
            stacked_widget->setCurrentWidget(update_page);
        else
            stacked_widget->setCurrentWidget(no_update_page);
    });

    connect(update_btn, &Button::clicked, this, [this] {
        action_stack->setCurrentWidget(action_progress_page);
        update_progressbar->start();
        timer->start();
    });

    connect(update_progressbar, &LineProgress::completed, this, [this]{
        downloading_label->hide();
        update_progressbar->hide();
        closing_app_label->show();
    });

    // Initial page
    stacked_widget->setCurrentWidget(checking_page);

    // Theme
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &AppUpdates::setDarkMode);
    setDarkMode(tm.isDarkMode());
}

UpdateInfo* AppUpdates::updateInfoWidget() const {
    return update_details_widget;
}

void AppUpdates::setDarkMode(bool isDarkMode) {
    // Seperator
    if (sep)
        sep->setColor(isDarkMode ? "#334155" : "#E5E7EB");

    // Update Details Widget
    update_details_widget->setDarkMode(isDarkMode);

    // Spinner
    if (spinner)
        spinner->setDarkMode(isDarkMode);

    // Update Progress Bar
    if (update_progressbar)
        update_progressbar->setDarkMode(isDarkMode);

    // Window Title, Spinner & No Update Labels
    for (auto *label : {winTitle, spinner_label, no_update_label, downloading_label, closing_app_label}) 
        label->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");

    // Window Theme
    SubWindow::setDarkMode(isDarkMode);
}

void AppUpdates::showEvent(QShowEvent *event) {
    SubWindow::showEvent(event);

    stacked_widget->setCurrentWidget(checking_page);

    if (spinner)
        spinner->start();
}