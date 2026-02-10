#include "Button.h"

Button::Button(const QString &text, QWidget *parent) : QPushButton(text, parent) { init(); }
Button::Button(QWidget *parent) : QPushButton(parent) { init(); }

void Button::init() {
  setFocusPolicy(Qt::NoFocus);

  // Shadow Effect
  effect = new SmoothShadow(this);
  effect->setOffset(0, 0);
  effect->setColor(QColor(0, 0, 0, 0));
  effect->setBlurRadius(0);

  // Smooth Shadow Effect animate
  animate = new QPropertyAnimation(effect, "blurRadius", this);
  animate->setDuration(300);
  animate->setEasingCurve(QEasingCurve::InOutQuad);

  // Loader Spinner
  spinner = new SpinnerProgress(this);
  spinner->setIndeterminate(true);
  spinner->setFixedSize(QSize(20, 20), true);
}

void Button::setShadow(bool value) {
  isShadowEnabled = value;
  (isShadowEnabled) ? setGraphicsEffect(effect) : setGraphicsEffect(nullptr);
}

void Button::setHyperLink(bool value) {
  isHyperLink = value;
  
  if (isSecondary) 
    isSecondary = false; 
}

void Button::setHyperLinkColors(const QColor &normalState, const QColor &hoverState) {
  hyperlinkNormal = normalState; 
  hyperlinkHover = hoverState;
}

void Button::setFontProperties(const QString &family, int pointSize, QFont::Weight weight, bool italic) {
  fontFamily = family; 
  fontSize = pointSize; 
  fontWeight = weight; 
  isItalic = italic;
}

void Button::setGradientColor(bool value, const QString &hex1, const QString &hex2) {
  isGradient = value;

  baseStart = QColor(hex1);
  baseEnd = QColor(hex2);

  color1 = baseStart;
  color2 = baseEnd;

  update();
}

void Button::setDisplayMode(DisplayMode mode) {
  displayMode = mode;
  
  if (displayMode == IconOnly) 
    setSecondary(true);
  
  update();
}

void Button::setIconPaths(const QString &lightIcon, const QString &darkIcon) {
  isUnicodeIcon = false;

  // Preload pixmaps
  if (!lightIcon.isEmpty()) {
    // _lightIcon.load(lightIcon);
    // _lightIcon = _lightIcon.scaled(_iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    _lightIcon = IconManager::renderSvg(lightIcon, _iconSize);
  }

  if (!darkIcon.isEmpty()) {
    // _darkIcon.load(darkIcon);
    // _darkIcon = _darkIcon.scaled(_iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    _darkIcon = IconManager::renderSvg(darkIcon, _iconSize);
  }
}

void Button::setUnicodeIcon(const QString &unicode, int pointSize) {
  unicodeIcon = unicode;
  unicodeIconSize = pointSize;
  isUnicodeIcon = true;

  update();
}

void Button::setIconSize(QSize s) {
  _iconSize = s;
  isUnicodeIcon = false;
}

void Button::setFixedSize(const QSize &s) { 
  if (s.isValid()) 
    customSize = s; 
  
  QPushButton::setFixedSize(s);
}

void Button::setDarkMode(bool value) { 
  isDarkMode = value; 

  if (isLoaderBtn && spinner) 
    spinner->setDarkMode(value); 
}

