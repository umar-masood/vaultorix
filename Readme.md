#include "ui/accountWindow/AccountWindow.h"
#include "ui/accountCreate/AccountCreate.h"
#include "ui/dialogs/termsConditions/TermsConditions.h"
#include "ui/dialogs/error/Error.h"
#include "ui/components/Dialog.h"
#include "ui/dialogs/dialogsUtils.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    TermsConditions *tC = new TermsConditions;
   
    Error *nI = new Error(DialogUtils::noRegDeviceAccDialogSize, DialogUtils::noRegDeviceAccText, DialogUtils::noRegDeviceAcc, DialogUtils::noRegDeviceAcc, DialogUtils::illustrationSize);

    AccountWindow *aw = new AccountWindow;
    AccountCreate *ac = new AccountCreate;
    aw->setRightWidget(ac);

    Dialog *d = new Dialog(nI, aw->subWindow(), true);
    aw->setSubWidgets({d, tC, nI});

    QObject::connect(ac->termsCondsBtn(), &CheckWithBtn::onButtonClicked, [d](){
        d->show();
    });

    QObject::connect(nI->actionButton(), &Button::pressed, [aw,d](){
       d->close();
    });

    aw->show();

    return app.exec();
}