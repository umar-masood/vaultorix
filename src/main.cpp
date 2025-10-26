#include "ui/accountWindow/AccountWindow.h"
#include "ui/accountCreate/AccountCreate.h"
#include "ui/dialogs/termsConditions/TermsConditions.h"
#include "ui/components/Dialog.h"
#include "core/emailValidator/emailValidator.h"
#include <iostream>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    TermsConditions *tC = new TermsConditions;

    AccountWindow *aw = new AccountWindow;
    AccountCreate *ac = new AccountCreate;
    aw->setRightWidget(ac);

    EmailValidator *ev = new EmailValidator;

    Dialog *d = new Dialog(tC, aw->subWindow(), true);
    aw->setSubWidgets({d, tC});

    static QTimer timer;
    timer.setSingleShot(true);

    QObject::connect(ac->emailField(), &CustomTextField::textChanged, [=](const QString &text){
        timer.stop();

        QObject::disconnect(&timer, nullptr, nullptr, nullptr);
        QObject::connect(&timer, &QTimer::timeout, [ev, ac, text](){
            if (!ev || !ac) return;
            bool ok = ev->checkDisposableEmail(text.toStdString());
            
            if (!ok) ac->emailField()->setChecked();
            else ac->emailField()->setUnchecked();

            qDebug() << ok;
            qDebug()  << "   " << text;
        });

        timer.start(5000);
    });

    QObject::connect(ac->termsCondsBtn(), &CheckWithBtn::onButtonClicked, [d](){
        d->show();
    });

    aw->show();

    return app.exec();
}

#include "main.moc"