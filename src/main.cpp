// #include "ui/accountWindow/AccountWindow.h"
// #include "ui/accountCreate/AccountCreate.h"
// #include "ui/dialogs/termsConditions/TermsConditions.h"
// #include "ui/components/Dialog.h"
// #include "core/pwdValidator/pwdValidator.h"

#include <iostream>
#include <QTimer>
#include <QString>
#include <QChar>
#include <QObject>
#include <QCoreApplication>

class OTPValidator {
    public:
    OTPValidator(){}

    private:

};

int main(int argc, char *agrv[]) {
    QCoreApplication app(argc, agrv);

    return 0;
}

int main(int argc, char *argv[])
{
    // QApplication app(argc, argv);
    
    // TermsConditions *tC = new TermsConditions;

    // AccountWindow *aw = new AccountWindow;
    // AccountCreate *ac = new AccountCreate;
    // aw->setRightWidget(ac);


    // Dialog *d = new Dialog(tC, aw->subWindow(), true);
    // aw->setSubWidgets({d, tC});

    // GetPassword *gp = new GetPassword;
    // gp->setAccountCreateObject(ac);
    
    
    // QObject::connect(ac->termsCondsBtn(), &CheckWithBtn::onButtonClicked, [d](){
    //     d->show();
    // });

    // aw->show();

    // return app.exec();
}

