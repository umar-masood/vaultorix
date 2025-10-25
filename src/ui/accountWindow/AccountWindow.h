#pragma once

#include "../windows/subWindow/SubWindow.h"
#include "../components/TextField.h"
#include "../components/Button.h"
#include "../components/Dialog.h"
#include "../components/Seperator.h"
#include <QVBoxLayout>
#include <QFont>
#include <QFontMetrics>
#include <QVector>

/* ---------------- Branding Widget ---------------- */
class Branding : public QWidget
{
   Q_OBJECT
public:
   explicit Branding(QWidget *parent = nullptr);

protected:
   void paintEvent(QPaintEvent *event) override;

private:
   QPixmap logo;
};

/* ---------------- BulletPoint Widget ---------------- */
class BulletPoint : public QLabel
{
   Q_OBJECT
public:
   explicit BulletPoint(const QString &text, const QString &iconPath, QWidget *parent = nullptr);

protected:
   void paintEvent(QPaintEvent *event) override;

private:
   QFont font() const;
   QPixmap icon;
   QString text;
};

/* ---------------- AccountWindow ---------------- */
class AccountWindow : public QObject
{
   Q_OBJECT
public:
   explicit AccountWindow(QWidget *rightWidget = nullptr, QObject *parent = nullptr, const QVector<QWidget *> &dialogs = {});

   void setDarkMode(bool value);
   SubWindow *subWindow() const;
   void setRightWidget(QWidget *rightWidget);
   void init();
   void setSubWidgets(const QVector<QWidget *> subWidgets = {});
   void show();

   signals:
   void themeModeChanged(bool enable);

private slots:
   void onthemeModeChanged(bool enable);

private:
   bool isDarkMode = false;

   SubWindow *w = nullptr;

   QWidget *rightWidget = nullptr;

   QVBoxLayout *entireLayoutRight = nullptr;
   QVBoxLayout *entireLayoutLeft = nullptr;
   QWidget *right = nullptr;
   QWidget *left = nullptr;

   QLabel *illustration = nullptr;
   QLabel *mainPoint = nullptr;

   Branding *brand = nullptr;

   BulletPoint *point1 = nullptr;
   BulletPoint *point2 = nullptr;
   BulletPoint *point3 = nullptr;
   BulletPoint *point4 = nullptr;
   BulletPoint *point5 = nullptr;
   QVector<BulletPoint *> points;

   Button *themeMode = nullptr;
   const QString darkModeIcon = ":/icons/AccountWindow/darkMode.svg";
   const QString lightModeIcon = ":/icons/AccountWindow/lightMode.svg";

   QVector<QWidget *> subWidgets = {};
   Seperator *seperator = nullptr;
};
