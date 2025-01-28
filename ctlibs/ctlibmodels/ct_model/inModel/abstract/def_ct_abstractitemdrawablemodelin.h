#ifndef DEF_CT_ABSTRACTITEMDRAWABLEMODELIN_H
#define DEF_CT_ABSTRACTITEMDRAWABLEMODELIN_H

#include "ct_model/abstract/ct_abstractsingularitemmodelt.h"

#include "ct_model/inModel/abstract/def_ct_abstractgroupmodelin.h"
#include "ct_model/inModel/abstract/ct_inabstractsingularitemmodel.h"
#include "ct_model/inModel/abstract/ct_inabstractitemattributemodel.h"

using DEF_CT_AbstractItemDrawableModelIn = CT_AbstractSingularItemModelT<DEF_CT_AbstractGroupModelIn, CT_InAbstractItemAttributeModel, CT_InAbstractSingularItemModel>;

#endif // DEF_CT_ABSTRACTITEMDRAWABLEMODELIN_H
