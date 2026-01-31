#pragma once

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QRect>
#include <QPainterPath>
#include <QFont>

class User : public QWidget {
    Q_OBJECT
    
    public:
    explicit User(QWidget *parent = nullptr);
    void setDarkMode(bool enable);
    void setEmail(const QString &email);
    void setName(const QString &name);
    void setAvator(const QPixmap &pixmap, int size);

    protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

    signals:
    void clicked();

    private:
    // Flags
    bool isDarkMode = false;
    bool isHover = false;
    bool isPressed = false;

    // Profile Icon
    QPixmap avator;

    // Name
    QString name = "Umar Masood Khan";

    // User Email-address
    QString email = "umarmasood8546@gmail.com";

};