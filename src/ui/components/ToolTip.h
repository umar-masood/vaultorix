#pragma once

#include <QObject>
#include <QString>
#include <QTimer>
#include <QPropertyAnimation>

class QWidget;
class QScreen;
class QEvent;
class RoundedBox;
class SmoothOpacity;

class ToolTip : public QObject {
  Q_OBJECT
  
  public:
  explicit ToolTip(QWidget *target = nullptr, QObject *parent = nullptr);
  ~ToolTip();

  void setText(const QString &text);
  void setDarkMode(bool enable);
  void setTargetWidget(QWidget *target);
  void hide();


  protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

  private:
  void position(QWidget *target);
  void showToolTip();
  void fadeInAnimation();
  void fadeOutAnimation();

  bool isDarkMode = false;
  bool isHovering = false;

  QTimer timer;
  QWidget *_target = nullptr;
  RoundedBox *tooltipWidget = nullptr;
  QPropertyAnimation *animation = nullptr;

  signals:
  void textEntered(const QString &text);

  private slots:
  void onTextEntered(const QString &text);
  void onTimeout();
};

