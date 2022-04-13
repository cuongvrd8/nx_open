// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <QtCore/QJsonObject>

#include <nx/utils/uuid.h>

#include <nx/fusion/model_functions_fwd.h>

#include <nx/vms/api/types/motion_types.h>

#include "settings.h"

namespace nx::vms::api::analytics {

struct NX_VMS_API DeviceAgentActiveSettingChangedRequest
{
    /**%apidoc
     * Id of Device.
     */
    QString deviceId;

    /**%apidoc
     * Unique id of an Analytics Engine.
     */
    QnUuid analyticsEngineId;

    /**%apidoc
     * Id of a setting which triggered the notification.
     */
    QString activeSettingId;

    /**%apidoc
     * Name-value map with setting values, using JSON types corresponding to each setting type.
     */
    SettingsValues settingsValues;

    /**%apidoc
     * Settings model.
     */
    SettingsModel settingsModel;
};
#define nx_vms_api_analytics_DeviceAgentActiveSettingChangedRequest_Fields \
    (deviceId) \
    (analyticsEngineId) \
    (activeSettingId) \
    (settingsValues) \
    (settingsModel)

QN_FUSION_DECLARE_FUNCTIONS(DeviceAgentActiveSettingChangedRequest, (json), NX_VMS_API)

} // namespace nx::vms::api::analytics
