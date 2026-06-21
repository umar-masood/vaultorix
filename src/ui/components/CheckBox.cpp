#include "CheckBox.h"
#include "../../../resources/IconManager.h"

#include <QPixmap>
#include <QFont>
#include <QFontMetrics>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QRect>
#include <QString>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QEvent>
#include <QEasingCurve>
#include <QVariantAnimation>

CheckBox::CheckBox(const QString &text, QWidget *parent) : QWidget(parent), text(text)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_Hover);

    pixmap = QPixmap(IconManager::icon(Icons::CheckBox_Check))
                 .scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    setFixedSize(20 + 12 + QFontMetrics(font()).horizontalAdvance(text), 22);

    checkProgress = _isChecked ? 1.0 : 0.0;

    checkAnimation = new QVariantAnimation(this);
    checkAnimation->setDuration(220);
    checkAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(checkAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value)
    {
        checkProgress = value.toReal();
        update();
    });
}

void CheckBox::setDarkMode(bool value)
{
    isDarkMode = value;
    update();
}

bool CheckBox::isChecked() const {
    return _isChecked;
}

void CheckBox::setChecked(bool value) {
    if (_isChecked == value)
        return;

    _isChecked = value;

    startCheckAnimation(_isChecked);
    emit toggled(_isChecked);
}

void CheckBox::startCheckAnimation(bool checked) {
    if (!checkAnimation)
        return;

    checkAnimation->stop();

    checkAnimation->setStartValue(checkProgress);
    checkAnimation->setEndValue(checked ? 1.0 : 0.0);

    checkAnimation->start();
}

QFont CheckBox::font() const {
    QFont fnt;
    fnt.setPointSize(10);
    fnt.setFamily("Segoe UI");
    fnt.setWeight(QFont::Normal);
    return fnt;
}

void CheckBox::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    QRectF rec(1, 1, 20, 20);

    qreal progress = qBound(0.0, checkProgress, 1.0);

    QColor uncheckedBorder = isDarkMode ? QColor("#616161") : QColor("#CCCCCC");
    QColor checkedBorder = QColor("#0191DF");

    QColor borderColor;
    if (progress > 0.0)
        borderColor = checkedBorder;
    else
        borderColor = uncheckedBorder;

    QPen pen;
    pen.setWidthF(progress > 0.0 ? 1.0 : 0.5);
    pen.setColor(borderColor);
    pen.setStyle(Qt::SolidLine);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    QColor uncheckedBg;
    if (isHover)
        uncheckedBg = isDarkMode ? QColor("#323232") : QColor("#F2F2F2");
    else
        uncheckedBg = isDarkMode ? QColor("#2D2D2D") : QColor("#FBFBFB");

    QColor checkedBg = isHover ? QColor("#1BB3E6") : QColor("#0191DF");

    painter.setBrush(uncheckedBg);
    painter.drawRoundedRect(rec, 6, 6);

    /*
     * Animated blue fill.
     * Pehle box normal draw hota hai,
     * phir blue fill center se grow hoti hai.
     */
    if (progress > 0.0)
    {
        qreal fillProgress = qMin(progress / 0.55, 1.0);

        qreal scale = 0.65 + (0.35 * fillProgress);
        QPointF center = rec.center();

        QRectF fillRect(
            center.x() - (rec.width() * scale) / 2.0,
            center.y() - (rec.height() * scale) / 2.0,
            rec.width() * scale,
            rec.height() * scale
        );

        QColor fillColor = checkedBg;
        fillColor.setAlphaF(fillProgress);

        painter.setPen(Qt::NoPen);
        painter.setBrush(fillColor);
        painter.drawRoundedRect(fillRect, 6, 6);
    }

    /*
     * Animated tick.
     * 35% animation ke baad tick draw hona start hota hai.
     */
    if (progress > 0.35) {
        qreal tickProgress = (progress - 0.35) / 0.65;
        tickProgress = qBound(0.0, tickProgress, 1.0);
        drawAnimatedCheck(painter, rec, tickProgress);
    }

    painter.setFont(font());
    painter.setPen(QPen(QColor("#8D8D8D")));

    QRect textRec(rec.right() + 12, 2, width() - rec.width() - 12, height());
    painter.drawText(textRec, text);
}

void CheckBox::drawAnimatedCheck(QPainter &painter, const QRectF &rec, qreal progress) {
    QPointF p1(
        rec.left() + rec.width() * 0.28,
        rec.top() + rec.height() * 0.53
    );

    QPointF p2(
        rec.left() + rec.width() * 0.43,
        rec.top() + rec.height() * 0.68
    );

    QPointF p3(
        rec.left() + rec.width() * 0.74,
        rec.top() + rec.height() * 0.34
    );

    QPainterPath path;

    if (progress <= 0.45) {
        qreal t = progress / 0.45;

        QPointF current(
            p1.x() + (p2.x() - p1.x()) * t,
            p1.y() + (p2.y() - p1.y()) * t
        );

        path.moveTo(p1);
        path.lineTo(current);
    } else {
        qreal t = (progress - 0.45) / 0.55;

        QPointF current(
            p2.x() + (p3.x() - p2.x()) * t,
            p2.y() + (p3.y() - p2.y()) * t
        );

        path.moveTo(p1);
        path.lineTo(p2);
        path.lineTo(current);
    }

    QPen checkPen(QColor("#FFFFFF"));
    checkPen.setWidthF(1.5);
    checkPen.setCapStyle(Qt::RoundCap);
    checkPen.setJoinStyle(Qt::RoundJoin);

    painter.setPen(checkPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);
}

void CheckBox::enterEvent(QEnterEvent *event) {
    isHover = true;
    QWidget::enterEvent(event);
    update();
}

void CheckBox::leaveEvent(QEvent *event) {
    isHover = false;
    QWidget::leaveEvent(event);
    update();
}

void CheckBox::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton)
        setChecked(!_isChecked);

    QWidget::mousePressEvent(event);
}