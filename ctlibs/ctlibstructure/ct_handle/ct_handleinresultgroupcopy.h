#ifndef CT_HANDLEINRESULTGROUPCOPY_H
#define CT_HANDLEINRESULTGROUPCOPY_H

#include "ct_model/inModel/handle/ct_handleinresultcopy.h"
#include "ct_result/ct_resultgroup.h"
#include "ct_iterator/ct_multimodeliteratorstdstyle.h"
#include "ct_iterator/ct_singlemodeliteratorstdstyleforresultgroup.h"
#include "ct_tools/ct_handleiteratort.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"

template<int min = 1, int max = 1>
class CT_HandleInResultGroupCopy : public CT_HandleInResultCopy<CT_ResultGroup, min, max> {
    using SingleModelIterator = CT_SingleModelIteratorStdStyleForResultGroup<CT_ResultGroup>;
    using MultiModelIterator = CT_MultiModelIteratorStdStyle<SingleModelIterator, CT_ResultGroup>;
    using ConstMultiModelIterator = CT_MultiModelIteratorStdStyle<SingleModelIterator, const CT_ResultGroup>;

public:
    using iterator = MultiModelIterator;
    using final_iterator = CT_HandleIteratorT<iterator>;
    using const_iterator = ConstMultiModelIterator;
    using final_const_iterator = CT_HandleIteratorT<const_iterator>;

    CT_HandleInResultGroupCopy() = default;

    /**
     * @brief Iterate over input results
     */
    final_const_iterator iterateInputs() const {
        QVector<CT_OutAbstractResultModel*> outModels;
        int currentIndex = 0;

        const CT_HandleInResultGroupCopy<>::ModelType* inResultModel = this->model();

        MODELS_ASSERT(inResultModel != nullptr);
        if (inResultModel == nullptr) {qDebug() << "CT_HandleInResultGroupCopy::iterateInputs" << ", " <<  "inResultModel == nullptr";}

        outModels.resize(inResultModel->nPossibilitySelected());

        inResultModel->possibilitiesGroup()->visitSelectedPossibilities([&outModels, &currentIndex](const CT_InStdModelPossibility* possibility) -> bool {
            outModels[currentIndex++] = static_cast<CT_OutAbstractResultModel*>(possibility->outModel());
            return true;
        });

        return final_const_iterator(const_iterator::create(outModels.begin(), outModels.end()), const_iterator());
    }

    /**
     * @brief Iterate over copies (output results)
     */
    final_iterator iterateOutputs() const {
        using InResultToolType = CT_HandleInResultGroupCopy<>::ModelType::ToolToModifyResultModelCopiesType;
        using OutResultModelType = InResultToolType::ModelType;

        QVector<CT_OutAbstractResultModel*> outModels;
        int currentIndex = 0;

        const CT_HandleInResultGroupCopy<>::ModelType* inResultModel = this->model();

        MODELS_ASSERT(inResultModel != nullptr);
        if (inResultModel == nullptr) {qDebug() << "CT_HandleInResultGroupCopy::iterateOutputs" << ", " <<  "inResultModel == nullptr";}

        InResultToolType* tool = inResultModel->toolToModifyResultModelCopies();

        MODELS_ASSERT(tool != nullptr);
        if (tool == nullptr) {qDebug() << "CT_HandleInResultGroupCopy::iterateOutputs" << ", " <<  "tool == nullptr";}

        outModels.resize(tool->nResultModels());

        const auto visitor = [&outModels, &currentIndex](const OutResultModelType* outResultModel) -> bool {
            outModels[currentIndex++] = const_cast<OutResultModelType*>(outResultModel);
            return true;
        };

        tool->visitOutResultModelCopies(visitor);

        return final_iterator(iterator::create(outModels.begin(), outModels.end()), iterator());
    }
};

#endif // CT_HANDLEINRESULTGROUPCOPY_H
