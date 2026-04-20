#pragma once

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QPixmap>
#include <QRect>
#include <QPainterPath>
#include <QFont>

namespace Ui::Vault { class UserMenu; };

namespace Ui::Vault {
    class User : public QWidget {
        Q_OBJECT

        public:
        explicit User(QWidget *parent = nullptr);
        void setName(const QString &name);
        void setAvator(const QPixmap &pixmap, int size);

        protected:
        void paintEvent(QPaintEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void enterEvent(QEnterEvent *event) override;
        void leaveEvent(QEvent *event) override;
        bool eventFilter(QObject *o, QEvent *event) override;

        signals:
        void clicked();

        private: 
        void adjustWidgetSize();
        QFont font();
        void setDarkMode(bool isDarkMode);

        // Flags
        bool isHover = false;
        bool isPressed = false;

        // Profile Icon
        QPixmap avator;

        // Name
        QString _name = "Umar Masood Khan";

        // Menu
        Ui::Vault::UserMenu *um = nullptr;
    };
};