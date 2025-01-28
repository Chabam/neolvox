#ifndef CT_HANDLEOUTGROUP_H
#define CT_HANDLEOUTGROUP_H

#include "ct_model/handle/ct_handlewithmultiplemodelt.h"
#include "ct_model/outModel/ct_outstdgroupmodel.h"

#include <type_traits>

#include <QDebug>

// CT_AbstractHandleOutGroup can only be used with method to add a group in a result or in another group. It can
// not be used with other handle to add a group handle !
using CT_AbstractHandleOutGroup = CT_HandleWithMultipleAbstractModel<DEF_CT_AbstractGroupModelOut>;

template<class GroupT, template<class GroupT2> class ModelT = CT_OutStdGroupModel>
class CT_HandleOutGroup : public CT_HandleWithMultipleModelT<ModelT<GroupT>> {

    using SuperClass = CT_HandleWithMultipleModelT<ModelT<GroupT>> ;

public:
    using GroupType = GroupT;

    CT_HandleOutGroup() : SuperClass() {
        static_assert(std::is_convertible<GroupT*, IGroupForModel*>::value, "CT_HandleOutGroup is only compatible with classes that inherit from IGroupForModel");
    }

    template<typename... Args>
    GroupT* createInstance(Args&&... args) {
        return new GroupT(std::forward<Args>(args)...);
    }
};

#endif // CT_HANDLEOUTGROUP_H
