#include "IllustrationManager.h"

QString IllustrationManager::illustration(Illustrations illustration)
{
    switch (illustration) {

    case Illustrations::AccessDenied:
        return ":/illustrations/accessDenied.png";

    case Illustrations::FolderDark:
        return ":/illustrations/folder-dark.svg";

    case Illustrations::FolderLight:
        return ":/illustrations/folder-light.svg";

    case Illustrations::AppBranding:
        return ":/illustrations/appBranding.png";

    case Illustrations::MaxLimitReached:
        return ":/illustrations/maxLimitReached.png";

    case Illustrations::NoInternet:
        return ":/illustrations/noInternet.png";

    case Illustrations::OTP:
        return ":/illustrations/otp.png";

    case Illustrations::SignIn:
        return ":/illustrations/sign-in.png";

    case Illustrations::SomethingWentWrong:
        return ":/illustrations/somethingWentWrong.png";

    case Illustrations::Timeout:
        return ":/illustrations/timeout.png";

    case Illustrations::UnknownDevice:
        return ":/illustrations/unknownDevice.png";

    case Illustrations::DeleteAccount:
        return ":/illustrations/deleteAccount.png";

    case Illustrations::WrongCredentials:
        return ":/illustrations/wrongCredentials.png";
    }

    return QString(); // safe fallback
}
