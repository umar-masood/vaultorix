#pragma once

#include "../windows/subWindow/SubWindow.h"
#include "../components/TextField.h"
#include "../components/Button.h"
#include "../components/Dialog.h"
#include "../components/Seperator.h"
#include "../components/ToolTip.h"

#include <QVBoxLayout>
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
   // App Logo
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
   // Helper Method for font
   QFont font() const;

   // Icon
   QPixmap icon;

   // Text
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

   private:
   void init();
   void onthemeModeChanged(bool enable);

   // Theme Mode Flag
   bool isDarkMode = false;

   // Icon Paths
   const QString darkModeIcon = ":/icons/AccountWindow/darkMode.svg";
   const QString lightModeIcon = ":/icons/AccountWindow/lightMode.svg";

   // SubWindow 
   SubWindow *w = nullptr;

   // Seperator for titlbar controls
   Seperator *seperator = nullptr;

   // Theme Mode Button
   Button *themeMode = nullptr;
   // ToolTip
   ToolTip *themeButtonTip = nullptr;

   // Vector to store all subwidgets (like dialogs, popups etc., especially for theme)
   QVector<QWidget *> subWidgets = {};

   // Main Layout
   QHBoxLayout *mainLayout = nullptr;

   // Right Widget
   QWidget *right = nullptr;
   // Layout
   QVBoxLayout *entireLayoutRight = nullptr;

   // Left Widget
   QWidget *left = nullptr;
   // Layout
   QVBoxLayout *entireLayoutLeft = nullptr;
   
   // Right Side Widget (It will change depending upon situation)
   QWidget *rightWidget = nullptr;

   // Left Panel
   // Illustration
   QLabel *illustration = nullptr;

   // Tagline
   QLabel *tagline = nullptr;

   // App Branding
   Branding *brand = nullptr;

   // Features bullet points
   BulletPoint *point1 = nullptr;
   BulletPoint *point2 = nullptr;
   BulletPoint *point3 = nullptr;
   BulletPoint *point4 = nullptr;
   BulletPoint *point5 = nullptr;

   // Vector to store all bullet points
   QVector<BulletPoint *> points;
};
