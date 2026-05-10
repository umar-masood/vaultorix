#pragma once

#include <QStyledItemDelegate>
#include <QModelIndex>
#include <QSize>

class QPainter;
class QStyleOptionViewItem;

class Delegate : public QStyledItemDelegate {
    Q_OBJECT

    public:
    explicit Delegate(const QSize& itemSize, QObject *parent = nullptr);
    void setDarkMode(bool value);
    void setIconic(bool value);
    void setAsMenu(bool value);
    void setDelegateSize(QSize size);
    void setHoveredIndex(const QModelIndex &index);
    void setSelectionDotIndicator(bool enable);
    void setSelectionCheckIndicator(bool enable);
    void setActiveIndex(const QModelIndex &index);

    protected:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    private:
    QSize m_itemSize;
    QModelIndex hoveredIndex, activeIndex;

    bool isDarkMode = false;
    bool isIconic = false;
    bool isMenu = true;
    bool hasDotIndicator = false;
    bool hasCheckIndicator = false;
};