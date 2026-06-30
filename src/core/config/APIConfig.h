#pragma once

#include <QString>

namespace APIRoutes {
    using T = const char*;
    
    // Base
    inline constexpr T BASE                     = "https://www.umarcreations.site/";
    
    // Auth 
    inline constexpr T SIGNIN                   = "auth/login";
    inline constexpr T SIGNUP                   = "auth/register";
    inline constexpr T OTP_SEND                 = "auth/otp/send";
    inline constexpr T OTP_VERIFY               = "auth/otp/verify";
    inline constexpr T LOGOUT                   = "auth/logout";

    // Token
    inline constexpr T REFRESH                  = "auth/refresh";

    // User
    inline constexpr T CHK_USERNAME             = "user/check-username/";
    inline constexpr T CHK_EMAIL                = "user/check-email/";

    // App
    inline constexpr T REPORT_BUG               = "app/report-bug";
    inline constexpr T CHK_UPDATE               = "app/check-update/"; 

    // Account Settings
    inline constexpr T UPDATE_PROFILE_PICTURE   = "user/account/update-profile-picture"; // POST
    inline constexpr T UPDATE_2FA_STATUS        = "user/account/update-2fa/"; // PUT
    inline constexpr T UPDATE_USERNAME          = "user/account/update-username/"; // PUT
    inline constexpr T UPDATE_PASSWORD          = "user/account/update-password"; // POST
    inline constexpr T DELETE_ACCOUNT           = "user/account/delete"; // DELETE
    inline constexpr T DELETE_PROFILE_PICTURE   = "user/account/profile-picture/delete";

    // Email, Password, Username BlackLists
    inline constexpr T EMAIL_BLACKLIST = "https://raw.githubusercontent.com/umar-masood/Weak-Credentials/refs/heads/main/badDomains.config";
    inline constexpr T PWD_BLACKLIST = "https://raw.githubusercontent.com/umar-masood/Weak-Credentials/refs/heads/main/badPwds.config";
    inline constexpr T USERNAME_BLACKLIST = "https://raw.githubusercontent.com/umar-masood/Weak-Credentials/refs/heads/main/badUsernames.config";
}

// Helper
inline QString route(const char *path) {
    return QString::fromUtf8(APIRoutes::BASE) + QString::fromUtf8(path);
}