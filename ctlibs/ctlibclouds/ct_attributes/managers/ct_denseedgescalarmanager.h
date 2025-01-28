#ifndef CT_DENSEEDGESCALARMANAGER_H
#define CT_DENSEEDGESCALARMANAGER_H

#include "ct_denseattributemanager.h"
#include "ct_cloudindex/registered/abstract/ct_abstractnotmodifiablecloudindexregisteredt.h"
#include "ct_scalarminmaxmanager.h"

template<typename T>
class CT_DenseEdgeScalarManager : public CT_DenseAttributeManager<T, CT_NMECIR, CT_Repository::SyncWithEdgeCloud>, public CT_ScalarMinMaxManager<T>
{
public:
    using ApplicableTo = CT_NMECIR::element_type::element_type;

    CT_DenseEdgeScalarManager() = default;
};

#endif // CT_DENSEEDGESCALARMANAGER_H
