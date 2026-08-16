#include "TextEdit.h"

#include "../../../resources/IconManager.h"

#include "Menu.h"
#include "ScrollBar.h"

#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QPalette>
#include <QTextDocument>
#include <QDebug>
#include <QContextMenuEvent>

TextEdit::TextEdit(QWidget *parent) : QTextEdit(parent) {    
    setFrameStyle(QFrame::NoFrame);    
    setAcceptRichText(false);
    setLineWrapMode(QTextEdit::WidgetWidth);
    setPadding(12);

    loadDefaultColors();
    updateStyle();

    // Scrollbar
    scroll_bar = new ScrollBar(Qt::Vertical);
    setVerticalScrollBar(scroll_bar);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Icons
    CopyIcon      =  IconManager::icon(Icons::Copy);
    CutIcon       =  IconManager::icon(Icons::Cut);
    PasteIcon     =  IconManager::icon(Icons::Paste);
    DeleteIcon    =  IconManager::icon(Icons::MenuDelete);
    SelectAllIcon =  IconManager::icon(Icons::SelectAll);
    UndoIcon      =  IconManager::icon(Icons::Undo);
    RedoIcon      =  IconManager::icon(Icons::Redo);
    ShowIcon      =  IconManager::icon(Icons::Show);     
    HideIcon      =  IconManager::icon(Icons::Hide);   
    ClearIcon     =  IconManager::icon(Icons::Clear);   

}

void TextEdit::setPadding(int padding) {
    document()->setDocumentMargin(padding);
}

void TextEdit::setDarkMode(bool enable) {
    isDarkMode = enable;

    if (scroll_bar)
        scroll_bar->setDarkMode(isDarkMode);

    updateStyle();
    update();
}

void TextEdit::setBorderRadius(int radius) {
    borderRadius = radius;
    update();
}

void TextEdit::loadDefaultColors() {
    // Border
    colors["BorderFocused"] = QColor("#0191DF");
    colors["BorderLight"]   = QColor("#CCCCCC");
    colors["BorderDark"]    = QColor("#616161");

    // Background
    colors["FocusedLight"] = QColor("#FFFFFF");
    colors["FocusedDark"]  = QColor("#242424");
    colors["HoverLight"]   = QColor("#F2F2F2");
    colors["HoverDark"]    = QColor("#323232");
    colors["NormalLight"]  = QColor("#FBFBFB");
    colors["NormalDark"]   = QColor("#2D2D2D");

    // Text
    colors["TextLight"]   = QColor("#000000");
    colors["TextDark"]    = QColor("#FFFFFF");

    // Selection
    colors["SelectionBg"]  = QColor("#32CCFE");
    colors["SelectionTxt"] = QColor("#FFFFFF");

    // Placeholder
    colors["Placeholder"] = QColor("#ACABAB");
}

void TextEdit::paintEvent(QPaintEvent *event) {
    QPainter painter(viewport());
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

    // Adjust rectangle for padding
    QRect rec = viewport()->rect().adjusted(2, 2, -2, -2); // viewport is already offset by margins

    // Border color
    QColor borderColor = isFocused ? colors["BorderFocused"]
            : (isDarkMode ? colors["BorderDark"] : colors["BorderLight"]);

    QPen pen(borderColor, isFocused ? 1.2 : 0.5);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    // Background color
    QColor bg;
    if (isFocused)
        bg = isDarkMode ? colors["FocusedDark"] : colors["FocusedLight"];
    else if (isHover)
        bg = isDarkMode ? colors["HoverDark"] : colors["HoverLight"];
    else
        bg = isDarkMode ? colors["NormalDark"] : colors["NormalLight"];

    painter.setBrush(bg);

    // Draw rounded background
    QPainterPath path;
    path.addRoundedRect(rec, borderRadius, borderRadius);
    painter.drawPath(path);

    // Draw text as usual
    QTextEdit::paintEvent(event);
}

void TextEdit::focusInEvent(QFocusEvent *event) {
    isFocused = true;
    QTextEdit::focusInEvent(event);
    update();
}

void TextEdit::focusOutEvent(QFocusEvent *event) {
    isFocused = false;
    QTextEdit::focusOutEvent(event);
    update();
}

void TextEdit::enterEvent(QEnterEvent *event) {
    isHover = true;
    QTextEdit::enterEvent(event);
    update();
}

void TextEdit::leaveEvent(QEvent *event) {
    isHover = false;
    QTextEdit::leaveEvent(event);
    update();
}

void TextEdit::contextMenuEvent(QContextMenuEvent *event) {
    if (contextMenuPolicy() == Qt::NoContextMenu || isReadOnly()) 
        return;

    menu = new Menu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->setMaxVisibleItems(12);
    menu->setItemSize(QSize(180, 36));
    menu->setDarkMode(isDarkMode);
    menu->setIconic(true);

    const bool hasText = !this->toPlainText().isEmpty();
    const bool hasSelection = this->textCursor().hasSelection();
    const bool hasClipboardText = QApplication::clipboard()->mimeData()->hasText();

    if (hasSelection) {
        menu->addAction({ "Copy",    false,  "Ctrl + C",  CopyIcon,    CopyIcon });
        menu->addAction({ "Cut",     false,  "Ctrl + X",  CutIcon,     CutIcon });
        menu->addAction({ "Delete",  false,  "Delete",    DeleteIcon,  DeleteIcon });
    }

    if (hasClipboardText)               menu->addAction({ "Paste",       false,  "Ctrl + V",  PasteIcon,     PasteIcon});
    if (hasText && !hasSelection)       menu->addAction({ "Select All",  false,  "Ctrl + A",  SelectAllIcon,  SelectAllIcon});
    if (document()->isUndoAvailable())  menu->addAction({ "Undo",        false,  "Ctrl + Z",  UndoIcon,       UndoIcon});
    if (document()->isRedoAvailable())  menu->addAction({ "Redo",        false,  "Ctrl + Y",  RedoIcon,       RedoIcon});

    connect(menu, &Menu::itemClicked, this, [=]() {
        QString action = menu->clickedItemText();
        if      (action == "Copy")       copy();
        else if (action == "Cut")        cut();
        else if (action == "Paste")      paste();
        else if (action == "Delete")     textCursor().deleteChar();
        else if (action == "Select All") selectAll();
        else if (action == "Undo")       undo();
        else if (action == "Redo")       redo();
        else qDebug() << "Unknown context menu action:" << action;
    });

    menu->show();
    menu->move(event->globalPos());
}

void TextEdit::updateStyle() {
    QString style = QString(R"(
        QTextEdit {
            background: transparent;
            border: none;
            color: %1;
            selection-background-color: %2;
            selection-color: %3;
        }
    )")
    .arg(isDarkMode ? colors["TextDark"].name() : colors["TextLight"].name())
    .arg(colors["SelectionBg"].name())
    .arg(colors["SelectionTxt"].name());

    setStyleSheet(style);

    // Placeholder color
    QPalette p = palette();
    p.setColor(QPalette::PlaceholderText, colors["Placeholder"]);
    setPalette(p);
}