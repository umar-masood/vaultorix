#pragma once
#include <QFileInfo>
#include <QProcessEnvironment>

#define APP_VERSION "1.0.0"
#define APP_NAME "Vaultorix"
#define APP_ORG "Umar Creations"

#define OTP_TIME 120
#define OTP_INITIAL_TIME_STR "02:00"
#define OTP_BOXS 5

#define PROFILE_PICTURE_MAX_SIZE 2097152
#define MAX_IMPORT_FILE_SIZE 2147483648
#define REQUEST_TIMEOUT 15000
#define OLDER_LIST_TIME 172800
#define REDOWNLOAD_INTERVAL 10000

#define EMAIL_BLACKLIST_FILE "WeakDomains.config"
#define PWD_BLACKLIST_FILE "WeakPasswords.config"
#define USERNAME_BLACKLIST_FILE "WeakUsernames.config"
#define UPDATE_DOWNLOAD_FILE "Vaultorix-Setup-v1.0.0.exe"

#define VAULT_IMPORT_CHUNK_SIZE 4 * 1024 * 1024
#define VAULT_ENCRYPT_CHUNK_SIZE 6 * 1024 * 1024

#ifdef Q_OS_WIN
    #define VAULT_SECRET_PATH \
        (QProcessEnvironment::systemEnvironment().value("ProgramData") + "/Vaultorix")

#elif defined(Q_OS_MAC)
    #define VAULT_SECRET_PATH \
        "/Library/Application Support/Vaultorix"

#elif defined(Q_OS_LINUX)
    #define VAULT_SECRET_PATH \
        "/var/lib/Vaultorix"

#endif

#define ACCESS_TOKEN_EXPIRY 600
#define REFRESH_TOKEN_EXPIRY 86400
#define USERNAME_CHANGE_INTERVAL 2592000

#define DEVICE_ID_SETTINGS_KEY "Device/Device Id"
#define SHOW_SIGN_IN_SETTINGS_KEY "Application/Sign in" // When an account is registered first time then on each app run, show sign in screen.
#define DEVICE_THEME_KEY "Device/Default Theme"
#define LOCKOUT_SETTINGS_KEY "Application/Quit"

#define DEBUG_HERE(msg)\
    qDebug().noquote() << "[DEBUG]" << " " << QFileInfo(__FILE__).fileName() << ":"  << __LINE__ << " " << QString(__FUNCTION__) + "()" << " - " << msg 

#define WARN_HERE(msg)\
    qDebug().noquote() << "[WARNING]" << " " << QFileInfo(__FILE__).fileName() << ":"  << __LINE__ << " " << QString(__FUNCTION__) + "()" << " - " << msg 

#define ERROR_HERE(msg)\
    qDebug().noquote() << "[ERROR]" << " " << QFileInfo(__FILE__).fileName() << ":"  << __LINE__ << " " << QString(__FUNCTION__) + "()" << " - " << msg 

#define INFO_HERE(msg)\
    qDebug().noquote() << "[INFO]" << " " << QFileInfo(__FILE__).fileName() << ":"  << __LINE__ << " " << QString(__FUNCTION__) + "()" << " - " << msg 