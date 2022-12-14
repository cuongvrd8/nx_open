// Copyright 2018-present Network Optix, Inc. Licensed under MPL 2.0: www.mozilla.org/MPL/2.0/

#include "model_index_wrapper.h"

#include "utils.h"

namespace nx::vms::client::desktop::testkit {

namespace {

QModelIndex modelIndexWrapperToModelIndex(const ModelIndexWrapper& wrapper)
{
    return wrapper.index();
}

ModelIndexWrapper modelIndexToModelIndexWrapper(const QModelIndex& index)
{
    if (!index.isValid())
        return ModelIndexWrapper();
    return ModelIndexWrapper(index, qobject_cast<QWidget*>(index.model()->parent()));
}

} // namespace

void ModelIndexWrapper::registerMetaType()
{
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        // Avoid multiple metatype register calls.
        if (QMetaType::hasRegisteredComparators(qMetaTypeId<ModelIndexWrapper>()))
            return;

        QMetaType::registerComparators<ModelIndexWrapper>();
    #endif

    QMetaType::registerConverter<ModelIndexWrapper, QModelIndex>(modelIndexWrapperToModelIndex);
    QMetaType::registerConverter<QModelIndex, ModelIndexWrapper>(modelIndexToModelIndexWrapper);
}

QString ModelIndexWrapper::checkStateName(QVariant state)
{
    switch (state.toInt())
    {
        case Qt::Unchecked:
            return "unchecked";
        case Qt::PartiallyChecked:
            return "partiallyChecked";
        case Qt::Checked:
            return "checked";
        default:
            return QString();
    }
}

QVariant ModelIndexWrapper::data(int role) const
{
    return m_index.data(role);
}

QVariantMap ModelIndexWrapper::metaInfo(QString name) const
{
    return utils::getMetaInfo(&this->staticMetaObject, name);
}

} // namespace nx::vms::client::desktop::testkit
