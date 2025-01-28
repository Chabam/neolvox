#ifndef CT_HANDLEINITEM_H
#define CT_HANDLEINITEM_H

#include "ct_model/inModel/handle/ct_handleinbase.h"
#include "ct_model/inModel/ct_instdsingularitemmodel.h"
#include "ct_model/inModel/ct_inresultmodelgrouptocopy.h"

using CT_AbstractHandleInItem = CT_AbstractInHandleWithModelT<CT_InStdSingularItemModel>;

template<class ItemT, int min = 1, int max = 1>
class CT_HandleInItem : public CT_HandleInBase<CT_InStdSingularItemModel, min, max> {

    using SuperClass = CT_HandleInBase<CT_InStdSingularItemModel, min, max>;

public:
    using ItemType = ItemT;
    using InResultModelCopyType = CT_InResultModelGroupToCopy;

    CT_HandleInItem() : SuperClass() {
        static_assert(std::is_convertible<ItemT*, ISingularItemDrawableForModel*>::value, "CT_HandleInItem is only compatible with classes that inherit from ISingularItemDrawableForModel");
    }
};

#endif // CT_HANDLEINITEM_H
