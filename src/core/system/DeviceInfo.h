#pragma once

#include <QHostInfo>
#include <QCoreApplication>
#include <QUuid>
#include <QList>
#include <QString>
#include <QSysInfo>

class DeviceInfo {
    public:
    explicit DeviceInfo();

    QString getDeviceName() const;
    QString getOSName() const;
    QString getProductVersion() const;
    QString getKernelVersion() const;
    QString getApplicationVersion() const;
    QString getDeviceId() const;
    
};