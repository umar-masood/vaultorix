#pragma once 
#include "SmoothShadow.h"
#include "SpinnerProgress.h"
#include "../../resources/IconManager.h"

#include <QPushButton>
#include <QPainterPath>
#include <QMouseEvent>
#include <QHideEvent>
#include <QEnterEvent>
#include <QGraphicsEffect>
#include <QGraphicsDropShadowEffect>
#include <QLinearGradient>
#include <QBrush>
#include <QPixmap>
#include <QSize>
#include <QRect>

class Button : public QPushButton {
  Q_OBJECT
  Q_PROPERTY(QColor startColor READ getStartColor WRITE setStartColor)
  Q_PROPERTY(QColor endColor READ getEndColor WRITE setEndColor)
  
  public:
  enum DisplayMode { IconOnly, TextOnly, IconText, TextUnderIcon, };

  explicit Button(const QString &text, QWidget *parent = nullptr);
  explicit Button(QWidget *parent = nullptr);

  void setDisplayMode(DisplayMode mode);
  void setIconPaths(const QString &iconLight = QString(), const QString &iconDark = QString());
  void setUnicodeIcon(const QString &unicode, int pointSize);
  void setFixedSize(const QSize &s);
  void setIconSize(const QSize &s);
  void setDarkMode(bool enable);
  void setSecondary(bool enable);
  void setShadow(bool enable);
  void setHyperLink(bool enable);
  void setHyperLinkColors(const QColor &normalState, const QColor &hoverState);
  void setFontProperties(const QString &family, int pointSize, QFont::Weight weight = QFont::Normal, bool italic = false);
  void setGradientColors(const QString &startColor, const QString &endColor, const QString &hoverColor);
  void setStartColor(const QColor &c);
  void setEndColor(const QColor &c);
  void setLoaderButton(bool enable);
  void setText(const QString &text);
  void setBorderTransparent(bool enable);
  void setNormalBackgroundTransparent(bool enable);
  void setCheckedButtonIcon(const QString &iconPath);
  void setPrimaryButtonIcon(const QString &iconPath);
  /** @warning Button Display Mode should be IconOnly or IconText*/
  void setRightSideIcon(const QString &iconLight, const QString &iconDark);
  void setFontXY(int x, int y);

  protected:
  void paintEvent(QPaintEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseReleaseEvent(QMouseEvent *event) override;
  void enterEvent(QEnterEvent *event) override;
  void leaveEvent(QEvent *event) override;
  void hideEvent(QHideEvent *event) override;

private:
  // Flags
  bool isHover = false;
  bool isPressed = false;
  bool isDarkMode = false;
  bool isSecondary = false;
  bool isShadowEnabled = false;
  bool isUnicodeIcon = false;
  bool isGradient = false;
  bool isHyperLink = false;
  bool isItalic = false;
  bool isLoaderBtn = false;
  bool isBorderTransparent = false;
  bool isNormalBackgroundTransparent = false;
  bool hasRightSideIcon = false;
  
  // Font Adjustment
  int _x = 0, _y = 0;

  bool isDisabledState() const;
  bool isHoverState() const;
  bool isNormalState() const;
  bool isPressedState() const;
  bool isIconOnly() const;

  void init();
  void drawBorder(QPainter &painter);
  void drawBackground(QPainter &painter, const QColor &bgColor);
  void drawContent(QPainter &painter, const QPixmap &pixmap);

  QColor getStartColor() const;
  QColor getEndColor() const;

  QColor getBackgroundColor() const;
  QColor getTextColor() const;
  QPixmap getPixmap() const;
  QFont getFont() const;

  // Icons
  QPixmap _lightIcon, _darkIcon, primaryButtonIcon, checkedButtonIcon, _rightSideLightIcon, _rightSideDarkIcon;

  // Icons Size
  QSize _iconSize = QSize(20, 20);
  QString unicodeIcon;
  int unicodeIconSize = 16;

  // Display Mode
  DisplayMode displayMode = IconText;
  QSize customSize;

  // Graphical Effects & Animations
  SmoothShadow *effect;
  QPropertyAnimation *animate;

  // Button Shadow Color
  QColor shadowColor;

  // Gradient Button Colors
  QColor gradientStart, gradientEnd, hoverGradientColor, color1, color2;
  
  // Standard Button States Colors
  // Primary Button
  QColor _normalPrimaryColor, _normalPrimaryHoverColor, _normalPrimaryPressedColor;

  // Secondary Button
  QColor _normalSecondaryLightModeColor, _normalSecondaryLightModeHoverColor, _normalSecondaryLightModePressedColor;


  // HyperLink Button Colors
  QColor hyperlinkNormal, hyperlinkHover;

  // Font 
  QString fontFamily = "Segoe UI";
  int fontSize = 10;
  QFont::Weight fontWeight = QFont::Normal;

  // Loader
  SpinnerProgress *spinner = nullptr;
};