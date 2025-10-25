#include "dialogsUtils.h"

namespace DialogUtils {
    const QSize noInternetDialogSize = QSize(342, 330);
    const QSize wrongCredentialsDialogSize = QSize(342, 330);
    const QSize noRegDeviceAccDialogSize = QSize(342, 300);

    const QSize illustrationSize = QSize(230, 160);

    const QString noInternet = ":/icons/Dialogs/noInternet.png";
    const QString wrongCredentials = ":/icons/Dialogs/wrongCredentials.png";
    const QString noRegDeviceAcc = ":/icons/Dialogs/noRegDeviceAcc.png";

    const QString noInternetText = "You're offline. We couldn't connect to the internet. Check your connection and try again.";
    const QString wrongCredentialsText = "The email or password you entered is incorrect. Please try again.";
    const QString noRegDeviceAccText = "This account is linked to another device and can't be used here for security reasons.";
};