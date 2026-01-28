#pragma once

#include <QVBoxLayout>
#include <QWidget>
#include <QVector>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QShowEvent>

#include "../../components/Button.h"
#include "../../components/Label.h"

/* ---------------- OTP Input Boxs Widget ----------------- */
class OTPWidget : public QWidget {
   Q_OBJECT

   public:
   explicit OTPWidget(QWidget *parent = nullptr);
   void setDarkMode(bool value);
   void setEnabled(bool enabled);

   signals:
   void OTPcompleted(const QString &otp);

   protected:
   void keyPressEvent(QKeyEvent *event) override;
   void showEvent(QShowEvent *event) override;

   private:
   // Theme Mode Flag
   bool isDarkMode = false;

   // Current Index Tracker
   int currentIndex = 0;

   // No. of Boxes
   const int boxes = 5;

   // Vectors to store OTP Boxes
   QVector<QLabel *> OTPBoxes;

   // StyleSheets for different states
   QString normalStyle;
   QString focusedStyle;
   QString disabledStyle;

   // Different State Colors
   QString bg_color_normal;
   QString bg_color_focused;
   QString text_color;
   QString border_color;

   // Helper Methods 
   void updateStyles();
   void updateHighlight();
};

/* ----------------- Text with Button ------------------ */
class TextWithBtn : public QWidget {
   Q_OBJECT

   public:
   explicit TextWithBtn(const QString &promptText = "",
                        const QSize &promptTextSize = QSize(), 
                        const QString &hyperlinkText = "", 
                        const QSize &hyperlinkSize = QSize(), 
                        bool hasTimer = false,
                        QWidget *parent = nullptr);
                     
   Label* text() const;
   Button* button() const;
   Label* timer() const;

   private:
   // Text Label
   Label *_text = nullptr;

   // Timer Label
   Label *_timer = nullptr;
   
   // Resend Button
   Button *_button = nullptr;

   signals:
   void buttonClicked();
};


/* ------------------- Account OTP ------------------------ */
class AccountOTP : public QWidget {
   Q_OBJECT

   public:
   explicit AccountOTP(QWidget *parent = nullptr);
   Q_INVOKABLE void setDarkMode(bool value);
   void setEmail(const QString &email);
   OTPWidget *otpWidget() const;
   Button *verifyButton() const;
   TextWithBtn *resendOtpWidget() const;
   AnimatedLabel *message() const;

   private:
   // Theme Mode Flag
   bool isDarkMode = false;

   // Illustration
   Label *illustration = nullptr;

   // Heading
   Label *heading = nullptr;

   // Text
   Label *text = nullptr;

   // Error Message Displaying Label
   AnimatedLabel *_message = nullptr;

   // OTP Widget
   OTPWidget *_otpWidget = nullptr;

   // Resend OTP Widget
   TextWithBtn *_resendOtpWidget = nullptr;

   // Verify Button
   Button *_verifyBtn = nullptr;

   // Cancel Button
   Button *_cancelBtn = nullptr;

   // Main Layout
   QVBoxLayout *layout = nullptr;

   // Slot
   void onEmailEntered(const QString &email);

   signals:
   void emailEntered(const QString &email);
   void resendClicked();
   void verifyClicked();
   void cancelClicked();
};