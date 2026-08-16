#pragma once

#include <QWidget>
#include <QVector>
#include <QCoreApplication>
#include <QDebug>

class Label;
class Dialog;
class QVBoxLayout;
class QFontMetrics;

namespace Ui::Vault {
    class About : public QWidget {
        public:
        explicit About(QWidget *parent = nullptr);
        void show(); 

        private:
        void setDarkMode(bool isDarkMode);

        // Labels
        Label *app_name = nullptr;
        Label *app_version = nullptr;
        Label *tagline = nullptr;
        Label *developer = nullptr;
        Label *copyright = nullptr;
        QVector<Label *> labels;

        Dialog *dialog = nullptr;
    };
};