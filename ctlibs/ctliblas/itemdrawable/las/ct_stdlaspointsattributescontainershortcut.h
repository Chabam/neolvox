#ifndef CT_STDLASPOINTSATTRIBUTESCONTAINERSHORTCUT_H
#define CT_STDLASPOINTSATTRIBUTESCONTAINERSHORTCUT_H

#include "ctliblas/ctliblas_global.h"

#include "ctliblas/itemdrawable/las/ct_stdlaspointsattributescontainer.h"

/**
 *  This class is a shortcut class for an existing standard container for LAS points attributes
 */
class CTLIBLAS_EXPORT CT_StdLASPointsAttributesContainerShortcut : public CT_StdLASPointsAttributesContainer
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_StdLASPointsAttributesContainerShortcut, CT_StdLASPointsAttributesContainer, LAS Point attributes shortcut)

    using SuperClass = CT_StdLASPointsAttributesContainer;

public:     

    CT_StdLASPointsAttributesContainerShortcut();
    CT_StdLASPointsAttributesContainerShortcut(CT_StdLASPointsAttributesContainer* container);

    CT_StdLASPointsAttributesContainerShortcut(const CT_StdLASPointsAttributesContainerShortcut& other) = default;

    void insertPointsAttributesAt(CT_LasDefine::LASPointAttributesType key, const CT_AbstractPointAttributesScalar *att) override;

    CT_AbstractPointsAttributes* pointsAttributesAt(const int &key) const override;

    QList<CT_LasDefine::LASPointAttributesType> lasPointAttributesTypes() const override;

    bool getLASDataAt(const size_t i, CT_LASData &data) const override;

    QHash<CT_LasDefine::LASPointAttributesType, CT_AbstractPointAttributesScalar*> lasPointsAttributes() const override;

    CT_ITEM_COPY_IMP(CT_StdLASPointsAttributesContainerShortcut)

private:

    CT_StdLASPointsAttributesContainer* _container;
};

#endif // CT_STDLASPOINTSATTRIBUTESCONTAINERSHORTCUT_H
