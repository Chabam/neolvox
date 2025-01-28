#ifndef CT_HANDLEINITEMATTRIBUTE_H
#define CT_HANDLEINITEMATTRIBUTE_H

#include "ct_model/inModel/handle/ct_handleinbase.h"
#include "ct_model/inModel/ct_instditemattributemodel.h"
#include "ct_model/inModel/ct_inresultmodelgrouptocopy.h"

using CT_AbstractHandleInItemAttribute = CT_AbstractInHandleWithModelT<CT_InStdItemAttributeModel>;

template<class ItemAttributeT, int valueType, int min = 1, int max = 1>
class CT_HandleInItemAttribute : public CT_HandleInBase<CT_InStdItemAttributeModel, min, max> {

    using SuperClass = CT_HandleInBase<CT_InStdItemAttributeModel, min, max>;

public:
    enum {
        ValueType = valueType
    };

    using ItemAttributeType = ItemAttributeT;
    using InResultModelCopyType = CT_InResultModelGroupToCopy;

    CT_HandleInItemAttribute() : SuperClass()  {
        static_assert(std::is_convertible<ItemAttributeT*, IItemAttributeForModel*>::value, "CT_HandleInItemAttribute is only compatible with classes that inherit from IItemAttributeForModel");
    }
};

#endif // CT_HANDLEINITEMATTRIBUTE_H
