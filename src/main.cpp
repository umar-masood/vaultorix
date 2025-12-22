#include "ui/accountWindow/AccountWindow.h"
#include "./core/accountSignInManager/accountSignInManager.h"
#include "./ui/accountSignIn/AccountSignIn.h"
#include "./ui/accountOTP/AccountOTP.h"
#include "./core/validators/OTPValidator/OTPValidator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    AccountWindow *aw = new AccountWindow;
    AccountSignIn *ac = new AccountSignIn;
    aw->setRightWidget(ac);

    AccountSignInManager *asmgr = new AccountSignInManager(aw);
    asmgr->setAccountSignInObject(ac);


    aw->show();
    return app.exec();
}
