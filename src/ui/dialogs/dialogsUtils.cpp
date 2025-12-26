#include "dialogsUtils.h"

namespace DialogUtils {
    const QSize dialogSize = QSize(342, 320);
    const QSize illustrationSize = QSize(200, 200);

    const QString noInternetIcon = ":/icons/Dialogs/noInternet.png";
    const QString wrongCredentialsIcon = ":/icons/Dialogs/wrongCredentials.png";
    const QString unknownDeviceIcon = ":/icons/Dialogs/unknownDevice.png";
    const QString somethingWentWrongIcon = ":/icons/Dialogs/somethingWentWrong.png";
    const QString maxAttemptsIcon = ":/icons/Dialogs/maxLimitReached.png";
    const QString accessDeniedIcon = ":/icons/Dialogs/unknownDevice.png"; // We will change this icon later
    const QString timeoutIcon = ":/icons/Dialogs/noInternet.png";

    const QString noInternetText = "You're offline. We couldn't connect to the internet. Check your connection and try again.";
    const QString wrongCredentialsText = "The email or password you entered is incorrect. Please try again.";
    const QString unknownDeviceText = "This account is linked to another device and can't be used here for security reasons.";
    const QString somethingWentWrongText = "Something went wrong. Please try again.";
    const QString maxAttemptsText = "Maximum invalid sign-in attempts limit reached. Please try again after 24 hours.";
    const QString accessDeniedText = "We have blocked access to your account from this device for security reasons.";
    const QString timeoutText = "We could not connect to the server. Please check your internet connection and try again.";
};