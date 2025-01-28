#ifndef CT_POINTCLOUDPARSEATTRIBUTESMANAGER_H
#define CT_POINTCLOUDPARSEATTRIBUTESMANAGER_H

#include "ct_attributes/managers/ct_sparseattributemanager.h"
#include "ct_attributes/setters/ct_undefinedpointcloudsizesparseattributesetter.h"
#include "ct_scalarminmaxmanager.h"

template<typename T>
class CT_SparsePointScalarManager : public CT_SparseAttributeManager<T, CT_NMPCIR>, public CT_ScalarMinMaxManager<T>
{
public:
    using ApplicableTo = CT_NMPCIR::element_type::element_type;
    using UPCSSetter = CT_UndefinedPointCloudSizeSparseAttributeSetter<T>;
    using UPCSSetterPtr = std::unique_ptr<UPCSSetter>;

    UPCSSetter createUndefinedPointCloudSizeAttributesSetter(CT_AbstractUndefinedSizePointCloud* uspc);
    UPCSSetterPtr createUndefinedPointCloudSizeAttributesSetterPtr(CT_AbstractUndefinedSizePointCloud* uspc);
};

template<typename T>
typename CT_SparsePointScalarManager<T>::UPCSSetter CT_SparsePointScalarManager<T>::createUndefinedPointCloudSizeAttributesSetter(CT_AbstractUndefinedSizePointCloud* uspc)
{
    this->createCollectionsIfNotCreated();

    return CT_SparsePointScalarManager<T>::UPCSSetter(*uspc, this->mAttributes);
}

template<typename T>
typename CT_SparsePointScalarManager<T>::UPCSSetterPtr CT_SparsePointScalarManager<T>::createUndefinedPointCloudSizeAttributesSetterPtr(CT_AbstractUndefinedSizePointCloud* uspc)
{
    this->createCollectionsIfNotCreated();

    return std::make_unique<CT_SparsePointScalarManager<T>::UPCSSetter>(*uspc, this->mAttributes);
}

#endif // CT_POINTCLOUDPARSEATTRIBUTESMANAGER_H
