#ifndef CT_HANDLEINGROUP_H
#define CT_HANDLEINGROUP_H

#include "ct_model/inModel/handle/ct_handleinbase.h"
#include "ct_model/inModel/ct_instdgroupmodel.h"
#include "ct_model/inModel/ct_inresultmodelgrouptocopy.h"

using CT_AbstractHandleInGroup = CT_AbstractInHandleWithModelT<CT_InStdGroupModel>;

template<class GroupT, int min = 1, int max = 1>
class CT_HandleInGroup : public CT_HandleInBase<CT_InStdGroupModel, min, max> {

    using SuperClass = CT_HandleInBase<CT_InStdGroupModel, min, max>;

public:
    using GroupType = GroupT;
    using InResultModelCopyType = CT_InResultModelGroupToCopy;

    CT_HandleInGroup() : SuperClass() {
        static_assert(std::is_convertible<GroupT*, IGroupForModel*>::value, "CT_HandleInGroup is only compatible with classes that inherit from IGroupForModel");
    }
};
#endif // CT_HANDLEINGROUP_H
