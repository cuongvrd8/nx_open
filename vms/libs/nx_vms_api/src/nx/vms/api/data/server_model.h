// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <tuple>
#include <optional>
#include <type_traits>

#include <nx/fusion/model_functions_fwd.h>
#include <nx/reflect/instrument.h>

#include "storage_model.h"
#include "module_information.h" //< For nx::utils::OsInfo fusion.
#include "media_server_data.h"

namespace nx::vms::api {

/**%apidoc Server information object.
 * %param[opt]:object parameters Server-specific key-value parameters.
 */
struct NX_VMS_API ServerModel: ResourceWithParameters
{
    QnUuid id;

    /**%apidoc
     * %example Server 1
     */
    QString name;

    /**%apidoc
     * %example https://127.0.0.1:7001
     */
    QString url;

    /**%apidoc[opt] */
    QString version;

    /**%apidoc[opt] */
    std::vector<QString> endpoints;
    std::optional<QString> authKey;

    /**%apidoc[readonly] */
    std::optional<nx::utils::OsInfo> osInfo;
    std::optional<QnUuid> metadataStorageId;

    /**%apidoc[opt] */
    ServerFlags flags = SF_None;

    /**%apidoc[opt] */
    bool isFailoverEnabled = false;
    std::optional<int> maxCameras = 0;

    /**%apidoc[opt]:array
     * Backup bitrate per day of week and hour, as a JSON array of name-value objects, structured
     * according to the following example:
     * <pre><code>
     * [
     *     {
     *         "key": { "day": "DAY_OF_WEEK", "hour": HOUR },
     *         "value": BYTES_PER_SECOND
     *     },
     *     ...
     * ]
     * </code></pre>
     * Here <code>DAY_OF_WEEK</code> is one of <code>monday</code>, <code>tuesday</code>,
     * <code>wednesday</code>, <code>thursday</code>, <code>friday</code>, <code>saturday</code>,
     * <code>sunday</code>; <code>HOUR</code> is an integer in range 0..23;
     * <code>BYTES_PER_SECOND</code> is an integer.
     * <br/>
     * For any day-hour position, a missing value means "unlimited bitrate", and a zero value means
     * "don't perform backup".
     * %example [{"key": {"day": "monday", "hour": 0}, "value": 0}]
     */
    BackupBitrateBytesPerSecond backupBitrateBytesPerSecond;

    std::optional<ResourceStatus> status;
    std::optional<std::vector<StorageModel>> storages;

    using DbReadTypes = std::tuple<
        MediaServerData,
        MediaServerUserAttributesData,
        ResourceStatusDataList,
        ResourceParamWithRefDataList,
        StorageDataList>;

    using DbUpdateTypes = std::tuple<
        MediaServerData,
        std::optional<MediaServerUserAttributesData>,
        std::optional<ResourceStatusData>,
        ResourceParamWithRefDataList>;

    using DbListTypes = std::tuple<
        MediaServerDataList,
        MediaServerUserAttributesDataList,
        ResourceStatusDataList,
        ResourceParamWithRefDataList,
        StorageDataList>;

    QnUuid getId() const { return id; }
    void setId(QnUuid id_) { id = std::move(id_); }
    static_assert(isCreateModelV<ServerModel>);
    static_assert(isUpdateModelV<ServerModel>);
    static_assert(isFlexibleIdModelV<ServerModel>);

    DbUpdateTypes toDbTypes() &&;
    static std::vector<ServerModel> fromDbTypes(DbListTypes data);

    void extractFromList(const QnUuid& id, ResourceParamWithRefDataList* list);
};
#define ServerModel_Fields \
    (id)(name)(url)(version)(endpoints)(authKey)(osInfo)(metadataStorageId)(flags) \
    (isFailoverEnabled)(maxCameras)(backupBitrateBytesPerSecond)(status)(storages)(parameters)
QN_FUSION_DECLARE_FUNCTIONS(ServerModel, (json), NX_VMS_API)
NX_REFLECTION_INSTRUMENT(ServerModel, ServerModel_Fields);

} // namespace nx::vms::api
