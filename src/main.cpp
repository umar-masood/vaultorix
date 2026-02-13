// #include "./core/account/accountAuthCoordinator/AccountAuthCoordinator.h"
// #include "./ui/vault/vaultWindow/VaultWindow.h"
#include <QApplication>
#include "./ui/vault/accountSettingsWindow/AccountSettingsWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("Vaultorix");
    QCoreApplication::setApplicationVersion("1.0.0");
    QCoreApplication::setOrganizationName("Umar Creations");
    
    AccountSettingsWindow *ass = new AccountSettingsWindow();
    ass->setDarkMode(true);
    ass->show();
    
    // AccountAuthCoordinator *c = new AccountAuthCoordinator(&app);
    // c->show();

    // VaultWindow *vw = new VaultWindow;
    // vw->show();

    return app.exec();
}