#include "ReportBug.h"
#include "../../../core/theme/ThemeManager.h"

#include "../../components/Label.h"
#include "../../components/TextEdit.h"
#include "../../components/TextField.h"
#include "../../components/Button.h"
#include "../../../../resources/IconManager.h"
#include "../../dialogs/error_dialog/ErrorDialog.h"

#include <QVBoxLayout>
#include <QFileDialog>
#include <QStandardPaths>

using Ui::Vault::ReportBug;
ReportBug::ReportBug(QWidget *parent) : SubWindow(QSize(390, 454), parent){
    setModal(true);
    setFocusPolicy(Qt::StrongFocus);

    // ---------------- Titlebar Layout ----------------
    QHBoxLayout *_titlebarLayout = this->titlebarLayout();

    winTitle = new Label("Segoe UI", 10, QFont::Normal, false, "Report a bug", Qt::AlignLeft);
    winTitle->setFixedWidth(250);

    _titlebarLayout->addSpacing(6);
    _titlebarLayout->addWidget(winTitle, 0, Qt::AlignLeft | Qt::AlignVCenter);
    _titlebarLayout->addStretch();

    // ---------------- Content Area Layout ----------------
    win_content_area_layout = new QVBoxLayout(contentArea());
    win_content_area_layout->setSpacing(8);
    win_content_area_layout->setContentsMargins(14, 34, 14, 12);

    // ------------ Label -----------
    label = new Label("Segoe UI", 10, QFont::Normal, false, "Let us know any specific issue you experienced", Qt::AlignCenter);
    label->setFixedWidth(360);

    // ------------ Subject -----------
    _subject_field = new TextField;
    _subject_field->setPlaceholderText("Subject");
    _subject_field->setContextMenu(true);
    _subject_field->setClearButton(true);
    _subject_field->setFixedSize(QSize(360, 36));

    // ------------- Bug Description ------------------
    _descp_field = new TextEdit;
    _descp_field->setFixedSize(QSize(360, 250));
    _descp_field->setPlaceholderText("Describe the bug experienced as detailed as you can");

    // -------------- Upload Screenshot --------------
    // Label
    screenshot_label = new Label("Segoe UI", 10, QFont::Normal, false, "Upload a screenshot (Optional):", Qt::AlignLeft);
    screenshot_label->setMinimumWidth(250);
    
    // Button
    screenshot_btn = new Button("Upload");
    screenshot_btn->setCursor(Qt::PointingHandCursor);
    screenshot_btn->setDisplayMode(Button::IconText);
    screenshot_btn->setFontProperties("Segoe UI", 10, QFont::Normal);
    screenshot_btn->setIconSize(QSize(18, 18));
    screenshot_btn->setIconPaths(IconManager::icon(Icons::UploadLight), IconManager::icon(Icons::UploadDark));
    screenshot_btn->setSecondary(true);

    // Layout
    auto *screenshot_layout = new QHBoxLayout;
    screenshot_layout->setContentsMargins(2, 0, 2, 0);
    screenshot_layout->setSpacing(0);
    screenshot_layout->addWidget(screenshot_label, 0, Qt::AlignLeft | Qt::AlignVCenter);
    screenshot_layout->addStretch();
    screenshot_layout->addWidget(screenshot_btn, 0, Qt::AlignRight | Qt::AlignVCenter);

    // ------------- Submit Button ------------------
    _submit_btn = new Button("Submit");
    _submit_btn->setCursor(Qt::PointingHandCursor);
    _submit_btn->setDisplayMode(Button::TextOnly);
    _submit_btn->setFixedSize(QSize(360, 36));
    _submit_btn->setGradientColors("#008EDE", "#15F2FF", "#008EDE");
    _submit_btn->setFontProperties("Segoe UI", 10, QFont::Normal);
    _submit_btn->setLoaderButton(true);
    _submit_btn->setEnabled(false);

    // Adding label, fields, screenshot layout and button to layout
    win_content_area_layout->addWidget(label);
    win_content_area_layout->addWidget(_subject_field);
    win_content_area_layout->addWidget(_descp_field);
    win_content_area_layout->addLayout(screenshot_layout);
    win_content_area_layout->addWidget(_submit_btn);

    // Signal Slots
    connect(_subject_field, &TextField::textChanged, this, &ReportBug::onTextChanged);
    connect(_descp_field, &TextEdit::textChanged, this, &ReportBug::onTextChanged);
    connect(_submit_btn, &Button::clicked, this, &ReportBug::reportFinished);
    connect(screenshot_btn, &Button::clicked, this, [this](){
        QString path = QFileDialog::getOpenFileName(
            this,
            tr("Select a Screenshot"),
            QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
            "Image Files (*.png *jpg)"
        );

        if (!path.isEmpty()) {
            _filePath = path;
            screenshot_btn->hide();

            QFontMetrics fm(screenshot_label->font());
            QString elidedText = fm.elidedText(_filePath, Qt::ElideRight, 280);
            screenshot_label->setText(tr("Screenshot: ") + elidedText);
        }
    });

    // Theme
    auto &tm = ThemeManager::instance();
    connect(&tm, &ThemeManager::themeChanged, this, &ReportBug::setDarkMode);
    setDarkMode(tm.isDarkMode());

    // Registering this window for ErrorDialogManager
    ErrorDialogManager::instance()->registerWindow("ReportBug", this);

    // Bug report code
    report_bug_core = new Core::ReportBugService(this);

    connect(report_bug_core, &Core::ReportBugService::somethingWentWrong, this, &ReportBug::onSomethingWentWrong);
    connect(this, &ReportBug::reportFinished, this, [this](){
        if (report_bug_core) {
            Core::ReportBugService::BugData data;
            data.description = _descp_field->document()->toPlainText();
            data.subject = _subject_field->text();
            data.screenshotPath = this->filePath();

            report_bug_core->sendBugReport(data);

            changeSubmitBtnState("", false);
        }
    });

    connect(report_bug_core, &Core::ReportBugService::reportSubmitted, this, [this](){
        changeSubmitBtnState("Submitted", false);
        close();
    });

    connect(report_bug_core, &Core::ReportBugService::noInternet, this, [this](){
        changeSubmitBtnState("Submit", true);
    });
}

