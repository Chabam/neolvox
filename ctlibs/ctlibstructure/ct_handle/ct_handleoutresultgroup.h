#ifndef CT_HANDLEOUTRESULTGROUP_H
#define CT_HANDLEOUTRESULTGROUP_H

#include "ct_model/outModel/handle/ct_handleoutresult.h"
#include "ct_result/ct_resultgroup.h"
#include "ct_iterator/ct_multimodeliteratorstdstyle.h"
#include "ct_iterator/ct_singlemodeliteratorstdstyleforresultgroup.h"
#include "ct_tools/ct_handleiteratort.h"

class CT_HandleOutResultGroup : public CT_HandleOutResult<CT_ResultGroup> {
    using SingleModelIterator = CT_SingleModelIteratorStdStyleForResultGroup<CT_ResultGroup>;
    using MultiModelIterator = CT_MultiModelIteratorStdStyle<SingleModelIterator, CT_ResultGroup>;

public:
    using iterator = MultiModelIterator;
    using final_iterator = CT_HandleIteratorT<iterator>;

    CT_HandleOutResultGroup() = default;

    /**
     * @brief Iterate over output results
     */
    final_iterator iterateOutputs() const {

        QVector<CT_OutAbstractResultModel*> outModels(1);
        outModels[0] = model();

        return final_iterator(iterator::create(outModels.begin(), outModels.end()), iterator());
    }
};

#endif // CT_HANDLEOUTRESULTGROUP_H
