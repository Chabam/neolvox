#ifndef CT_DENSEPOINTSCALARMANAGER_H
#define CT_DENSEPOINTSCALARMANAGER_H

#include "ct_denseattributemanager.h"
#include "ct_attributes/setters/ct_undefinedpointcloudsizedenseattributesetter.h"
#include "ct_cloudindex/registered/abstract/ct_abstractnotmodifiablecloudindexregisteredt.h"
#include "ct_scalarminmaxmanager.h"

template<typename T>
class CT_DensePointScalarManager : public CT_DenseAttributeManager<T, CT_NMPCIR, CT_Repository::SyncWithPointCloud>, public CT_ScalarMinMaxManager<T>
{
public:
    using ApplicableTo = CT_NMPCIR::element_type::element_type;
    using UPCSSetter = CT_UndefinedPointCloudSizeDenseAttributeSetter<T>;
    using UPCSSetterPtr = std::unique_ptr<UPCSSetter>;

    UPCSSetter createUndefinedPointCloudSizeAttributesSetter(CT_AbstractUndefinedSizePointCloud* uspc);
    UPCSSetterPtr createUndefinedPointCloudSizeAttributesSetterPtr(CT_AbstractUndefinedSizePointCloud* uspc);
};

template<typename T>
typename CT_DensePointScalarManager<T>::UPCSSetter CT_DensePointScalarManager<T>::createUndefinedPointCloudSizeAttributesSetter(CT_AbstractUndefinedSizePointCloud* uspc)
{
    this->createCollectionsIfNotCreated();

    return CT_DensePointScalarManager<T>::UPCSSetter(*uspc,
                                                     this->mAttributes,
                                                     this->mBits);
}

template<typename T>
typename CT_DensePointScalarManager<T>::UPCSSetterPtr CT_DensePointScalarManager<T>::createUndefinedPointCloudSizeAttributesSetterPtr(CT_AbstractUndefinedSizePointCloud* uspc)
{
    this->createCollectionsIfNotCreated();

    return std::make_unique<CT_DensePointScalarManager<T>::UPCSSetter>(*uspc,
                                                                       this->mAttributes,
                                                                       this->mBits);
}

#endif // CT_DENSEPOINTSCALARMANAGER_H

