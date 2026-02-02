#include "ViewModeToggle.h"

ViewModeToggle::ViewModeToggle(QWidget *parent) : QWidget(parent) {
    setFixedSize(QSize(60, 32));

    // List View Button
    list_view_btn = createButton();
    list_view_btn->setChecked(true);
    list_view_btn->setIconPaths( ":/icons/vaultWindow/layout-list.svg", ":/icons/vaultWindow/layout-list.svg");
    list_view_btn->setCheckedButtonIcon( ":/icons/vaultWindow/layout-list-checked.svg");

    // Grid View Button
    grid_view_btn = createButton();
    grid_view_btn->setIconPaths( ":/icons/vaultWindow/layout-grid.svg", ":/icons/vaultWindow/layout-grid.svg");
    grid_view_btn->setCheckedButtonIcon( ":/icons/vaultWindow/layout-grid-checked.svg");

    // Button Group
    group = new QButtonGroup(this);
    group->setExclusive(true);
    group->addButton(list_view_btn, 1);
    group->addButton(grid_view_btn, 2);
    connect(group, &QButtonGroup::idToggled, this, [this](int id, bool checked) {
        if (!checked) 
            return;
        
        if (id == 1) 
            emit list_view_selected();
        else 
            emit grid_view_selected();
    });

    // Layout
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(0);
    layout->addWidget(list_view_btn, 0, Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(grid_view_btn, 0, Qt::AlignRight | Qt::AlignVCenter);

    // Initial Theme
    setDarkMode(isDarkMode);
}

Button* ViewModeToggle::createButton() {
    auto *btn = new Button;
    btn->setCursor(Qt::PointingHandCursor);
    btn->setSecondary(true);
    btn->setIconSize(QSize(18, 18));
    btn->setDisplayMode(Button::IconOnly);
    btn->setFixedSize(QSize(28, 28));
    btn->setCheckable(true);

    return btn;
}

void ViewModeToggle::setDarkMode(bool enable) {
    isDarkMode = enable;
    list_view_btn->setDarkMode(enable);
    grid_view_btn->setDarkMode(enable);
    update();
}

void ViewModeToggle::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QPen pen;
    pen.setWidthF(0.5);
    pen.setColor(isDarkMode ? QColor("#4D4D4D") : QColor("#CCCCCC"));
    pen.setStyle(Qt::SolidLine);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    painter.setBrush(isDarkMode ? QColor("#2D2D2D") : QColor("#FBFBFB"));
    painter.drawRoundedRect(rect(), 6, 6);
}