void ReportBug::onTextChanged() {
    if (!_subject_field)
        return;

    if (!_descp_field)
        return;

    if (!_submit_btn)
        return;

    QString subText = _subject_field->text().trimmed();
    QString descpText = _descp_field->toPlainText().trimmed();

    bool enableButton = (!subText.isEmpty() && subText.length() > 10) &&
                        (!descpText.isEmpty() && descpText.length() > 10);

    _submit_btn->setEnabled(enableButton);
}

void ReportBug::setDarkMode(bool isDarkMode) {
    // Window
    SubWindow::setDarkMode(isDarkMode);

    // Window Title
    if (winTitle)
        winTitle->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");

    // Label, TextField, TextEdit
    for (auto *l : {label, screenshot_label})
        l->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");
    
    if (_subject_field)
        _subject_field->setDarkMode(isDarkMode);

    if (_descp_field)
        _descp_field->setDarkMode(isDarkMode);

    // Button
    for (auto *b : {_submit_btn, screenshot_btn})
        b->setDarkMode(isDarkMode);
}

void ReportBug::changeSubmitBtnState(const QString &buttonText, bool isEnabled) {
    _submit_btn->setText(buttonText);
    _submit_btn->setEnabled(isEnabled);
}

void ReportBug::onSomethingWentWrong() {
    changeSubmitBtnState("Submit", true);
    ErrorDialogManager::instance()->show("SomethingWentWrong", "ReportBug");
}

QString ReportBug::filePath() const { return _filePath; }
TextEdit *ReportBug::descp_field() const { return _descp_field; }
TextField *ReportBug::subject_field() const { return _subject_field; }
Button* ReportBug::submit_btn() const { return _submit_btn; }