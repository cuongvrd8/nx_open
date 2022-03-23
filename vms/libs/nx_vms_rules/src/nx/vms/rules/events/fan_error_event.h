// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <nx/vms/rules/basic_event.h>

namespace nx::vms::rules {

class NX_VMS_RULES_API FanErrorEvent: public BasicEvent
{
    Q_OBJECT
    Q_CLASSINFO("type", "nx.events.fanError")

public:
    static FilterManifest filterManifest();
};

} // namespace nx::vms::rules