#ifndef CT_HANDLEINRESULTGROUP_H
#define CT_HANDLEINRESULTGROUP_H

#include "ct_model/inModel/handle/ct_handleinresult.h"
#include "ct_result/ct_resultgroup.h"
#include "ct_iterator/ct_multimodeliteratorstdstyle.h"
#include "ct_iterator/ct_singlemodeliteratorstdstyleforresultgroup.h"
#include "ct_tools/ct_handleiteratort.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"

template<int min = 1, int max = 1>
class CT_HandleInResultGroup : public CT_HandleInResult<CT_ResultGroup, min, max> {
    using SingleModelIterator = CT_SingleModelIteratorStdStyleForResultGroup<CT_ResultGroup>;
    using ConstMultiModelIterator = CT_MultiModelIteratorStdStyle<SingleModelIterator, const CT_ResultGroup>;
public:
    using const_iterator = ConstMultiModelIterator;
    using final_const_iterator = CT_HandleIteratorT<const_iterator>;

    CT_HandleInResultGroup() = default;

    /**
     * @brief Iterate over input results
     */
    final_const_iterator iterateInputs() const {
        QVector<CT_OutAbstractResultModel*> outModels;
        int currentIndex = 0;

        const CT_HandleInResultGroup<>::ModelType* inResultModel = this->model();

        MODELS_ASSERT(inResultModel != nullptr);
        if (inResultModel == nullptr) {qDebug() << "CT_HandleInResultGroup::iterateInputs" << ", " <<  "inResultModel == nullptr";}

        outModels.resize(inResultModel->nPossibilitySelected());

        inResultModel->possibilitiesGroup()->visitSelectedPossibilities([&outModels, &currentIndex](const CT_InStdModelPossibility* possibility) -> bool {
            outModels[currentIndex++] = static_cast<CT_OutAbstractResultModel*>(possibility->outModel());
            return true;
        });

        return final_const_iterator(const_iterator::create(outModels.begin(), outModels.end()), const_iterator());
    }
};

#endif // CT_HANDLEINRESULTGROUP_H
