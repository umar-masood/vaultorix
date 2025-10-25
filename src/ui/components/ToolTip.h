#pragma once

#include <QObject>
#include "RoundedBox.h"
#include "SmoothOpacity.h"
#include <QPropertyAnimation>
#include <QTimer>
#include <QEvent>
#include <QScreen>
#include <QApplication>
#include <algorithm>

class ToolTip : public QObject {
  Q_OBJECT
public:
  static void setToolTip(QWidget *widget, const QString &text, bool isDarkMode);

private:
  explicit ToolTip(QWidget *target, const QString &text, bool isDarkMode);

  bool eventFilter(QObject *obj, QEvent *event) override;

  void position(QWidget *target);
  void showToolTip();
  void fadeInAnimation();
  void fadeOutAnimation();

  QTimer timer;
  QWidget *target = nullptr;
  RoundedBox *tooltipWidget = nullptr;
  bool isHovering = false;
  QPropertyAnimation *animation = nullptr;
};
