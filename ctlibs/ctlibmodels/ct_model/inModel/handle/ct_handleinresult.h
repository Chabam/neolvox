#ifndef CT_HANDLEINRESULT_H
#define CT_HANDLEINRESULT_H

#include "ct_model/inModel/handle/ct_inhandleminmaxwithmodelt.h"
#include "ct_model/inModel/ct_inresultmodelgroup.h"

template<class ResultT, int min = 1, int max = 1>
class CT_HandleInResult : public CT_InHandleMinMaxWithModelT<CT_InResultModelGroup, min, max> {

    using SuperClass = CT_InHandleMinMaxWithModelT<CT_InResultModelGroup, min, max>;

public:
    using ResultType = ResultT;

    CT_HandleInResult() : SuperClass() {
        static_assert(std::is_convertible<ResultT*, IResultGroupForModel*>::value, "CT_HandleInResult is only compatible with classes that inherit from IResultGroupForModel");
    }
};
#endif // CT_HANDLEINRESULT_H
