#include "Error.h"

Error::Error(QSize widgetSize, const QString &text, const QString &illustrationLight, const QString &illustrationDark, QSize iconSize, QWidget *parent) : QWidget(parent), lightIcon(illustrationLight), darkIcon(illustrationDark), size(iconSize) {
    
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(widgetSize);

    layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    //layout->setContentsMargins(10, 16, 10, 10);

    // Action Button
    actionBtn = new Button;
    actionBtn->setDisplayMode(Button::TextOnly);
    actionBtn->setSize(QSize(316, 36));
    actionBtn->setText("Retry");

    // Icon
    illustration = new QLabel;
    illustration->setAttribute(Qt::WA_TranslucentBackground);
    illustration->setFixedSize(size);
    illustration->setAlignment(Qt::AlignCenter);
    illustration->setPixmap(QPixmap(lightIcon).scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Text (only if provided)
    if (!text.isEmpty()) {
        textWidget = new Text;
        textWidget->setFixedSize(QSize(290, 36));
        textWidget->setFont("Segoe UI", 10, false, false, Qt::AlignCenter);
        textWidget->setTextColor("#8D8D8D");
        textWidget->setWordWrap(true);
        textWidget->setText(text);
    }

    layout->addStretch();
    layout->addSpacing(4);
    layout->addWidget(illustration, 0, Qt::AlignCenter);
    layout->addSpacing(16);

    if (textWidget) {
        layout->addWidget(textWidget, 0, Qt::AlignCenter);
        layout->addSpacing(16);
    }

    layout->addWidget(actionBtn, 0, Qt::AlignCenter);
    layout->addStretch();

    // Theme Change Signal Slot
    connect(this, &Error::themeModeChanged, this, &Error::onThemeModeChanged);
}

void Error::onThemeModeChanged(bool enable)
{
    if (illustration)
        illustration->setPixmap(QPixmap(enable ? darkIcon : lightIcon)
                                    .scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void Error::setDarkMode(bool value)
{
    if (isDarkMode == value) return;
    isDarkMode = value;
    emit themeModeChanged(isDarkMode);
}

Button * Error::actionButton()
{
    return actionBtn;
}
