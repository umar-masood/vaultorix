#pragma once

#include "../../windows/subwindow/Subwindow.h"

#include <QLabel>
#include <QVector>
#include <QPixmap>
#include <QString>

class QWidget;
class QVBoxLayout;
class QHBoxLayout;
class QPaintEvent;

class TextField;
class Button;
class Dialog;
class Seperator;

class SmoothOpacity;
class ThemeManager;

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

/* ---------------- Bullet Point Widget ---------------- */
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

class ThemeManager;

namespace Ui {
    /* ---------------- Account Window ---------------- */
    class AuthWindow : public SubWindow {
       Q_OBJECT

       public:
       explicit AuthWindow(QWidget *rightWidget = nullptr, QWidget *parent = nullptr);
       void setRightWidget(QWidget *rightWidget);
       void setDarkMode(bool isDarkMode);

       private:
       // Icons
       QString DarkModeIcon, LightModeIcon;

       // Seperator for titlbar controls
       Seperator *seperator = nullptr;

       // Theme Mode Button
       Button *themeMode = nullptr;

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
};
