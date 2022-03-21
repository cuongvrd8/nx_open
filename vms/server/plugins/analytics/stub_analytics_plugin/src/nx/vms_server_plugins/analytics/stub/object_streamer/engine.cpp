// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "engine.h"

#include "device_agent.h"
#include "common.h"

namespace nx {
namespace vms_server_plugins {
namespace analytics {
namespace stub {
namespace object_streamer {

using namespace nx::sdk;
using namespace nx::sdk::analytics;

static const std::string kDefaultManifestFile = "manifest.json";
static const std::string kDefaultStreamFile = "stream.json";

Engine::Engine(Plugin* plugin):
    nx::sdk::analytics::Engine(/*enableOutput*/ true),
    m_plugin(plugin)
{
}

Engine::~Engine()
{
}

void Engine::doObtainDeviceAgent(Result<IDeviceAgent*>* outResult, const IDeviceInfo* deviceInfo)
{
    *outResult = new DeviceAgent(deviceInfo);
}

std::string Engine::manifestString() const
{
    const std::string pluginHomeDir = m_plugin->utilityProvider()->homeDir();

    return /*suppress newline*/ 1 + (const char*)
R"json(
{
    "streamTypeFilter": "compressedVideo",
    "deviceAgentSettingsModel":
    {
        "type": "Settings",
        "items":
        [
            {
                "type": "TextArea",
                "name": ")json" + kManifestFileSetting + R"json(",
                "caption": "Path to Device Agent Manifest file",
                "defaultValue": ")json" + pluginHomeDir + "/" + kDefaultManifestFile + R"json("
            },
            {
                "type": "TextArea",
                "name": ")json" + kObjectStreamFileSetting + R"json(",
                "caption": "Path to Object stream file",
                "defaultValue": ")json" + pluginHomeDir + "/" +  kDefaultStreamFile + R"json("
            }
        ]
    }
}
)json";
}

} // namespace object_streamer
} // namespace stub
} // namespace analytics
} // namespace vms_server_plugins
} // namespace nx
