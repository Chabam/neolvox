#ifndef CT_HANDLEINSTDITEMATTRIBUTET_H
#define CT_HANDLEINSTDITEMATTRIBUTET_H

#include "ct_model/inModel/handle/ct_handleinitemattribute.h"
#include "ct_categories/abstract/ct_abstractcategory.h"
#include "ct_itemattributes/abstract/ct_abstractitemattributet.h"
#include "ct_tools/ct_handleiterationdecorator.h"

template<typename DataT, int min = 1, int max = 1>
using CT_HandleInStdItemAttributeT = CT_HandleIterationDecorator<CT_HandleInItemAttribute<CT_AbstractItemAttributeT<DataT>, CT_AbstractCategory::staticValueTypeToCategoryType<DataT>(), min, max>, CT_AbstractItemAttributeT<DataT>>;

#endif // CT_HANDLEINSTDITEMATTRIBUTET_H
