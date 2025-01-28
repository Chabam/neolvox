#ifndef CT_OUTMANAGER_H
#define CT_OUTMANAGER_H

#include "ctlibstep_global.h"
#include "ct_model/outModel/manager/ct_outmodelstructuremanager.h"

class CT_VirtualAbstractStep;
class CT_AbstractResult;

/**
 * @brief This class manage outputs (model and results)
 */
class CTLIBSTEP_EXPORT CT_OutManager : public QObject
{
    Q_OBJECT

public:
    enum CreateOutputModelsErrorType {
        CreationOK,
        StepHasChildrens,
        AtLeastOneResultOfStepIsVisibleInDocument
    };

    CT_OutManager();
    ~CT_OutManager();

    /**
     * @brief Returns the output models manager
     */
    CT_OutModelStructureManager* outModelStructureManager() const;

    /**
     * @brief A input result model visitor is a function that returns false it the visit must be stopped, otherwise it
     *        must return true. The function will receive a pointer to a result model.
     */
    using OutResultModelVisitor = std::function<bool(const CT_OutAbstractResultModel*)>;

    /**
     * @brief Visit all output result model
     * @param visitor : a lambda expression per example that return a bool (true if must continue the visit) and receive a const pointer to a CT_OutAbstractResultModel
     * @return True if no results has been visited, otherwise the returned value of the visitor
     */
    bool visitResultModels(const OutResultModelVisitor& visitor) const;

    /**
     * @brief Returns true if we can create output models.
     * @param why : The reason why we can not create output models if this method returns false. Empty otherwise.
     * @return If the step has at least one children or if the step has a result model visible in a document this method returns false. True otherwise.
     */
    bool canCreateOutputModels(const CT_VirtualAbstractStep& step, CreateOutputModelsErrorType* errorType = nullptr, QString* why = nullptr) const;

    /**
     * @brief Call all necessary methods to declare output models.
     * @return If the step has at least one children this method returns false. True otherwise.
     */
    bool createOutputModels(CT_VirtualAbstractStep& step);

    /**
     * @brief Call all necessary methods to create output results.
     * @return Return false if there was already output results, otherwise create it and returns true.
     */
    bool createOutputResults(CT_VirtualAbstractStep& step);

    /**
     * @brief Call all necessary methods to finalize results.
     */
    void finalizeResults();

    /**
     * @brief A input result model visitor is a function that returns false it the visit must be stopped, otherwise it
     *        must return true. The function will receive a pointer to a result model.
     */
    using OutResultVisitor = std::function<bool(const CT_AbstractResult*)>;

    /**
     * @brief Visit all output result
     * @param visitor : a lambda expression per example that return a bool (true if must continue the visit) and receive a const pointer to a CT_AbstractResult
     * @return True if no results has been visited, otherwise the returned value of the visitor
     */
    bool visitResults(const OutResultVisitor& visitor) const;

    /**
     * @brief Returns the number of output result
     */
    int nResult() const;

    /**
     * @brief Returns the output result at the specified index
     */
    const CT_AbstractResult* resultAt(int index) const;

    /**
     * @brief Remove from the collection the output result at the specified index
     *        and return it
     * @warning After that you are responsible of the life in memory of the result !
     */
    CT_AbstractResult* takeResultAt(int index);

private:
    using ResultCollection = QList<CT_AbstractResult*>;

    CT_OutModelStructureManager     m_outModelsStructureManager;
    ResultCollection                m_results;

signals:
    /**
     * @brief This signal is emmited when a result is finalized (no more items will be added to this result after that)
     * @param res : the result that was finalized
     */
    void resultAdded(const CT_AbstractResult* res);
};

#endif // CT_OUTMANAGER_H
