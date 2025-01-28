#ifndef CT_OUTMODELSTRUCTUREMANAGER_H
#define CT_OUTMODELSTRUCTUREMANAGER_H

#include <QSet>

#include "ct_model/outModel/abstract/ct_outabstractresultmodel.h"
#include "ct_model/inModel/abstract/ct_inabstractresultmodel.h"
#include "tools/sfinae.h"

#include <type_traits>
#include <functional>
#include <QDebug>

#define SECOND_NO_OUTPUT_STATIC_ASSERT_MESSAGE "It seems that the second handle does not contains an OUTPUT model. Please verify that you pass an OUTPUT handle !"

#define GENERATE_ADD_XXX_TO(FuncName, FuncNameInputTool, FuncToCall) \
                                        private: \
                                        template<class HandleParent, class HandleOut, typename... ConstructorArgs> \
                                        void FuncName(const HandleParent& inParent, \
                                                      HandleOut& outHandle, \
                                                      std::true_type, \
                                                      ConstructorArgs&& ...constructorArgs) { \
                                            using InResultModelCopyType = typename HandleParent::InResultModelCopyType; \
                                         \
                                            MODELS_ASSERT(inParent.isValid() && !outHandle.isValid()); \
                                            if (!inParent.isValid() || outHandle.isValid()) {qDebug() << "CT_OutModelStructureManager" << ", " <<  "!inParent.isValid() || outHandle.isValid()"; return;} \
                                         \
                                            auto inParentModel = inParent.model(); \
                                         \
                                            auto inResultModelCopy = dynamic_cast<InResultModelCopyType*>(inParentModel->rootModel()); \
                                         \
                                            MODELS_ASSERT(inResultModelCopy != nullptr); \
                                            if (inResultModelCopy == nullptr) {qDebug() << "CT_OutModelStructureManager" << ", " <<  "inResultModelCopy == nullptr"; return;} \
                                         \
                                            auto tool = inResultModelCopy->toolToModifyResultModelCopies(); \
                                         \
                                            MODELS_ASSERT(tool != nullptr); \
                                            if (tool == nullptr) {qDebug() << "CT_OutModelStructureManager" << ", " <<  "tool == nullptr"; return;} \
                                         \
                                            FuncNameInputTool(inParentModel, tool, outHandle, std::forward<ConstructorArgs>(constructorArgs)...); \
                                        } \
                                          \
                                        public: \
                                        template<typename InParentModelT, typename ToolToModifyResultModelCopiesT, class HandleOut, typename... ConstructorArgs>  \
                                        void FuncNameInputTool(InParentModelT* inParentModel, \
                                                               ToolToModifyResultModelCopiesT* tool, \
                                                               HandleOut& outHandle, \
                                                               ConstructorArgs&& ...constructorArgs) { \
                                                auto outModelPrototypeToCopy = new typename HandleOut::ModelType(std::forward<ConstructorArgs>(constructorArgs)...); \
                                             \
                                                typename HandleOut::ModelsCollectionType allModelsCreated; \
                                             \
                                                tool->FuncToCall(inParentModel, outModelPrototypeToCopy, allModelsCreated); \
                                             \
                                                outHandle.setModels(allModelsCreated); \
                                             \
                                                delete outModelPrototypeToCopy; \
                                        } \
                                          \
                                        private: \
                                        template<class HandleParent, class HandleOut, typename... ConstructorArgs> \
                                        void FuncName(const HandleParent& outParent, \
                                                      HandleOut& outHandle, \
                                                      std::false_type, \
                                                      ConstructorArgs&& ...constructorArgs) { \
                                     \
                                            MODELS_ASSERT(m_ignoreInvalidParentHandle || (outParent.isValid() && !outHandle.isValid())); \
                                            if (!m_ignoreInvalidParentHandle && (!outParent.isValid() || outHandle.isValid())) {qDebug() << "CT_OutModelStructureManager" << ", " <<  "!m_ignoreInvalidParentHandle && (!outParent.isValid() || outHandle.isValid())"; return;} \
                                     \
                                            auto outModelPrototypeToCopy = new typename HandleOut::ModelType(std::forward<ConstructorArgs>(constructorArgs)...); \
                                     \
                                            typename HandleOut::ModelsCollectionType allModelsCreated; \
                                     \
                                            const auto visitor = [&outModelPrototypeToCopy, &allModelsCreated](const typename HandleParent::ModelType* model) -> bool { \
                                                auto copy = static_cast<typename HandleOut::ModelType*>(outModelPrototypeToCopy->copy()); \
                                     \
                                                const_cast<typename HandleParent::ModelType*>(model)->FuncToCall(copy); \
                                     \
                                                allModelsCreated.append(copy); \
                                     \
                                                return true; \
                                            }; \
                                     \
                                            outParent.visitModels(visitor); \
                                            outHandle.setModels(allModelsCreated); \
                                     \
                                            delete outModelPrototypeToCopy; \
                                        }

