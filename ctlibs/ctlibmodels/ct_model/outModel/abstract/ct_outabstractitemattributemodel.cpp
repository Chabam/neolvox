#include "ct_outabstractitemattributemodel.h"

CT_OutAbstractItemAttributeModel::CT_OutAbstractItemAttributeModel(const QString& displayableName) : SuperClass(displayableName),
    m_default(false)
{
}

CT_OutAbstractItemAttributeModel::CT_OutAbstractItemAttributeModel(const CT_OutAbstractItemAttributeModel& other) : SuperClass(other),
    m_default(other.m_default)
{
    IItemAttributeForModel* att = other.itemAttribute();

    if(att != nullptr)
        setPrototype(other.itemAttribute()->itemAttributeToolForModel()->copyItemAttribute());
}

IItemAttributeForModel* CT_OutAbstractItemAttributeModel::itemAttribute() const
{
    return static_cast<IItemAttributeForModel*>(prototype());
}

bool CT_OutAbstractItemAttributeModel::isADefaultItemAttributeModel() const
{
    return m_default;
}

void CT_OutAbstractItemAttributeModel::setAsDefaultItemAttributeModel()
{
    m_default = true;
}
