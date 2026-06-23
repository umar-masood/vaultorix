#include "./ui/auth/auth_coordinator/AuthCoordinator.h"
#include "ui/vault/vault_window/VaultWindow.h"
#include "core/config/Constants.h"
#include "./core/services/vault/tasks/TaskManager.h"

#include <QApplication>
#include <QThreadPool>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setWindowIcon(QIcon(":/app-icon.ico"));

    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(APP_VERSION);
    QCoreApplication::setOrganizationName(APP_ORG);
    
    QObject::connect(qApp, &QCoreApplication::aboutToQuit, []{
        TaskManager::shutdownRequested = true;
        auto pool = QThreadPool::globalInstance();
        pool->clear();
        pool->waitForDone(3000);
    });

    // AuthCoordinator *c = new AuthCoordinator(&app);
    // c->show();

    Ui::Vault::VaultWindow::instance()->show();
    return app.exec();
}
