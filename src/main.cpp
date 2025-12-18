#include "ui/accountWindow/AccountWindow.h"
#include "./core/accountCreationManager/accountCreationManager.h"
#include "./ui/accountCreate/AccountCreate.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    AccountWindow *aw = new AccountWindow;
    AccountCreate *ac = new AccountCreate;
    aw->setRightWidget(ac);

    AccountCreationManager *acm = new AccountCreationManager;
    acm->setAccountCreateObject(ac);
    
    aw->show();
    return app.exec();
}
