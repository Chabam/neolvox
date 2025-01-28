#ifndef CT_HANDLEOUTSTDITEMATTRIBUTEWRAPPER_H
#define CT_HANDLEOUTSTDITEMATTRIBUTEWRAPPER_H

#include "ct_model/outModel/handle/ct_handleoutitemattribute.h"
#include "ct_itemattributes/ct_stditemattributewrappert.h"

template<class ClassType, typename DataType>
class CT_HandleOutStdItemAttributeWrapper : public CT_HandleOutItemAttribute<CT_StdItemAttributeWrapperT<ClassType, DataType>> {

    using SuperClass = CT_HandleOutItemAttribute<CT_StdItemAttributeWrapperT<ClassType, DataType>>;

public:
    CT_HandleOutStdItemAttributeWrapper() : SuperClass()  {
    }
};

#endif // CT_HANDLEOUTSTDITEMATTRIBUTEWRAPPER_H

