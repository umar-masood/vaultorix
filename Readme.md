
    QObject::connect(ac->pwdField(), CustomTextField::textChanged, [=](const QString &text){
        timer.stop();

        QObject::disconnect(&timer, nullptr, nullptr, nullptr);
        QObject::connect(&timer, &QTimer::timeout, [ev, ac, text](){
            if (!ev || !ac) return;
            bool ok = ev->checkStrongPwd(text.toStdString());
            
            if (!ok) ac->pwdField()->setChecked();
            else ac->pwdField()->setUnchecked();

            qDebug() << ok;
            qDebug()  << "   " << text;
        });

        timer.start(5000);
    });




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



//int main(int argc, char *argv[])
//{
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
//}


// #include "ui/accountWindow/AccountWindow.h"
// #include "ui/accountCreate/AccountCreate.h"
// #include "ui/dialogs/termsConditions/TermsConditions.h"
// #include "ui/components/Dialog.h"
// #include "core/pwdValidator/pwdValidator.h"