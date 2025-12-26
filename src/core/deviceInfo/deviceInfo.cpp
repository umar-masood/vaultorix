#include "deviceInfo.h"

DeviceInfo::DeviceInfo() {}

// Get Device Name (DESKTOP-5AKC0MOO format)
QString DeviceInfo::getDeviceName() const {
    return QHostInfo::localHostName();
}

// Get OS Name (e.g., "Windows 11 Version 24H2" format)
QString DeviceInfo::getOSName() const {
    return QSysInfo::prettyProductName();
}

// Get Product Version (e.g., "11" format)
QString DeviceInfo::getProductVersion() const {
    return QSysInfo::productVersion();
}

// Get Kernel Version (e.g., "10.0.22601" format)
QString DeviceInfo::getKernelVersion() const {
    return QSysInfo::kernelVersion();
}

// Get Application Version ("1.0.0" format)
QString DeviceInfo::getApplicationVersion() const {
    return QCoreApplication::applicationVersion();
}

QString DeviceInfo::getDeviceId() const {
    return QUuid::createUuid().toString(QUuid::WithoutBraces);
}
