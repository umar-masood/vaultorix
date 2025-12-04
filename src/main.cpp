#include "ui/accountWindow/AccountWindow.h"
#include "./ui/accountCreate/AccountCreate.h"
#include "./core/emailValidator/emailValidator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    AccountWindow *aw = new AccountWindow;
    AccountCreate *ac = new AccountCreate;
    aw->setRightWidget(ac);

    GetEmail *ge = new GetEmail;
    ge->setAccountCreateObject(ac);

    aw->show();
    return app.exec();
}