#include "Update.h"

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
    sep = new Seperator(nullptr, 1, width(), Qt::Horizontal);

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

    if (update_ver_date_label)
        update_ver_date_label->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");

    if (whats_new_label)
        whats_new_label->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");

    if (sep)
        sep->setColor(isDarkMode ? "#334155" : "#E5E7EB");

    if (update_notes_label)
        update_notes_label->setTextColor(isDarkMode ? "#94A3B8" : "#6B7280");
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

    // ---------------- Spinner ----------------
    // Wrapper
    spinner_wrapper = new QWidget;
    spinner_wrapper->setContentsMargins(0, 0, 0, 0);
    spinner_wrapper->setFixedSize(160,160);
    spinner_wrapper->setAttribute(Qt::WA_TranslucentBackground);

    // Wrapper Layout
    auto *spinner_layout = new QVBoxLayout(spinner_wrapper);
    spinner_layout->setContentsMargins(0, 0, 0, 0);
    spinner_layout->setSpacing(0);

    // Spinner Loader
    spinner = new SpinnerProgress;
    spinner->setIndeterminate(true);
    spinner->setFixedSize(QSize(100, 140));
    spinner->setColor(SpinnerProgress::BackgroundLight, Qt::transparent);
    spinner->setColor(SpinnerProgress::BackgroundDark, Qt::transparent);
    
    // Text
    spinner_label = new Label("Segoe UI", 10, QFont::Normal, false, "Checking for update");
    
    // Adding text, loader to wrapper layout
    spinner_layout->addWidget(spinner, 0, Qt::AlignHCenter);
    spinner_layout->addWidget(spinner_label, 0, Qt::AlignHCenter);

    // Adding wrapper to win content area layout
    win_content_area_layout->addWidget(spinner_wrapper, 0, Qt::AlignCenter);

    // ---------------- Update Details Widget ----------------
    update_details_widget = new UpdateInfo;
    update_details_widget->hide();

    win_content_area_layout->addWidget(update_details_widget, 0, Qt::AlignHCenter);

    // ---------------- Separator ----------------
    sep = new Seperator(nullptr, 1, 0, Qt::Horizontal);
    sep->hide();

    win_content_area_layout->addSpacing(8);
    win_content_area_layout->addWidget(sep, 0, Qt::AlignHCenter);

    // ---------------- Connection ----------------
    connect(update_details_widget, &UpdateInfo::isUpdateAvailable, this, [this](bool isAvailable) {
        spinner->stop();
        spinner_wrapper->hide();

        win_content_area_layout->setContentsMargins(14, 34, 14, 10);

        if (isAvailable) {
            update_details_widget->show();
            sep->show();
        }
    });

    // Initial Theme
    setDarkMode(isDarkMode);
}

UpdateInfo* AppUpdates::updateInfoWidget() const {
    return update_details_widget;
}

void AppUpdates::setDarkMode(bool enable) {
    isDarkMode = enable;

    // Seperator
    if (sep)
        sep->setColor(isDarkMode ? "#334155" : "#E5E7EB");

    // Update Details Widget
    update_details_widget->setDarkMode(isDarkMode);

    // Spinner
    if (spinner)
        spinner->setDarkMode(isDarkMode);

    // Window Title
    if (winTitle)
        winTitle->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");

    // Spinner Label
    if (spinner_label)
        spinner_label->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");

    // Window Theme
    SubWindow::setDarkMode(isDarkMode);
}

void AppUpdates::showEvent(QShowEvent *event) {
    SubWindow::showEvent(event);
    if (spinner)
        spinner->start();
}