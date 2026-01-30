#include "DialogsUtils.h"

namespace DialogUtils {
    const QString noInternetIcon = ":/icons/dialogs/noInternet.png";
    const QString wrongCredentialsIcon = ":/icons/dialogs/wrongCredentials.png";
    const QString unknownDeviceIcon = ":/icons/dialogs/unknownDevice.png";
    const QString somethingWentWrongIcon = ":/icons/dialogs/somethingWentWrong.png";
    const QString maxAttemptsIcon = ":/icons/dialogs/maxLimitReached.png";
    const QString accessDeniedIcon = ":/icons/dialogs/accessDenied.png"; 
    const QString timeoutIcon = ":/icons/dialogs/timeout.png";
    const QString futherAttemptBlockedIcon = ":/icons/dialogs/accessDenied.png";

    const QString noInternetText = "You're offline. We couldn't connect to the internet. Check your connection and try again.";
    const QString wrongCredentialsText = "The email or password you entered is incorrect. Please try again.";
    const QString unknownDeviceText = "This account is linked to another device and can't be used here for security reasons.";
    const QString somethingWentWrongText = "Something went wrong. Please try again.";
    const QString maxAttemptsText = "Maximum invalid attempts limit reached. Please try again after 24 hours.";
    const QString accessDeniedText = "We have blocked access to your account from this device for security reasons.";
    const QString timeoutText = "Request tmeout. Please check your internet connection and try again.";
    const QString futherAttemptBlockedText = "You cannot create multiple accounts on same device.";
};