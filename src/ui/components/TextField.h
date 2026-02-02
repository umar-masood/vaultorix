#pragma once

#include "Button.h"
#include "Menu.h"
#include "SmoothShadow.h"

#include <QResizeEvent>
#include <QFocusEvent>
#include <QContextMenuEvent>
#include <QTimer>
#include <QDebug>
#include <algorithm>
#include <QLineEdit>

class TextField : public QLineEdit {
  Q_OBJECT

  public:
  explicit TextField(const QString &text, QWidget *parent = nullptr);
  explicit TextField(QWidget *parent = nullptr);

  void setShadow(bool value);
  virtual void setDarkMode(bool value);
  void setFixedSize(QSize s);
  void setTextFieldIcon(bool value);
  void setTextFieldIconSize(QSize s);
  void setIconPaths(const QString &lightIcon = "", const QString &darkIcon = "");
  void setClearButton(bool value);
  void setPasswordTextField(bool value);
  void setPadding(int left = 0, int top = 0, int right = 0, int bottom = 0);
  void setTextSelectedBackgroundColor(const QString &hex);
  void setPlaceHolderTextColor(const QString &hex);
  void setTextSelectionColor(const QString &hex);
  void setTextColor(const QString &hex);
  void setReadOnly(bool value);
  void setEnabled(bool value);
  void setFontProperties(const QString &family, int pointSize = 12, bool bold = false, bool italic = false);
  void setContextMenu(bool value);
  void setBorderTransparent(bool value);
  void setNormalBackgroundTransparent(bool value);

  protected:
  void paintEvent(QPaintEvent *event) override;
  void enterEvent(QEnterEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void focusInEvent(QFocusEvent *event) override;
  void focusOutEvent(QFocusEvent *event) override;
  void keyPressEvent(QKeyEvent *event) override;
  void contextMenuEvent(QContextMenuEvent *event) override;
  bool event(QEvent *event);
  void resizeEvent(QResizeEvent *event) override;

  private:
  void init();
  void updateStyle();
  void positionButton(Button *button);

  // States
  bool isHover = false;
  bool isFocused = false;
  bool isDarkMode = false;

  bool hasShadow = false;
  bool hasTextFieldIcon = false;
  bool hasClearButton = false;
  bool hasPasswordButton = false;
  bool hasContextMenu = true;

  bool isPasswordVisible = false;
  bool isReadOnly = false;
  bool isEnabled = true;
  bool isBold = false;
  bool isItalic = false;
  bool isBorderTransparent = false;
  bool isBackgroundTransparent = false;

  // Padding
  int _left = 0, _right = 0, _top = 0, _bottom = 0;

  // Text Colors
  QString _selected_text_color, _selected_text_background_color, _text_color, _placeholder_text_color;

  // Context Menu Icons
  const QHash<QString , QString> icons = {
    { "Copy",       ":/icons/components/copy.svg" },
    { "Cut",        ":/icons/components/cut.svg" },
    { "Paste",      ":/icons/components/paste.svg" },
    { "Delete",     ":/icons/components/delete.svg" },
    { "Select All", ":/icons/components/select-all.svg" },
    { "Undo",       ":/icons/components/undo.svg" },
    { "Redo",       ":/icons/components/redo.svg" }
  };

  // Graphics
  SmoothShadow *effect = nullptr;
  QPropertyAnimation *animate = nullptr;

  // Icons
  QString light_icon;
  QString dark_icon;
  const QString eyeIcon = ":/icons/components/eye.svg";
  const QString eyeClosedIcon = ":/icons/components/eye-closed.svg";
  const QString clearIcon = ":/icons/components/x.svg";
  QSize textFieldIconSize = QSize(20, 20);

  // Buttons
  Button *clear = nullptr;
  Button *password = nullptr;

  // Font 
  QString fontFamily = "Segoe UI";
  int fontSize = 10;

  // Size
  const int fixedHeight = 36;
  const int minWidth = 36;

  // Menu
  Menu *menu = nullptr;
};
