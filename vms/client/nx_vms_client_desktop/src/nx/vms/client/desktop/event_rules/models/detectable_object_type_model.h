// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#pragma once

#include <QtCore/QAbstractListModel>

#include <nx/utils/impl_ptr.h>
#include <nx/utils/scoped_model_operations.h>

namespace nx::analytics::taxonomy { class AbstractEngine; }

namespace nx::vms::client::desktop {

namespace analytics { class TaxonomyManager; }

/**
 * An item model providing a tree of analytics detectable object types available in the system.
 */
class DetectableObjectTypeModel: public ScopedModelOperations<QAbstractItemModel>
{
    Q_OBJECT
    using base_type = ScopedModelOperations<QAbstractItemModel>;
    using TaxonomyManager = nx::vms::client::desktop::analytics::TaxonomyManager;

public:
    explicit DetectableObjectTypeModel(TaxonomyManager* taxonomyManager, QObject* parent = nullptr);
    virtual ~DetectableObjectTypeModel() override;

    virtual QModelIndex index(int row, int column, const QModelIndex& parent) const override;
    virtual QModelIndex parent(const QModelIndex& index) const override;

    virtual int rowCount(const QModelIndex& parent) const override;
    virtual int columnCount(const QModelIndex& parent) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    bool liveTypesExcluded() const;
    void setLiveTypesExcluded(bool value);

    nx::analytics::taxonomy::AbstractEngine* engine() const;
    void setEngine(nx::analytics::taxonomy::AbstractEngine* value);

    enum Roles
    {
        NameRole = Qt::DisplayRole,
        IdRole = Qt::UserRole
    };

private:
    class Private;
    nx::utils::ImplPtr<Private> d;
};

} // namespace nx::vms::client::desktop