void Button::setCheckedButtonIcon(const QString &iconPath) { 
  if (!iconPath.isEmpty()) {
    checkedButtonIcon.load(iconPath);
    checkedButtonIcon = checkedButtonIcon.scaled(_iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }
}

void Button::setPrimaryButtonIcon(const QString &iconPath) { 
  if (!iconPath.isEmpty()) {
    primaryButtonIcon.load(iconPath);
    primaryButtonIcon = primaryButtonIcon.scaled(_iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }
}

void Button::setRightSideIcon(const QString &iconLight, const QString &iconDark) {
  if (displayMode == Button::IconOnly || displayMode == Button::IconText) {
    hasRightSideIcon = true;

    if (!iconLight.isEmpty()) {
      _rightSideLightIcon.load(iconLight);
      _rightSideLightIcon = _rightSideLightIcon.scaled(_iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    if (!iconDark.isEmpty()) {
      _rightSideDarkIcon.load(iconDark);
      _rightSideDarkIcon = _rightSideDarkIcon.scaled(_iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
  } else {
    qWarning() << "Error: setRightSideIcon() -> Button display mode is not correct.";
    return;
  }
}

void Button::setHoverGradientColor(const QString &hex) { hoverColor = QColor(hex); }
void Button::setStartColor(const QColor &c) { color1 = c.name(); update(); }
void Button::setEndColor(const QColor &c) { color2 = c.name(); update(); }
void Button::setSecondary(bool value) { isSecondary = value; }
void Button::setLoaderButton(bool value) { isLoaderBtn = value; }
void Button::setBorderTransparent(bool value) { isBorderTransparent = value; }
void Button::setNormalBackgroundTransparent(bool value) { isNormalBackgroundTransparent = value; }
void Button::setFontXY(int x, int y) { this->x = x; this->y = y; update(); }

void Button::setText(const QString &text) {
  if (isLoaderBtn && spinner) {
    if (text.isEmpty()) {
      spinner->start();
      spinner->move((width() - spinner->width()) / 2, (height() - spinner->height()) / 2);
    } else 
      spinner->stop();
  }

  QPushButton::setText(text);
}

bool Button::isDisabledState() const { return !isEnabled(); }
bool Button::isHoverState() const { return isHover && !isPressed; }
bool Button::isNormalState() const { return !isHover && !isPressed; }
bool Button::isPressedState() const { return isPressed; }
bool Button::isIconOnly() const { return displayMode == IconOnly; }

QColor Button::getStartColor() const { return QColor(color1); }
QColor Button::getEndColor() const { return QColor(color2); }

QColor Button::getBackgroundColor() const {
  // Colors
  QColor disabledSecondary   = isDarkMode ? "#555555" : "#E0E0E0";
  QColor disabledPrimary     = "#B0E0FF";
  QColor normalSecondary     = isDarkMode ? "#2D2D2D" : "#FBFBFB";
  QColor normalPrimary       = "#008EDE";

  if (isNormalBackgroundTransparent) {
    normalSecondary = Qt::transparent;
    normalPrimary = Qt::transparent;
  }

  QColor hoverSecondary      = isDarkMode ? "#323232" : "#F0F0F0";
  QColor hoverPrimary        = "#1BB3E6";
  QColor pressedSecondary    = isDarkMode ? "#242424" : "#FFFFFF";
  QColor pressedPrimary      = "#109AC7";

  if (isDisabledState()) 
    return isIconOnly() ? Qt::transparent : (isSecondary ? disabledSecondary : disabledPrimary);

  if (isCheckable() && isChecked()) 
    return "#109AC7";

  if (isNormalState()) 
    return isIconOnly() ? Qt::transparent : (isSecondary ? normalSecondary : normalPrimary);
  
  if (isHoverState()) 
    return (isSecondary || isIconOnly()) ? hoverSecondary : hoverPrimary;
  
  if (isPressedState() && !isCheckable()) 
    return (isSecondary || isIconOnly()) ? pressedSecondary : pressedPrimary;

  return normalPrimary; 
}

QColor Button::getTextColor() const {
  if (isSecondary)
    return isDarkMode ? QColor("#F0F0F0") : QColor("#000000");
  else if (isHyperLink)
    return isDisabledState() ? QColor("#555555") : (isHoverState() ? hyperlinkHover : hyperlinkNormal);
  else
    return QColor("#FFFFFF");
}

QPixmap Button::getPixmap() const {
  if (isUnicodeIcon)
    return QPixmap();  

  if (isCheckable() && isChecked())
    return checkedButtonIcon;
  else if (!isSecondary)
    return primaryButtonIcon;
  else
    return isDarkMode ? _darkIcon : _lightIcon;
}

QFont Button::getFont() const {
  QFont font;
  
  if (isUnicodeIcon) {
    font.setFamily("Segoe Fluent Icons");
    font.setPixelSize(unicodeIconSize);
    return font;
  }

  font.setFamily(fontFamily);
  font.setPointSize(fontSize);
  font.setWeight(fontWeight);
  font.setItalic(isItalic);

  return font;
}

void Button::drawBorder(QPainter &painter) {
  bool showBorder = isSecondary && !isIconOnly();

  if (!showBorder || isGradient || isHyperLink || isBorderTransparent)
    painter.setPen(Qt::NoPen);
  else {
    QPen pen(isDarkMode ? "#4D4D4D" : "#CCCCCC");
    pen.setWidthF(0.2);
    pen.setStyle(Qt::SolidLine);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);
  }
}

void Button::drawBackground(QPainter &painter, const QColor &bgColor) {
  if (isGradient) {
    QLinearGradient gradient(rect().topLeft(), rect().topRight());
    gradient.setColorAt(0, color1);
    
    if (!isDisabledState())
      gradient.setColorAt(1, color2);
    
    painter.setBrush(gradient);

  } else if (isHyperLink) 
    painter.setBrush(Qt::NoBrush);
  else 
    painter.setBrush(bgColor);

  if (!isHyperLink) {
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(1,1,-1,-1), 6, 6);
    painter.drawPath(path);
  }
}

void Button::drawContent(QPainter &painter, const QPixmap &pixmap) {
  QFontMetrics fm(painter.font());
  QSize tSize = fm.size(Qt::TextSingleLine, this->text());
  int textW = tSize.width();
  int textH = tSize.height();
  
  switch (displayMode) {
    case IconText: {
      const int spacing = 10;    
      const int iconX = spacing;

      int iconH = isUnicodeIcon ? unicodeIconSize : pixmap.height();
      int iconW = isUnicodeIcon ? unicodeIconSize : pixmap.width();
      int iconY = (height() - iconH) / 2;

      int textX = x != 0 ? x : iconX + iconW + spacing;
      int textY = y != 0 ? y : 0;
      
      int buttonWidth = iconX + (hasRightSideIcon ? (2 * iconW + 2 * spacing) : (iconW + spacing)) + textW + 2 * spacing;

      if (!customSize.isValid()) 
        QPushButton::setFixedSize(buttonWidth, 36);

      QRect textRect(textX, textY, textW , height());
      painter.drawText(textRect, Qt::AlignVCenter, text());

      if (isUnicodeIcon)
        painter.drawText(QRect(iconX, 0, unicodeIconSize + 4, height()), Qt::AlignCenter, unicodeIcon);
      else
        painter.drawPixmap(iconX, iconY, pixmap);

      if (hasRightSideIcon) 
        painter.drawPixmap((width() - 10 - iconW), iconY, (isDarkMode ? _rightSideDarkIcon : _rightSideLightIcon)); 

      break;
    }

    case IconOnly: {
      if (isUnicodeIcon)
        painter.drawText(rect(), Qt::AlignCenter, unicodeIcon);
      else {
        int x = (width() - pixmap.width()) / 2;
        int y = (height() - pixmap.height()) / 2;
        painter.drawPixmap(x, y, pixmap);
      }

      break;
    }

    case TextOnly: {
      int padding = 12;
      int buttonWidth = padding + textW + padding;

      if (customSize.isValid()) {
        textW = width() - 2 * padding;
        setFixedSize(customSize);
      } else 
        QPushButton::setFixedSize(buttonWidth, 36);

      QRect textRect = isHyperLink ? QRect(0, 0, width(), height()) : QRect(padding, 0, textW, height());
      painter.drawText(textRect, isHyperLink ? (Qt::AlignLeft | Qt::AlignVCenter) : (Qt::AlignCenter | Qt::TextWordWrap), text());
      break;
    }

    case TextUnderIcon: {
      const int hspacing = 12;
      const int vSpacing = 6;
      const int gap = 4;

      int textY = vSpacing + (isUnicodeIcon ? unicodeIconSize : pixmap.height()) + gap;

      int buttonWidth = hspacing + textW + hspacing;
      int buttonHeight = vSpacing + pixmap.height() + gap + textH + vSpacing;

      if (customSize.isValid()) 
        setFixedSize(customSize);
      else 
        QPushButton::setFixedSize(buttonWidth, buttonHeight);

      if (isUnicodeIcon)
        painter.drawText(QRect(0, vSpacing, width(), unicodeIconSize + 4), Qt::AlignCenter, unicodeIcon);
      else {
        int x = (width() - pixmap.width()) / 2;
        painter.drawPixmap(x, vSpacing, pixmap);
      }

      QRect textRect(0, textY, width(), textH);  
      painter.drawText(textRect, Qt::AlignHCenter, text());

      break;
    }

    default:  break;
  }
}

void Button::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

  // Border
  drawBorder(painter);

  // Background
  drawBackground(painter, getBackgroundColor());

  // Font
  painter.setFont(getFont());

  // Text color & opacity
  painter.setPen(getTextColor());
  painter.setOpacity(isPressed ? 0.6 : 1.0);

  // Draw icon & text
  drawContent(painter, getPixmap());
}

void Button::mousePressEvent(QMouseEvent *event) {
  isPressed = true;
  QPushButton::mousePressEvent(event);
  update();
}

void Button::mouseReleaseEvent(QMouseEvent *event) {
  isPressed = false;
  QPushButton::mouseReleaseEvent(event);
  update();
}

void Button::hideEvent(QHideEvent *event) {
  isPressed = false;
  isHover = false;
  update();
}

void Button::enterEvent(QEnterEvent *event) {
  isHover = true;

  if (isShadowEnabled && !isIconOnly()) {
    if (!isSecondary) 
      shadow_color = QColor::fromString("#008EDE");
    else 
      shadow_color = isDarkMode ? QColor::fromString("#333333") : QColor::fromString("#FFFFFF");

    effect->setColor(shadow_color);
    animate->setStartValue(effect->blurRadius());
    animate->setEndValue(25);
    animate->start();
  }

  if (isGradient && hoverColor.isValid()) {
    auto *a1 = new QPropertyAnimation(this, "startColor", this);
    a1->setDuration(300);
    a1->setStartValue(color1);
    a1->setEndValue(hoverColor);
    a1->start();

    auto *a2 = new QPropertyAnimation(this, "endColor", this);
    a2->setDuration(300);
    a2->setStartValue(color2);
    a2->setEndValue(hoverColor);
    a2->start();
  }

  QPushButton::enterEvent(event);
  update();
}

void Button::leaveEvent(QEvent *event) {
  isHover = false;

  if (isShadowEnabled) {
    animate->setStartValue(effect->blurRadius());
    animate->setEndValue(0);
    animate->start();
  }

  if (isGradient) {
    auto *a1 = new QPropertyAnimation(this, "startColor", this);
    a1->setDuration(300);
    a1->setStartValue(color1);
    a1->setEndValue(baseStart);
    a1->start();

    auto *a2 = new QPropertyAnimation(this, "endColor", this);
    a2->setDuration(300);
    a2->setStartValue(color2);
    a2->setEndValue(baseEnd);
    a2->start();
  }

  QPushButton::leaveEvent(event);
  update();
}