/**
 * @brief Class that you must use to create the structure (tree) of output models and assign each
 *        created model to an handle
 */
class CTLIBMODELS_EXPORT CT_OutModelStructureManager
{
public:
    CT_OutModelStructureManager() : m_ignoreInvalidParentHandle(false) {}
    virtual ~CT_OutModelStructureManager();

    /**
     * @brief Call it if you want ot ignore invalid parent handle (parent handle that does not have at least one model)
     */
    void setIgnoreInvalidParentHandle(bool ignore);

    /**
     * @brief Add a new output result model to the structure (result are always root)
     * @param handleResult : the handle to use to create the result model and access it later
     */
    template<class HandleOutResult>
    void addResult(HandleOutResult& handleResult,
                   const QString& resultName = QString{"Result"},
                   const QString& displayableName = QString{"Result"},
                   const QString& shortDescription = QString{""}) {
        // check, at compilation time, if the handle contains an output model
        internalAddResult(handleResult,
                          std::integral_constant<bool, IsAnOutputModel<typename HandleOutResult::ModelType>::value>(),
                          resultName,
                          displayableName,
                          shortDescription);
    }

    /**
     * @brief Add one or multiple new output result model to the structure according to the number of possibility that
     *        the input result model (in the input result handle) has found. Because you can set that the input result
     *        model can have multiple possibility, if you want to create a copy of the output result model you will not
     *        one but multiple output result model. So each output result model (copy) is added to this manager.
     * @param handleResult : the handle to use to create the result model and access it later
     */
    template<class HandleInResultCopy>
    void addResultCopy(HandleInResultCopy& handleInResultCopy) {
        // check, at compilation time, if the handle contains an input result model that produce copies
        internalAddResultCopy<HandleInResultCopy>(handleInResultCopy);
    }

    /**
     * @brief Set a specific root group to the result model
     * @param handleResult : the handle of an input result model or an output result model to use to set the zero or more root group
     * @param rootGroupHandle : the handle of the output group model to use to create the new group model and access it later
     * @param displayableName : the displayable that must be set to the new group model
     * @param shortDescription : the short description that must be set to the new group model
     * @param detailledDescription : the detailled description that must be set to the new group model
     */
    template<class HandleResult, class HandleOutGroup>
    void setRootGroup(const HandleResult& handleResult,
                      HandleOutGroup& rootGroupHandle,
                      const QString& displayableName = QObject::tr("Root Group"),
                      const QString& shortDescription = QString{""},
                      const QString& detailledDescription = QString{""}) {

        static_assert(IsAnOutputModel<typename HandleOutGroup::ModelType>::value, SECOND_NO_OUTPUT_STATIC_ASSERT_MESSAGE);

        internalSetRootGroupTo(handleResult,
                               rootGroupHandle,
                               std::integral_constant<bool, IsAnInputModel<typename HandleResult::ModelType>::value>(),
                               displayableName,
                               shortDescription,
                               detailledDescription);
    }

    /**
     * @brief Add a group to another group model
     * @param parentGroup : the handle of an input or an output group model to use to add the new group
     * @param groupHandle : the handle of the output group model to use to create the new group model and access it later
     * @param displayableName : the displayable that must be set to the new group model
     * @param shortDescription : the short description that must be set to the new group model
     * @param detailledDescription : the detailled description that must be set to the new group model
     */
    template<class HandleGroupParent, class HandleOutGroup>
    void addGroup(const HandleGroupParent& parentGroup,
                  HandleOutGroup& groupHandle,
                  const QString& displayableName = QObject::tr("Group"),
                  const QString& shortDescription = QString{""},
                  const QString& detailledDescription = QString{""},
                  typename HandleOutGroup::GroupType* prototype = nullptr) {

        static_assert(IsAnOutputModel<typename HandleOutGroup::ModelType>::value, SECOND_NO_OUTPUT_STATIC_ASSERT_MESSAGE);

        internalAddGroupTo(parentGroup,
                           groupHandle,
                           std::integral_constant<bool, IsAnInputModel<typename HandleGroupParent::ModelType>::value>(),
                           displayableName,
                           shortDescription,
                           detailledDescription,
                           prototype);
    }

