#include "ui/accountWindow/AccountWindow.h"
#include "./ui/accountOTP/AccountOTP.h"
#include "./core/OTPValidator/OTPValidator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    AccountWindow *aw = new AccountWindow;
    AccountOTP *ac = new AccountOTP;
    aw->setRightWidget(ac);

    GetOTP *go = new GetOTP;
    QString e = "alimasood7281@gmail.com", u = "umarmasood1", n = "Umar Masood";
    go->setAccountOTPObjectWithDetails(ac, e, u, n);

  
    aw->show();
    return app.exec();
}