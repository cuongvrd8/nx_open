// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "initializer.h"

#include <nx/vms/common/system_context.h>
#include <nx/vms/rules/action_fields/builtin_fields.h>
#include <nx/vms/rules/actions/builtin_actions.h>
#include <nx/vms/rules/event_fields/builtin_fields.h>
#include <nx/vms/rules/events/builtin_events.h>

namespace nx::vms::rules {

Initializer::Initializer(nx::vms::common::SystemContext* context):
    SystemContextAware(context)
{
}

Initializer::~Initializer()
{
}

void Initializer::registerEvents() const
{
    // Register built-in events.
    registerEvent<AnalyticsObjectEvent>();
    registerEvent<DeviceDisconnectedEvent>();
    registerEvent<DeviceIpConflictEvent>();
    registerEvent<DebugEvent>();
    registerEvent<FanErrorEvent>();
    registerEvent<GenericEvent>();
    registerEvent<LicenseIssueEvent>();
    registerEvent<MotionEvent>();
    registerEvent<NetworkIssueEvent>();
    registerEvent<PoeOverBudgetEvent>();
    registerEvent<ServerCertificateErrorEvent>();
    registerEvent<ServerConflictEvent>();
    registerEvent<ServerFailureEvent>();
    registerEvent<ServerStartedEvent>();
    registerEvent<SoftTriggerEvent>();
    registerEvent<StorageIssueEvent>();
}

void Initializer::registerActions() const
{
    // Register built-in actions.
    registerAction<HttpAction>();
    registerAction<NotificationAction>();
    registerAction<SendEmailAction>();
    registerAction<TextOverlayAction>();
}

void Initializer::registerFields() const
{
    registerEventField<AnalyticsEventTypeField>();
    registerEventField<AnalyticsObjectAttributesField>();
    m_engine->registerEventField(
        fieldMetatype<AnalyticsObjectTypeField>(),
        [this] { return new AnalyticsObjectTypeField(this->m_context); });
    registerEventField<CustomizableIconField>();
    registerEventField<CustomizableTextField>();
    registerEventField<EventTextField>();
    registerEventField<ExpectedUuidField>();
    registerEventField<IntField>();
    registerEventField<KeywordsField>();
    registerEventField<SourceCameraField>();
    registerEventField<SourceServerField>();
    registerEventField<SourceUserField>();
    registerEventField<StateField>();

    registerActionField<ActionTextField>();
    registerActionField<ContentTypeField>();
    registerActionField<FlagField>();
    registerActionField<HttpMethodField>();
    registerActionField<OptionalTimeField>();
    registerActionField<PasswordField>();
    registerActionField<TargetDeviceField>();
    registerActionField<TargetUserField>();
    m_engine->registerActionField(
        fieldMetatype<TextWithFields>(),
        [this] { return new TextWithFields(this->m_context); });
    registerActionField<Substitution>();
}

} // namespace nx::vms::rules