    template<class HandleResult, class HandleInGroup>
    void removeGroup(const HandleResult& handleResult,
                     const HandleInGroup& inGroupHandle) {
        auto tool = handleResult.model()->toolToModifyResultModelCopies();
        tool->removeGroup(inGroupHandle.model());
    }

    /**
     * @brief Add an item to a group model
     * @param parentGroup : the handle of an input or an output group model to use to add the new group
     * @param itemHandle : the handle of the output item model to use to create the new item model and access it later
     * @param displayableName : the displayable that must be set to the new item model
     * @param shortDescription : the short description that must be set to the new item model
     * @param detailledDescription : the detailled description that must be set to the new item model
     */
    template<class HandleGroupParent, class HandleOutItem>
    void addItem(const HandleGroupParent& parentGroup,
                 HandleOutItem& itemHandle,
                 const QString& displayableName = QObject::tr("Item"),
                 const QString& shortDescription = QString{""},
                 const QString& detailledDescription = QString{""},
                 typename HandleOutItem::ItemType* prototype = nullptr) {

        static_assert(IsAnOutputModel<typename HandleOutItem::ModelType>::value, SECOND_NO_OUTPUT_STATIC_ASSERT_MESSAGE);

        internalAddItem(parentGroup,
                        itemHandle,
                        displayableName,
                        shortDescription,
                        detailledDescription,
                        prototype);

    }

    template<class HandleResult, class HandleInItem>
    void removeItem(const HandleResult& handleResult,
                    const HandleInItem& inItemHandle) {
        auto tool = handleResult.model()->toolToModifyResultModelCopies();
        tool->removeItem(inItemHandle.model());
    }

    /**
     * @brief Add an item attribute to an item model
     * @param parentItem : the handle of an input or an output item model to use to add the new item attribute
     * @param itemAttributeHandle : the handle of the output item attribute model to use to create the new item attribute model and access it later
     * @param category : a category, per example CT_AbstractCategory::DATA_VALUE
     * @param displayableName : the displayable that must be set to the new item attribute model
     * @param shortDescription : the short description that must be set to the new item attribute model
     * @param detailledDescription : the detailled description that must be set to the new item attribute model
     */
    template<class HandleItemParent, class HandleOutItemAttribute, typename Category>
    void addItemAttribute(const HandleItemParent& parentItem,
                          HandleOutItemAttribute& itemAttributeHandle,
                          const Category& category,
                          const QString& displayableName = QObject::tr("Item Attribute"),
                          const QString& shortDescription = QString{""},
                          const QString& detailledDescription = QString{""},
                          typename HandleOutItemAttribute::ItemAttributeType* prototype = nullptr) {

        static_assert(IsAnOutputModel<typename HandleOutItemAttribute::ModelType>::value, SECOND_NO_OUTPUT_STATIC_ASSERT_MESSAGE);

        internalAddItemAttributeTo(parentItem,
                                   itemAttributeHandle,
                                   std::integral_constant<bool, IsAnInputModel<typename HandleItemParent::ModelType>::value>(),
                                   category,
                                   displayableName,
                                   shortDescription,
                                   detailledDescription,
                                   prototype);
    }

    /**
     * @brief Add a point attribute to a group model
     * @param parentGroup : the handle of an input or an output group model to use to add the new group
     * @param itemHandle : the handle of the output item model to use to create the new item model and access it later
     * @param displayableName : the displayable that must be set to the new item model
     * @param shortDescription : the short description that must be set to the new item model
     * @param detailledDescription : the detailled description that must be set to the new item model
     */
    template<class HandleGroupParent, class HandleOutPointAttribute>
    void addPointAttribute(const HandleGroupParent& parentGroup,
                           HandleOutPointAttribute& itemHandle,
                           const QString& displayableName = QObject::tr("Item"),
                           const QString& shortDescription = QString{""},
                           const QString& detailledDescription = QString{""},
                           typename HandleOutPointAttribute::ItemType* prototype = nullptr) {

        static_assert(SFINAE_And_<IsAnOutputModel<typename HandleOutPointAttribute::ModelType>, HasApplicableToPoint<HandleOutPointAttribute>>::value, SECOND_NO_OUTPUT_STATIC_ASSERT_MESSAGE);

        internalAddItem(parentGroup,
                        itemHandle,
                        displayableName,
                        shortDescription,
                        detailledDescription,
                        prototype);
    }

