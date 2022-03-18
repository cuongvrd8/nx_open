// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <vector>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTimer>

#include <dinput.h>

#include "device.h"

namespace nx::vms::client::desktop::joystick {

struct JoystickDescriptor;

class DeviceWindows: public Device
{
    Q_OBJECT

    using base_type = Device;

public:
    DeviceWindows(
        LPDIRECTINPUTDEVICE8 inputDevice,
        const JoystickDescriptor& modelInfo,
        const QString& path,
        QTimer* pollTimer,
        QObject* parent = 0);

    virtual ~DeviceWindows() override;

    virtual bool isValid() const override;

    bool isInitialized() const;

protected:
    virtual State getNewState() override;
    static bool enumObjectsCallback(LPCDIDEVICEOBJECTINSTANCE deviceObject, LPVOID devicePtr);
    static void formAxisLimits(int min, int max, Device::AxisLimits* limits);

private:
    LPDIRECTINPUTDEVICE8 m_inputDevice;
    bool m_xAxisInitialized = false;
    bool m_yAxisInitialized = false;
    bool m_zAxisInitialized = false;
};

} // namespace nx::vms::client::desktop::joystick