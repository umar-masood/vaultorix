// #include "./core/account/accountAuthCoordinator/AccountAuthCoordinator.h"
#include "./ui/vault/vaultWindow/VaultWindow.h"
#include <QApplication>
#include "./ui/vault/userMenu/UserMenu.h"
#include "./resources/IconManager.h"
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("Vaultorix");
    QCoreApplication::setApplicationVersion("1.0.0");
    QCoreApplication::setOrganizationName("Umar Creations");
    
    // AccountAuthCoordinator *c = new AccountAuthCoordinator(&app);
    // c->show();

    VaultWindow *vw = new VaultWindow;
    vw->show();

    UserMenu *um = new UserMenu;
    um->setAvator(QPixmap(
                (IconManager::icon(Icons::Avator))
                ).scaled(100, 100, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    um->setName("Umar Masood");
    um->setEmail("umarmasood8546@gmail.com");
    um->setDarkMode(true);
    um->show();

    return app.exec();
}