    /**
     * @brief Add an edge attribute to a group model
     * @param parentGroup : the handle of an input or an output group model to use to add the new group
     * @param itemHandle : the handle of the output item model to use to create the new item model and access it later
     * @param displayableName : the displayable that must be set to the new item model
     * @param shortDescription : the short description that must be set to the new item model
     * @param detailledDescription : the detailled description that must be set to the new item model
     */
    template<class HandleGroupParent, class HandleOutEdgeAttribute>
    void addEdgeAttribute(const HandleGroupParent& parentGroup,
                          HandleOutEdgeAttribute& itemHandle,
                          const QString& displayableName = QObject::tr("Item"),
                          const QString& shortDescription = QString{""},
                          const QString& detailledDescription = QString{""},
                          typename HandleOutEdgeAttribute::ItemType* prototype = nullptr) {

        static_assert(SFINAE_And_<IsAnOutputModel<typename HandleOutEdgeAttribute::ModelType>, HasApplicableToEdge<HandleOutEdgeAttribute>>::value, SECOND_NO_OUTPUT_STATIC_ASSERT_MESSAGE);

        internalAddItem(parentGroup,
                        itemHandle,
                        displayableName,
                        shortDescription,
                        detailledDescription,
                        prototype);
    }

    /**
     * @brief Add a face attribute to a group model
     * @param parentGroup : the handle of an input or an output group model to use to add the new group
     * @param itemHandle : the handle of the output item model to use to create the new item model and access it later
     * @param displayableName : the displayable that must be set to the new item model
     * @param shortDescription : the short description that must be set to the new item model
     * @param detailledDescription : the detailled description that must be set to the new item model
     */
    template<class HandleGroupParent, class HandleOutFaceAttribute>
    void addFaceAttribute(const HandleGroupParent& parentGroup,
                          HandleOutFaceAttribute& itemHandle,
                          const QString& displayableName = QObject::tr("Item"),
                          const QString& shortDescription = QString{""},
                          const QString& detailledDescription = QString{""},
                          typename HandleOutFaceAttribute::ItemType* prototype = nullptr) {

        static_assert(SFINAE_And_<IsAnOutputModel<typename HandleOutFaceAttribute::ModelType>, HasApplicableToFace<HandleOutFaceAttribute>>::value, SECOND_NO_OUTPUT_STATIC_ASSERT_MESSAGE);

        internalAddItem(parentGroup,
                        itemHandle,
                        displayableName,
                        shortDescription,
                        detailledDescription,
                        prototype);
    }

    /**
     * @brief Call this method to recursively call the method "finalize" and
     *        assign a unique index to all models in this manager
     * @param uniqueIndexGenerator : a function or a lambda expresion or an object that can generate a new unique index
     *                               each time we call the operator ().
     */
    template<typename UniqueIndexGenerator>
    void finalize(UniqueIndexGenerator& uniqueIndexGenerator) {
        const auto visitor = [&uniqueIndexGenerator](const CT_OutAbstractModel* model) -> bool {

            if((model->uniqueIndex() <= 0) || (model->originalModel() == model))
                const_cast<CT_OutAbstractModel*>(model)->setUniqueIndex(uniqueIndexGenerator());

            return true;
        };

        for(OutModelInfo* resultModelInfo : m_results) {
            if((resultModelInfo->model->uniqueIndex() <= 0) || (resultModelInfo->model->originalModel() == resultModelInfo->model))
                resultModelInfo->model->setUniqueIndex(uniqueIndexGenerator());

            resultModelInfo->model->recursiveFinalize();
            resultModelInfo->model->recursiveVisitOutChildrens(visitor);
        }
    }

    /**
     * @brief Visit all output result model added to this manager
     * @param visitor : a lambda expression per example that return a bool (true if must continue the visit) and receive a const pointer to a CT_InAbstractResultModel
     * @return True if no results has been visited, otherwise the returned value of the visitor
     */
    template<typename Visitor>
    bool visitResults(const Visitor& visitor) const {
        for(const OutModelInfo* resultModelInfo : m_results) {
            if(!visitor(resultModelInfo->model))
                return false;
        }

        return true;
    }

