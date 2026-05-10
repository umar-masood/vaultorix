#pragma once

#include <QWidget>

class QPaintEvent;
class QString;

class Seperator : public QWidget {
    public:
    explicit Seperator(int height = 1, int width = 1, Qt::Orientation o = Qt::Vertical, QWidget *parent = nullptr);
    void setColor(const QString &hex);

    protected:
    void paintEvent(QPaintEvent *event) override;

    private:
    QString color;
    Qt::Orientation orientation;
};