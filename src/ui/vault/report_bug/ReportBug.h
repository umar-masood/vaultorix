#pragma once

#include "../../windows/subwindow/Subwindow.h"
#include "../../../core/services/report_bug/ReportBugService.h"

class Label;
class Button;
class TextField;
class TextEdit;
class QVBoxLayout;

namespace Ui::Vault {
    class ReportBug : public SubWindow {
        Q_OBJECT

        public:
        explicit ReportBug(QWidget *parent = nullptr);

        QString filePath() const;
        TextEdit *descp_field() const;
        TextField *subject_field() const;
        Button *submit_btn() const;

        private:
        void setDarkMode(bool isDarkMode);
        void changeSubmitBtnState(const QString &buttonText, bool isEnabled);

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

        // Upload Screenshot Label
        Label *screenshot_label = nullptr;
        // Upload Screenshot Button
        Button *screenshot_btn = nullptr;
        // Screenshot file path
        QString _filePath;

        // Submit
        Button *_submit_btn = nullptr;

        // Bug Report Core
        Core::ReportBugService *report_bug_core = nullptr;

        signals:
        void reportFinished();

        private slots:
        void onTextChanged();
        void onSomethingWentWrong();
    };
};