    /**
     * @brief Returns the number of results
     */
    int nResults() const;

    /**
     * @brief Clear output result model from this manager and from the memory
     * @param deleteFromMemory : set it to true if you want to remove from collection and delete from memory output result models. If
     *                           you set it to false it will only be removed from the collection.
     */
    void clearResults(bool deleteFromMemory = true);

protected:
    /**
     * @brief A structure to keep the output result model and a boolean to know if it must be deleted
     *        from memory by this class or not
     */
    struct OutModelInfo {
        OutModelInfo() : model(nullptr), mustBeAutoDelete(true) {}
        OutModelInfo(CT_OutAbstractResultModel* m, bool autoDeleteIt) : model(m), mustBeAutoDelete(autoDeleteIt) {}

        ~OutModelInfo() {
            if(mustBeAutoDelete)
                delete model;
        }

        CT_OutAbstractResultModel*  model;
        bool                        mustBeAutoDelete;
    };

    void internalAddResultModel(CT_OutAbstractResultModel* resultModel, bool autoDeleteIt = true)
    {
        m_results.append(new OutModelInfo(resultModel, autoDeleteIt));
    }

    template<typename ResultCopyModel>
    void internalAddResultCopyModel(ResultCopyModel* inModel)
    {
        MODELS_ASSERT(!m_inResultsCopy.contains(inModel) || (inModel->toolToModifyResultModelCopies() == nullptr));
        if (m_inResultsCopy.contains(inModel) && (inModel->toolToModifyResultModelCopies() != nullptr)) {qDebug() << "CT_OutModelStructureManager::internalAddResultCopyModel" << ", " <<  "m_inResultsCopy.contains(inModel) && (inModel->toolToModifyResultModelCopies() != nullptr)"; return;}

        m_inResultsCopy.insert(inModel);

        const auto tool = inModel->createOutResultModelCopiesAccordingToNumberOfPossibility();

        const auto visitor = [this](const CT_OutAbstractResultModel* outResultModel) -> bool {
            internalAddResultModel(const_cast<CT_OutAbstractResultModel*>(outResultModel), false);
            return true;
        };

        tool->visitOutResultModelCopies(visitor);
    }

private:
    /**
     * @brief Collection of output result model
     */
    QList<OutModelInfo*>                m_results;
    QSet<CT_InAbstractResultModel*>     m_inResultsCopy;
    bool                                m_ignoreInvalidParentHandle;

    GENERATE_ADD_XXX_TO(internalAddGroupTo, addGroupWithInputTool, addGroup)
    GENERATE_ADD_XXX_TO(internalAddItemTo, addItemWithInputTool, addItem)
    GENERATE_ADD_XXX_TO(internalAddItemAttributeTo, addItemAttributeWithInputTool, addItemAttribute)

    /*****************************************************/

    template<class HandleOutResult, typename... ConstructorArgs>
    void internalAddResult(HandleOutResult& handleResult,
                           std::true_type, // is an output model
                           ConstructorArgs&& ...constructorArgs) {
        MODELS_ASSERT(!handleResult.isValid());
        if (handleResult.isValid()) {qDebug() << "CT_OutModelStructureManager::internalAddResult" << ", " <<  "handleResult.isValid()"; return;}

        // nullptr = no root group model to pass to the output result model
        auto resultModel = new typename HandleOutResult::ModelType(nullptr, std::forward<ConstructorArgs>(constructorArgs)...);
        handleResult.setModel(resultModel);

        internalAddResultModel(resultModel, true);
    }

    template<class HandleInResult, typename... ConstructorArgs>
    void internalAddResult(HandleInResult& handleResult,
                           std::false_type, // is an input model
                           ConstructorArgs&& ...) {
        // TODO : MK 16.11.18 ==> static_warning(message) !!! (doesn't exist yet)
        // "You have called the method addResult(...) with an INPUT COPY handle. Arguments like \"resultName\", etc... will be ignored but the result will be added as a input copy. Prefer call the method \"addResultCopy(...)\"."
        internalAddResultCopy(handleResult);
    }

    /*****************************************************/

