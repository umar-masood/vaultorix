#pragma once

#include <QWidget>
#include <QVector>
#include <QLabel>
#include <QString>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QFont>

class OTPWidget : public QWidget
{
   Q_OBJECT

public:
   explicit OTPWidget(QWidget *parent = nullptr);
   void setDarkMode(bool value);

signals:
   void OTPcompleted(const QString &otp);

protected:
   void keyPressEvent(QKeyEvent *event) override;

private:
   bool isDarkMode = false;
   int currentIndex = 0;
   QVector<QLabel *> OTPBoxes;
   const int boxes = 5;

   QString normalStyle;
   QString focusedStyle;

   void updateStyles();
   void updateHighlight();
};
