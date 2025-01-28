#ifndef CT_HANDLEWITHMULTIPLEABSTRACTMODEL_H
#define CT_HANDLEWITHMULTIPLEABSTRACTMODEL_H

#include "ct_model/outModel/abstract/ct_outabstractmodel.h"

template<class AbstractModelT>
class CT_HandleWithMultipleAbstractModel {
public:
    CT_HandleWithMultipleAbstractModel() {}
    virtual ~CT_HandleWithMultipleAbstractModel() { }

    virtual AbstractModelT* findAbstractModelWithParent(const CT_OutAbstractModel* parentModel) const = 0;
};

#endif // CT_HANDLEWITHMULTIPLEABSTRACTMODEL_H