    template<class HandleInResultCopy>
    void internalAddResultCopy(HandleInResultCopy& handleInResultCopy) {

        static_assert(IsAResultModelCopy<typename HandleInResultCopy::ModelType>::value, "It seems that the specified handle doesn't contains a model that was an input result model COPY.");

        MODELS_ASSERT(handleInResultCopy.isValid());
        if (!handleInResultCopy.isValid()) {qDebug() << "CT_OutModelStructureManager::internalAddResultCopy" << ", " <<  "It seems that the specified handle doesn't contains a model that was an input result model COPY."; return;}

        internalAddResultCopyModel(handleInResultCopy.model());
    }

    /*****************************************************/

    // setRootGroup to an input result copy
    template<class HandleResult, class HandleOutGroup, typename... ConstructorArgs>
    void internalSetRootGroupToCopy(const HandleResult& handleResult,
                                    HandleOutGroup& rootGroupHandle,
                                    ConstructorArgs&& ...constructorArgs) {
        static_assert(IsAResultModelCopy<typename HandleResult::ModelType>::value, "It seems that the specified handle doesn't contains a model that was an input result model COPY. Please verify that you pass an INPUT COPY handle !");

        MODELS_ASSERT((handleResult.model() != nullptr) && !rootGroupHandle.isValid());
        if ((handleResult.model() == nullptr) || rootGroupHandle.isValid()) {qDebug() << "CT_OutModelStructureManager::internalSetRootGroupToCopy" << ", " <<  "(handleResult.model() == nullptr) || rootGroupHandle.isValid()"; return;}

        auto tool = handleResult.model()->toolToModifyResultModelCopies();

        MODELS_ASSERT(tool != nullptr);
        if (tool == nullptr) {qDebug() << "CT_OutModelStructureManager::internalSetRootGroupToCopy" << ", " <<  "tool == nullptr"; return;}

        auto rootGroupModelPrototypeToCopy = new typename HandleOutGroup::ModelType(std::forward<ConstructorArgs>(constructorArgs)...);

        typename HandleOutGroup::ModelsCollectionType allModelsCreated;

        tool->setRootGroup(rootGroupModelPrototypeToCopy, allModelsCreated);
        rootGroupHandle.setModels(allModelsCreated);

        delete rootGroupModelPrototypeToCopy;
    }

    template<class HandleResult, class HandleOutGroup, typename... ConstructorArgs>
    void internalSetRootGroupTo(const HandleResult& handleResult,
                                HandleOutGroup& rootGroupHandle,
                                std::true_type,
                                ConstructorArgs&& ...constructorArgs) {
        internalSetRootGroupToCopy(handleResult,
                                   rootGroupHandle,
                                   std::forward<ConstructorArgs>(constructorArgs)...);
    }

    // setRootGroup to an output result
    template<class HandleResult, class HandleOutGroup, typename... ConstructorArgs>
    void internalSetRootGroupTo(const HandleResult& handleResult,
                                HandleOutGroup& rootGroupHandle,
                                std::false_type,
                                ConstructorArgs&& ...constructorArgs) {

        MODELS_ASSERT((handleResult.model() != nullptr)
                 && (handleResult.model()->rootGroup() == nullptr)
                 && !rootGroupHandle.isValid());
        if ((handleResult.model() == nullptr) || (handleResult.model()->rootGroup() != nullptr) || rootGroupHandle.isValid()) {qDebug() << "CT_OutModelStructureManager::internalSetRootGroupTo" << ", " <<  "(handleResult.model() == nullptr) || (handleResult.model()->rootGroup() != nullptr) || rootGroupHandle.isValid()"; return;}

        auto rootGroupModel = new typename HandleOutGroup::ModelType(std::forward<ConstructorArgs>(constructorArgs)...);

        handleResult.model()->setRootGroup(rootGroupModel);
        rootGroupHandle.setModels(rootGroupModel);
    }

    /*****************************************************/

    template<class HandleGroupParent, class HandleOutItem, typename... ConstructorArgs>
    void internalAddItem(const HandleGroupParent& parentGroup,
                         HandleOutItem& itemHandle,
                         ConstructorArgs&& ...constructorArgs) {

        internalAddItemTo(parentGroup,
                          itemHandle,
                          std::integral_constant<bool, IsAnInputModel<typename HandleGroupParent::ModelType>::value>(),
                          std::forward<ConstructorArgs>(constructorArgs)...);
    }
};

#endif // CT_OUTMODELSTRUCTUREMANAGER_H
