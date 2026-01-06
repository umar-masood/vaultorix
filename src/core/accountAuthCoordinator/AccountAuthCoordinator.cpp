#include "AccountAuthCoordinator.h"

/* ---------- Account Auth Coordinator ----------- */
AccountAuthCoordinator::AccountAuthCoordinator(QObject *parent) {
 
}

bool AccountAuthCoordinator::isAccountRegistered() const {
    if (!settings.contains(key)) 
        return false;

    return settings.value(key).toBool();
}

void AccountAuthCoordinator::setAccountRegistered(bool isRegistered) {
    if (!settings.contains(key)) 
        settings.setValue(key, isRegistered);
}  