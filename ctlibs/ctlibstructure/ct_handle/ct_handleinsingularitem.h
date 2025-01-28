#ifndef CT_HANDLEINSINGULARITEM_H
#define CT_HANDLEINSINGULARITEM_H

#include "ct_model/inModel/handle/ct_handleinitem.h"
#include "ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.h"
#include "ct_tools/ct_handleiterationdecorator.h"

template<class ItemT = CT_AbstractSingularItemDrawable, int min = 1, int max = 1>
using CT_HandleInSingularItem = CT_HandleIterationDecorator<CT_HandleInItem<ItemT, min, max>, ItemT, CT_StandardItemGroup>;

#endif // CT_HANDLEINSINGULARITEM_H
