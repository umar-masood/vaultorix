#include "ReportBug.h"

ReportBug::ReportBug(QWidget *parent) : SubWindow(QSize(390, 412), parent){
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
    win_content_area_layout->setContentsMargins(14, 34, 14, 14);

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

    // ------------- Submit Button ------------------
    submit_btn = new Button("Submit");
    submit_btn->setCursor(Qt::PointingHandCursor);
    submit_btn->setDisplayMode(Button::TextOnly);
    submit_btn->setFixedSize(QSize(360, 36));
    submit_btn->setGradientColors("#008EDE", "#15F2FF", "#008EDE");
    submit_btn->setFontProperties("Segoe UI", 10, QFont::Normal);
    submit_btn->setLoaderButton(true);
    submit_btn->setEnabled(false);

    // Adding label, fields and buttons to layout
    win_content_area_layout->addWidget(label);
    win_content_area_layout->addWidget(_subject_field);
    win_content_area_layout->addWidget(_descp_field);
    win_content_area_layout->addWidget(submit_btn);
    win_content_area_layout->addStretch(); 

    // Signal Slots
    connect(_subject_field, &TextField::textChanged, this, &ReportBug::onTextChanged);
    connect(_descp_field, &TextEdit::textChanged, this, &ReportBug::onTextChanged);
    connect(submit_btn, &Button::clicked, this, &ReportBug::reportFinished);

    // Initial Theme
    setDarkMode(!isDarkMode);
}

void ReportBug::onTextChanged() {
    if (!_subject_field)
        return;

    if (!_descp_field)
        return;

    if (!submit_btn)
        return;

    QString subText = _subject_field->text().trimmed();
    QString descpText = _descp_field->toPlainText().trimmed();

    bool enableButton = (!subText.isEmpty() && subText.length() > 10) &&
                        (!descpText.isEmpty() && descpText.length() > 10);

    submit_btn->setEnabled(enableButton);
}

void ReportBug::setDarkMode(bool enable) {
    isDarkMode = enable;

    // Window
    SubWindow::setDarkMode(isDarkMode);

    // Window Title
    if (winTitle)
        winTitle->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");

    // Label, TextField, TextEdit
    if (label)
        label->setTextColor(isDarkMode ? "#F1F5F9" : "#111827");
    
    if (_subject_field)
        _subject_field->setDarkMode(isDarkMode);

    if (_descp_field)
        _descp_field->setDarkMode(isDarkMode);

    // Button
    if (submit_btn)
        submit_btn->setDarkMode(isDarkMode);
}

TextEdit *ReportBug::descp_field() const { return _descp_field; }
TextField *ReportBug::subject_field() const { return _subject_field; }
