// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "cross_system_camera_resource.h"

#include <network/base_system_description.h>
#include <nx/vms/client/desktop/resource/resource_descriptor.h>
#include <nx/vms/client/desktop/system_context.h>
#include <nx/vms/common/system_context.h>

#include "cloud_cross_system_context.h"

namespace nx::vms::client::desktop {

namespace {

const QnUuid kThumbCameraTypeId("{72d232d7-0c67-4d8e-b5a8-a0d5075ff3a4}");

} // namespace

struct CrossSystemCameraResource::Private
{
    CrossSystemCameraResource* const q;
    std::optional<nx::vms::api::CameraDataEx> source;
    QPointer<CloudCrossSystemContext> crossSystemContext;

    QString calculateName() const
    {
        if (crossSystemContext->isSystemReadyToUse() && NX_ASSERT(source))
            return source->name;

        const auto crossSystemContextStatus = crossSystemContext->status();
        if (crossSystemContextStatus == CloudCrossSystemContext::Status::connecting
            || crossSystemContextStatus == CloudCrossSystemContext::Status::connectionFailure)
        {
            return toString(crossSystemContextStatus);
        }

        return toString(CloudCrossSystemContext::Status::uninitialized);
    }
};

CrossSystemCameraResource::CrossSystemCameraResource(
    CloudCrossSystemContext* crossSystemContext,
    const nx::vms::api::CameraDataEx& source)
    :
    QnClientCameraResource(source.typeId),
    d(new Private{
        .q = this,
        .source = source,
        .crossSystemContext = crossSystemContext
    })
{
    addFlags(Qn::cross_system);
    setIdUnsafe(source.id);
    setTypeId(source.typeId);
    setModel(source.model);
    setVendor(source.vendor);
    setPhysicalId(source.physicalId);
    setMAC(nx::utils::MacAddress(source.mac));

    m_name = source.name; //< Set resource name, but not camera name.
    m_url = source.url;
    m_parentId = source.parentId;

    // Test if the camera is a desktop or virtual camera.
    if (source.typeId == nx::vms::api::CameraData::kDesktopCameraTypeId)
        addFlags(Qn::desktop_camera);

    if (source.typeId == nx::vms::api::CameraData::kVirtualCameraTypeId)
        addFlags(Qn::virtual_camera);

    watchOnCrossSystemContext();
}

CrossSystemCameraResource::CrossSystemCameraResource(
    CloudCrossSystemContext* crossSystemContext,
    const nx::vms::common::ResourceDescriptor& descriptor)
    :
    QnClientCameraResource(kThumbCameraTypeId),
    d(new Private{
        .q = this,
        .crossSystemContext = crossSystemContext
    })
{
    setIdUnsafe(descriptor.id);
    addFlags(Qn::cross_system | Qn::fake);

    m_name = d->calculateName(); //< Set resource name, but not camera name.

    watchOnCrossSystemContext();
}

CrossSystemCameraResource::~CrossSystemCameraResource() = default;

void CrossSystemCameraResource::update(nx::vms::api::CameraDataEx data)
{
    if (!d->source)
    {
        // Test if the camera is a desktop or virtual camera.
        if (data.typeId == nx::vms::api::CameraData::kDesktopCameraTypeId)
            addFlags(Qn::desktop_camera);

        if (data.typeId == nx::vms::api::CameraData::kVirtualCameraTypeId)
            addFlags(Qn::virtual_camera);

        removeFlags(Qn::fake);

        setTypeId(data.typeId);
        setModel(data.model);
        setVendor(data.vendor);
        setPhysicalId(data.physicalId);
        setMAC(nx::utils::MacAddress(data.mac));
    }

    NotifierList notifiers;
    {
        NX_MUTEX_LOCKER locker(&m_mutex);

        d->source = std::move(data);

        if (m_parentId != d->source->parentId)
        {
            const auto oldParentId = m_parentId;
            m_parentId = d->source->parentId;
            notifiers <<
                [r = toSharedPointer(this), oldParentId]
                {
                    emit r->parentIdChanged(r, oldParentId);
                };
        }

        if (m_url != d->source->url)
        {
            m_url = d->source->url;
            notifiers << [r = toSharedPointer(this)] { emit r->urlChanged(r); };
        }

        if (m_name != d->source->name)
        {
            m_name = d->source->name;
            notifiers << [r = toSharedPointer(this)] { emit r->nameChanged(r); };
        }

        notifiers <<
            [r = toSharedPointer(this)] { emit r->statusChanged(r, Qn::StatusChangeReason::Local); };
    }

    for (const auto& notifier: notifiers)
        notifier();
}

api::ResourceStatus CrossSystemCameraResource::getStatus() const
{
    // Returns resource status only if the system contains it is connected and online, otherwise
    // calculate status from the system status.
    if (d->crossSystemContext->isSystemReadyToUse())
        return QnClientCameraResource::getStatus();

    const auto crossSystemContextStatus = d->crossSystemContext->status();
    if (crossSystemContextStatus == CloudCrossSystemContext::Status::connecting)
        return api::ResourceStatus::undefined;

    if (crossSystemContextStatus == CloudCrossSystemContext::Status::connectionFailure)
        return api::ResourceStatus::unauthorized;

    return api::ResourceStatus::offline;
}

CloudCrossSystemContext* CrossSystemCameraResource::crossSystemContext() const
{
    return d->crossSystemContext;
}

nx::vms::common::ResourceDescriptor CrossSystemCameraResource::descriptor() const
{
    return nx::vms::client::desktop::descriptor(getId(), d->crossSystemContext->systemId());
}

void CrossSystemCameraResource::watchOnCrossSystemContext()
{
    const auto update =
        [this]
        {
            QnResource::setName(d->calculateName());
            if (d->crossSystemContext->isSystemReadyToUse())
                removeFlags(Qn::fake);
            else
                addFlags(Qn::fake);

            statusChanged(toSharedPointer(), Qn::StatusChangeReason::Local);
        };

    connect(
        d->crossSystemContext,
        &CloudCrossSystemContext::statusChanged,
        this,
        update);

    connect(
        d->crossSystemContext->systemDescription(),
        &QnBaseSystemDescription::onlineStateChanged,
        this,
        update);
}

} // namespace nx::vms::client::desktop
