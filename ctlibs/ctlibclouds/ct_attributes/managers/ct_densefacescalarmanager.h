#ifndef CT_DENSEFACESCALARMANAGER_H
#define CT_DENSEFACESCALARMANAGER_H

#include "ct_denseattributemanager.h"
#include "ct_cloudindex/registered/abstract/ct_abstractnotmodifiablecloudindexregisteredt.h"
#include "ct_scalarminmaxmanager.h"

template<typename T>
class CT_DenseFaceScalarManager : public CT_DenseAttributeManager<T, CT_NMFCIR, CT_Repository::SyncWithFaceCloud>, public CT_ScalarMinMaxManager<T>
{
public:
    using ApplicableTo = CT_NMFCIR::element_type::element_type;

    CT_DenseFaceScalarManager() = default;
};

#endif // CT_DENSEFACESCALARMANAGER_H
