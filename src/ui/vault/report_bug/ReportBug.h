#pragma once

#include "../../windows/subWindow/SubWindow.h"
#include "../../components/Label.h"
#include "../../components/TextEdit.h"
#include "../../components/TextField.h"
#include "../../components/Button.h"

#include <QVBoxLayout>

class ReportBug : public SubWindow {
    Q_OBJECT

    public:
    explicit ReportBug(QWidget *parent = nullptr);
    void setDarkMode(bool enable);

    TextEdit *descp_field() const;
    TextField *subject_field() const;

    private:
    bool isDarkMode = false;

    // Window Title
    Label *winTitle = nullptr;

    // Main Layout
    QVBoxLayout *win_content_area_layout = nullptr;

    // Label
    Label *label = nullptr;
    
    // Subject TextField
    TextField *_subject_field = nullptr;

    // Bug Descp 
    TextEdit *_descp_field = nullptr;

    // Submit
    Button *submit_btn = nullptr;

    signals:
    void reportFinished();

    private slots:
    void onTextChanged();
};