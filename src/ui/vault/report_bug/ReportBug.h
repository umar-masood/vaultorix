#pragma once

#include "../../windows/subWindow/SubWindow.h"

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

        signals:
        void reportFinished();

        private slots:
        void onTextChanged();
    };
};