// #include "./ui/auth/auth_coordinator/AuthCoordinator.h"
#include "ui/vault/vault_window/VaultWindow.h"
#include "core/config/Constants.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);
    QCoreApplication::setOrganizationName(APP_ORG);
    
    // AuthCoordinator *c = new AuthCoordinator(&app);
    // c->show();

    Ui::Vault::VaultWindow::instance()->show();
    return app.exec();
}