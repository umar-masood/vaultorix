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
#include <QLabel>

/* ---------------- Branding Widget ---------------- */
class Branding : public QWidget {
   Q_OBJECT

   public:
   explicit Branding(QWidget *parent = nullptr);

   protected:
   void paintEvent(QPaintEvent *event) override;

   private:
   QPixmap logo;
};

/* ---------------- BulletPoint Widget ---------------- */
class BulletPoint : public QLabel {
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
class AccountWindow : public QObject {
   Q_OBJECT

   public:
   explicit AccountWindow(QWidget *rightWidget = nullptr, QObject *parent = nullptr, const QVector<QWidget *> &dialogs = {});

   void setDarkMode(bool value);
   void setRightWidget(QWidget *rightWidget);
   void setSubWidgets(const QVector<QWidget *> subWidgets = {});
   void show();
   SubWindow *subWindow() const;

   signals:
   void themeModeChanged(bool enable);

   private slots:
   void onthemeModeChanged(bool enable);

   private:
   void init();

   // Theme
   bool isDarkMode = false;
   const QString darkModeIcon = ":/icons/AccountWindow/darkMode.svg";
   const QString lightModeIcon = ":/icons/AccountWindow/lightMode.svg";

   // SubWindow 
   SubWindow *w = nullptr;
   Seperator *seperator = nullptr;
   Button *themeMode = nullptr;
   QVector<QWidget *> subWidgets = {};

   QHBoxLayout *mainLayout = nullptr;

   QWidget *right = nullptr;
   QVBoxLayout *entireLayoutRight = nullptr;

   QWidget *left = nullptr;
   QVBoxLayout *entireLayoutLeft = nullptr;
   
   QWidget *rightWidget = nullptr; // Dynamic

   // Left Panel
   QLabel *illustration = nullptr;
   QLabel *mainPoint = nullptr;

   Branding *brand = nullptr;

   BulletPoint *point1 = nullptr;
   BulletPoint *point2 = nullptr;
   BulletPoint *point3 = nullptr;
   BulletPoint *point4 = nullptr;
   BulletPoint *point5 = nullptr;
   QVector<BulletPoint *> points;
};
