#include "ui/accountWindow/AccountWindow.h"
#include "./ui/accountSignIn/AccountSignIn.h"
#include "./ui/accountOTP/AccountOTP.h"
#include "./ui/accountCreate/AccountCreate.h"
#include "./core/validators/OTPValidator/OTPValidator.h"
#include "./core/deviceInfo/deviceInfo.h"
#include "./core/accountSignInManager/accountSignInManager.h"
#include "./core/accountCreationManager/accountCreationManager.h"
#include "./ui/dialogs/errorDialog/ErrorDialog.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("Vaultorix");
    QCoreApplication::setApplicationVersion("1.0.0");
    QCoreApplication::setOrganizationName("Umar Creations");
    
    AccountWindow *aw = new AccountWindow;
    
    AccountSignIn *ac = new AccountSignIn();
    aw->setRightWidget(ac);

    // AccountCreationManager *m = new AccountCreationManager(aw);
    // m->setAccountCreateObject(ac);


    aw->show();
    return app.exec();
}

