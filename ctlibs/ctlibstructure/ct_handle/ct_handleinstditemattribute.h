#ifndef CT_HANDLEINSTDITEMATTRIBUTE_H
#define CT_HANDLEINSTDITEMATTRIBUTE_H

#include "ct_model/inModel/handle/ct_handleinitemattribute.h"
#include "ct_categories/abstract/ct_abstractcategory.h"
#include "ct_itemattributes/abstract/ct_abstractitemattribute.h"
#include "ct_tools/ct_handleiterationdecorator.h"

template<CT_AbstractCategory::ValueType valueType, int min = 1, int max = 1>
using CT_HandleInStdItemAttribute = CT_HandleIterationDecorator<CT_HandleInItemAttribute<CT_AbstractItemAttribute, valueType, min, max>, CT_AbstractItemAttribute, CT_AbstractSingularItemDrawable>;

#endif // CT_HANDLEINSTDITEMATTRIBUTE_H
