#include "ct_stdlaspointsattributescontainershortcut.h"

CT_TYPE_IMPL_INIT_MACRO(CT_StdLASPointsAttributesContainerShortcut)

CT_StdLASPointsAttributesContainerShortcut::CT_StdLASPointsAttributesContainerShortcut() : SuperClass()
{
    _container = nullptr;
}

CT_StdLASPointsAttributesContainerShortcut::CT_StdLASPointsAttributesContainerShortcut(CT_StdLASPointsAttributesContainer *container) : SuperClass()
{
    _container = container;
}

void CT_StdLASPointsAttributesContainerShortcut::insertPointsAttributesAt(CT_LasDefine::LASPointAttributesType key, const CT_AbstractPointAttributesScalar *att)
{
    Q_UNUSED(key);
    Q_UNUSED(att);
    // do nothing
    qFatal("CT_StdLASPointsAttributesContainerShortcut class must not been used for inserting new attributes");
}

CT_AbstractPointsAttributes *CT_StdLASPointsAttributesContainerShortcut::pointsAttributesAt(const int &key) const
{
    if (_container != nullptr)
        return _container->pointsAttributesAt(key);

    qFatal("CT_StdLASPointsAttributesContainerShortcut class not correctly initialized");
    return nullptr;

}

QList<CT_LasDefine::LASPointAttributesType> CT_StdLASPointsAttributesContainerShortcut::lasPointAttributesTypes() const
{
    if (_container != nullptr)
        return _container->lasPointAttributesTypes();

    qFatal("CT_StdLASPointsAttributesContainerShortcut class not correctly initialized");
    return QList<CT_LasDefine::LASPointAttributesType>();
}

bool CT_StdLASPointsAttributesContainerShortcut::getLASDataAt(const size_t i, CT_LASData &data) const
{
    if (_container != nullptr)
        return _container->getLASDataAt(i, data);

    qFatal("CT_StdLASPointsAttributesContainerShortcut class not correctly initialized");
    return false;
}

QHash<CT_LasDefine::LASPointAttributesType, CT_AbstractPointAttributesScalar *> CT_StdLASPointsAttributesContainerShortcut::lasPointsAttributes() const
{
    if (_container != nullptr)
        return _container->lasPointsAttributes();

    qFatal("CT_StdLASPointsAttributesContainerShortcut class not correctly initialized");
    return QHash<CT_LasDefine::LASPointAttributesType, CT_AbstractPointAttributesScalar *>();
}
