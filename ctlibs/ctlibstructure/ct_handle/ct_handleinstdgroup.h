#ifndef CT_HANDLEINSTDGROUP_H
#define CT_HANDLEINSTDGROUP_H

#include "ct_model/inModel/handle/ct_handleingroup.h"
#include "ct_itemdrawable/ct_standarditemgroup.h"
#include "ct_tools/ct_handleiterationdecorator.h"

template<int min = 1, int max = 1>
using CT_HandleInStdGroup = CT_HandleIterationDecorator<CT_HandleInGroup<CT_StandardItemGroup, min, max>, CT_StandardItemGroup>;

#endif // CT_HANDLEINSTDGROUP_H
