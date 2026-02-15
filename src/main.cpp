#include "./core/account/accountAuthCoordinator/AccountAuthCoordinator.h"
#include "./ui/vault/vaultWindow/VaultWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("Vaultorix");
    QCoreApplication::setApplicationVersion("1.0.0");
    QCoreApplication::setOrganizationName("Umar Creations");
    
    AccountAuthCoordinator *c = new AccountAuthCoordinator(&app);
    c->show();

    VaultWindow::instance()->show();

    return app.exec();
}