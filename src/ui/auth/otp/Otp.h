#pragma once

#include <QVBoxLayout>
#include <QWidget>
#include <QVector>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QShowEvent>
#include <QVector>
#include <QLabel>
#include <QTimer>
#include "../../../core/services/auth/OTPService.h"

class Button;
class Label;
class AnimatedLabel;

namespace Ui::Utils { class TextWithBtn; };
namespace Ui  { class AuthWindow; };

/* ---------------- OTP Input Boxs Widget ----------------- */
class OTPInputWidget : public QWidget {
   Q_OBJECT

   public:
   explicit OTPInputWidget(QWidget *parent = nullptr);
   void setDarkMode(bool isDarkMode);
   void setEnabled(bool enabled);

   signals:
   void OTPcompleted(const QString &otp);
   void OTPincomplete();

   protected:
   void keyPressEvent(QKeyEvent *event) override;
   void showEvent(QShowEvent *event) override;

   private:
   bool _isDarkMode = false;
   
   // Current Index Tracker
   int currentIndex = 0;

   // Vectors to store OTP Boxes
   QVector<QLabel *> OTPBoxes;

   // StyleSheets for different states
   QString normalStyle, focusedStyle, disabledStyle;

   // Different State Colors
   QString bg_color_normal, bg_color_focused, text_color, border_color;

   // Helper Methods 
   void updateStyles();
   void updateHighlight();
   void checkOtpCompletion();
};

namespace Ui {
    /* ------------------- Account OTP ------------------------ */
    class OTP : public QWidget {
       Q_OBJECT
    
       public:       
       explicit OTP(QWidget *parent = nullptr, Ui::AuthWindow *authWindow = nullptr);
       void setDarkMode(bool value);
       void setEmail(const QString &email);
       void setAuthType(const QString &authType);
    
       private:
       // Email
       QString _email;

       // Illustration
       Label *illustration = nullptr;

       // Heading
       Label *heading = nullptr;
    
       // Text
       Label *text = nullptr;
    
       // Error Message Displaying Label
       AnimatedLabel *_message = nullptr;
    
       // OTP Widget
       OTPInputWidget *_otpInputWidget = nullptr;
    
       // Resend OTP Widget
       Ui::Utils::TextWithBtn *_resendOtpWidget = nullptr;
    
       // Verify Button
       Button *_verifyBtn = nullptr;
    
       // Cancel Button
       Button *_cancelBtn = nullptr;

       // Auth Window
       Ui::AuthWindow *_authWindow = nullptr;
       
       // OTP Core
       using OTPCore = Core::OTPService;
       OTPCore *otp_core = nullptr;

       QString _authType, currentOtp;
       bool isSendingOTP = false;

       // Timer
       int totalSecs = 90;
       QTimer *timer = nullptr;
       QString timeString;

       // Helpers
       void disableResendAndSetTimer();
       void disableControls(const QString &buttonText, const QString &errorMessage = "");
    
       private slots:
       void onMaxLimitReached();
       void onSomethingWentWrong(const Core::OTPService::OTPAction &action);
       void onNoInternet();
       void onOTPSent();
       void onOTPVerified(bool isVerified, const QJsonObject &obj = QJsonObject());
       void onEmailEntered(const QString &email);

       void onResendClicked();
       void onVerifyClicked();
       void onCancelClicked();
       void onOTPTimeout();
    
       signals:
       void emailEntered(const QString &email);
       void OTPVerified(bool isVerified);
    };
};