#include "./core/accountAuthCoordinator/AccountAuthCoordinator.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("Vaultorix");
    QCoreApplication::setApplicationVersion("1.0.0");
    QCoreApplication::setOrganizationName("Umar Creations");
    
    AccountAuthCoordinator *c = new AccountAuthCoordinator;
    c->show();

    return app.exec();
}

