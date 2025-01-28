#ifndef CT_HANDLEITERATIONDECORATOR_H
#define CT_HANDLEITERATIONDECORATOR_H

#include "tools/sfinae.h"
#include "ct_iterator/ct_singlecollectioniteratorstdstyle.h"
#include "ct_iterator/ct_multimodeliteratorstdstyle.h"
#include "ct_iterator/ct_singlemodeliteratorstdstyleforresultgroup.h"
#include "ct_handleiteratort.h"

template<typename SuperClass, typename ItemT, typename ParentT = CT_AbstractItem>
class CT_HandleIterationDecorator : public SuperClass {
    using SingleModelIterator = CT_SingleModelIteratorStdStyleForResultGroup<ItemT, ParentT>;
    using MultiModelIterator = CT_MultiModelIteratorStdStyle<SingleModelIterator, ItemT>;
    using ConstMultiModelIterator = CT_MultiModelIteratorStdStyle<SingleModelIterator, const ItemT>;

public:
    using iterator = MultiModelIterator;
    using final_iterator = CT_HandleIteratorT<iterator>;
    using const_iterator = ConstMultiModelIterator;
    using final_const_iterator = CT_HandleIteratorT<const_iterator>;

    CT_HandleIterationDecorator() = default;

    template<class HandleInResult>
    const ItemT* firstInput(const HandleInResult& inResult) const
    {
        for(const ItemT* item : iterateInputs(inResult))
        {
            return item;
        }

        return nullptr;
    }

    template<class HandleInResult>
    final_const_iterator iterateInputs(const HandleInResult& inResult) const {
        return iterateModelOrItem<final_const_iterator, CT_OutAbstractModel, HandleInResult>(inResult);
    }

    template<class HandleInResult>
    final_iterator iterateOutputs(const HandleInResult& inResult) const {
        return internalIterateOutputs(inResult);
    }

    template<class OutModelT = CT_OutAbstractModel, class HandleInResult>
    CT_HandleIteratorT<CT_SingleCollectionIteratorStdStyle<OutModelT>> iterateSelectedOutputModels(const HandleInResult& inResult) const {
        return iterateModelOrItem<CT_HandleIteratorT<CT_SingleCollectionIteratorStdStyle<OutModelT>>, OutModelT, HandleInResult>(inResult);
    }

private:
    template<typename FinalIterator, typename OutModelT, class HandleInResult>
    FinalIterator iterateModelOrItem(const HandleInResult& inResult) const {
        using IntermediateIterator = typename FinalIterator::IteratorType;

        QVector<OutModelT*> outModels;
        int currentIndex = 0;

        const CT_InAbstractModel* inResultModel = inResult.model();

        if(inResultModel == nullptr)
            return FinalIterator(IntermediateIterator(), IntermediateIterator());

        const int nResultPossibility = inResultModel->nPossibilitySelected();

        for(int i=0; i<nResultPossibility; ++i) {
            const CT_InStdModelPossibilitySelectionGroup* selectionGroup = this->inModelForSelectedPossibilities(inResult, i)->possibilitiesGroup();

            outModels.resize(outModels.size() + selectionGroup->nPossibilitySelected());

            for(const CT_InStdModelPossibility* possibility : selectionGroup->selectedPossibilities()) {
                outModels[currentIndex++] = static_cast<OutModelT*>(possibility->outModel());
            }
        }

        return FinalIterator(IntermediateIterator::create(outModels.begin(), outModels.end()), IntermediateIterator());
    }

    // iterate with a result that is a copy
    template<class HandleInResultCopy>
    final_iterator internalIterateOutputs(const HandleInResultCopy& inResultCopy) const {
        static_assert(IsAResultModelCopy<typename HandleInResultCopy::ModelType>::value, "You can not modify input results, so this iterate method is disabled with an intput handle that is not a copy ! Use method \"iterateInputs(...)\" instead.");

        QVector<DEF_CT_AbstractGroupModelOut*> outModels;
        findOutModelsFromResultCopy(inResultCopy, outModels);

        return final_iterator(iterator::create(outModels.begin(), outModels.end()), iterator());
    }

    template<class HandleInResultCopy>
    void findOutModelsFromResultCopy(const HandleInResultCopy& inResultCopy, QVector<DEF_CT_AbstractGroupModelOut*>& outModels) const {
        using InResultModelType = typename HandleInResultCopy::ModelType;
        using InResultToolType = typename InResultModelType::ToolToModifyResultModelCopiesType;
        using OutResultModelType = typename InResultToolType::ModelType;

        int currentIndex = 0;

        const InResultModelType* inResultModel = inResultCopy.model();

        if(inResultModel == nullptr)
            return;

        InResultToolType* tool = inResultModel->toolToModifyResultModelCopies();

        MODELS_ASSERT(tool != nullptr);
        if (tool == nullptr) {qDebug() << "CT_ResultGroup::findOutModelsFromResultCopy" << ", " << "tool == nullptr";}

        const int nResultPossibility = inResultModel->nPossibilitySelected();

        for(int i=0; i<nResultPossibility; ++i) {
            const CT_InStdModelPossibilitySelectionGroup* selectionGroup = this->inModelForSelectedPossibilities(inResultCopy, i)->possibilitiesGroup();

            outModels.resize(outModels.size() + selectionGroup->nPossibilitySelected());

            for(const CT_InStdModelPossibility* possibility : selectionGroup->selectedPossibilities()) {
                const CT_OutAbstractModel* originalOutModel = possibility->outModel();

                const auto visitor = [&originalOutModel, &outModels, &currentIndex](const OutResultModelType* outResultModel) -> bool {

                    CT_OutAbstractModel* copiedOutModel = outResultModel->recursiveSearchTheModelThatWasACopiedModelFromThisOriginalModel(originalOutModel);

                    Q_ASSERT(copiedOutModel != nullptr);
                    if (copiedOutModel == nullptr) {qDebug() << "CT_ResultGroup::findOutModelsFromResultCopy" << ", " << "copiedOutModel == nullptr";}

                    outModels[currentIndex++] = static_cast<DEF_CT_AbstractGroupModelOut*>(copiedOutModel);
                    return true;
                };

                tool->visitOutResultModelCopies(visitor);
            }
        }
    }
};

#endif // CT_HANDLEITERATIONDECORATOR_H
