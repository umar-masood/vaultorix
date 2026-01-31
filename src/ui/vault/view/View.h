#pragma once

#include "../emptyStateWidget/EmptyStateWidget.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>

class View : public QWidget {
    Q_OBJECT

    public:
    explicit View(QWidget *parent = nullptr);
    void setDarkMode(bool enable);

    protected:
    void paintEvent(QPaintEvent *event) override;

    private:
    // Flags
    bool isDarkMode = false;

    // Empty State Widget
    EmptyStateWidget *empty_state = nullptr;

    // Main Layout
    QVBoxLayout *layout = nullptr;
};