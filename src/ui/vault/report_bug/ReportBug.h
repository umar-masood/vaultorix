#pragma once

#include "../../windows/subWindow/SubWindow.h"
#include <QVBoxLayout>

class Label;
class Button;
class TextField;
class TextEdit;

namespace Ui::Vault {
    class ReportBug : public SubWindow {
        Q_OBJECT

        public:
        explicit ReportBug(QWidget *parent = nullptr);

        TextEdit *descp_field() const;
        TextField *subject_field() const;

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

        // Submit
        Button *submit_btn = nullptr;

        signals:
        void reportFinished();

        private slots:
        void onTextChanged();
    };
};