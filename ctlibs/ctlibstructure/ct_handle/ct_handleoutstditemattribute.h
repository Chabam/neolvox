#ifndef CT_HANDLEOUTSTDITEMATTRIBUTE_H
#define CT_HANDLEOUTSTDITEMATTRIBUTE_H

#include "ct_model/outModel/handle/ct_handleoutitemattribute.h"
#include "ct_itemattributes/ct_stditemattributet.h"

template<typename DataType>
class CT_HandleOutStdItemAttribute : public CT_HandleOutItemAttribute<CT_StdItemAttributeT<DataType>> {

    using SuperClass = CT_HandleOutItemAttribute<CT_StdItemAttributeT<DataType>>;

public:
    CT_HandleOutStdItemAttribute() : SuperClass()  {
    }
};

using CT_HandleOutStdIntItemAttribute = CT_HandleOutStdItemAttribute<int>;
using CT_HandleOutStdDoubleItemAttribute = CT_HandleOutStdItemAttribute<double>;
using CT_HandleOutStdBoolItemAttribute = CT_HandleOutStdItemAttribute<bool>;
using CT_HandleOutStdStringItemAttribute = CT_HandleOutStdItemAttribute<QString>;

#endif // CT_HANDLEOUTSTDITEMATTRIBUTE_H
