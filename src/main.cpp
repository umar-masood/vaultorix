#include "./ui/auth/auth_coordinator/AuthCoordinator.h"
#include "core/config/Constants.h"
#include "./core/services/vault/tasks/TaskManager.h"

#include <QApplication>
#include <QThreadPool>
#include <QIcon>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    app.setWindowIcon(QIcon(":/app-icon.ico"));
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION);
    app.setOrganizationName(APP_ORG);
    
    QObject::connect(qApp, &QApplication::aboutToQuit, []{
        TaskManager::shutdownRequested = true;
        auto pool = QThreadPool::globalInstance();
        pool->clear();
        pool->waitForDone(3000);
    });

    AuthCoordinator *c = new AuthCoordinator(&app);
    c->show();

    return app.exec();
}
