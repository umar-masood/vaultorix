#include "ui/accountWindow/AccountWindow.h"
#include "./ui/accountCreate/AccountCreate.h"
#include "./core/emailValidator/emailValidator.h"
#include "./core/usernameValidator/usernameValidator.h"
#include "./core/pwdValidator/pwdValidator.h"
#include "./core/nameValidator/nameValidator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    AccountWindow *aw = new AccountWindow;
    AccountCreate *ac = new AccountCreate;
    aw->setRightWidget(ac);

    GetEmail *ge = new GetEmail;
    ge->setAccountCreateObject(ac);

    GetUsername *gu = new GetUsername;
    gu->setAccountCreateObject(ac);

    GetPassword *gp = new GetPassword;
    gp->setAccountCreateObject(ac);

    GetName *gn = new GetName;
    gn->setAccountCreateObject(ac);

    aw->show();
    return app.exec();
}
#include "main.moc"