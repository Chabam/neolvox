#ifndef CT_SPARSEEDGESCALARMANAGER_H
#define CT_SPARSEEDGESCALARMANAGER_H

#include "ct_sparseattributemanager.h"
#include "ct_scalarminmaxmanager.h"

template<typename T>
class CT_SparseEdgeScalarManager : public CT_SparseAttributeManager<T, CT_NMECIR>, public CT_ScalarMinMaxManager<T>
{
public:
    using ApplicableTo = CT_NMECIR::element_type::element_type;

    CT_SparseEdgeScalarManager() = default;
};

#endif // CT_SPARSEEDGESCALARMANAGER_H
