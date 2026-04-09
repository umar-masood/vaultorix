#pragma once

#include <QWidget>
#include <QVector>
#include <QVBoxLayout>
#include <QCoreApplication>
#include <QFontMetrics>
#include <QDebug>

class Label;
class Dialog;

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