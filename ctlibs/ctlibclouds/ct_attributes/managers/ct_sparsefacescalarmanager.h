#ifndef CT_SPARSEFACESCALARMANAGER_H
#define CT_SPARSEFACESCALARMANAGER_H

#include "ct_sparseattributemanager.h"
#include "ct_scalarminmaxmanager.h"

template<typename T>
class CT_SparseFaceScalarManager : public CT_SparseAttributeManager<T, CT_NMFCIR>, public CT_ScalarMinMaxManager<T>
{
public:
    using ApplicableTo = CT_NMFCIR::element_type::element_type;

    CT_SparseFaceScalarManager() = default;
};

#endif // CT_SPARSEFACESCALARMANAGER_H
