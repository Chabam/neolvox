#ifndef CT_HANDLEOUTRESULT_H
#define CT_HANDLEOUTRESULT_H

#include "ct_model/handle/ct_handlewithmodelt.h"
#include "ct_model/outModel/ct_outresultmodelgroup.h"

#include <QDebug>

template<class ResultT>
class CT_HandleOutResult : public CT_HandleWithModelT<CT_OutResultModelGroup<ResultT>> {

    using SuperClass = CT_HandleWithModelT<CT_OutResultModelGroup<ResultT>>;

public:
    using ResultType = ResultT;

    CT_HandleOutResult() : SuperClass() {
        static_assert(std::is_convertible<ResultT*, IResultGroupForModel*>::value, "CT_HandleOutResult is only compatible with classes that inherit from IResultGroupForModel");
    }

    ResultT* createInstance() {
        return static_cast<ResultT*>(this->model()->createResult());
    }
};

#endif // CT_HANDLEOUTRESULT_H
