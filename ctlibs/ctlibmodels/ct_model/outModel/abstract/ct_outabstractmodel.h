#ifndef CT_OUTABSTRACTMODEL_H
#define CT_OUTABSTRACTMODEL_H

#include "ct_model/abstract/ct_abstractmodel.h"

class CT_AbstractResult;

/**
 * @brief Represent an output model : An object that was produced by a step.
 *
 * An output model has a unique index. It must be unique for each model produced
 * by a step that was not a copy.
 *
 * Example :
 *
 * Step A produce this output model hierarchy with unique index in parenthesis :
 *
 *  StepA
 *    |---ResultA (0)
 *    |   |--- GroupX (1)
 *    |            |--- Item (2)
 *    |            |--- Item (3)
 *    |--- ResultB (4)
 *           |--- GroupY (5)
 *
 * If a Step B copy the first result and create a new result that will be the unique index produced :
 *
 *  StepB
 *    |---ResultA (copy of ResultA of StepA) (0)
 *    |   |--- GroupX (copy) (1)
 *    |            |--- Item (copy) (2)
 *    |            |--- Item (copy) (3)
 *    |--- ResultC (new result) (6 because last unique index produced was "5" for the GroupY in the StepA)
 *           |--- GroupZ (7)
 *
 * ---------------------------
 *
 * If an output model is copied you can retrieve the model that was used to produce this copy with the
 * method "originalModel()".
 *
 * ---------------------------
 *
 * An output model has a prototype of the item that will be produced by the step.
 *
 * ---------------------------
 *
 * An output model know the result that contains the item that use this model. If a step use a copy of a result from
 * an another step this means that all output models is copied but it will not know the result because only the original
 * model will know it. So when you wan to know which result contains this model with the method "result()" it will
 * go up recursively in the hierarchy of original model to get the original model with the result that was not nullptr.
 */
class CTLIBMODELS_EXPORT CT_OutAbstractModel : public CT_AbstractModel
{
    Q_OBJECT
    typedef CT_AbstractModel    SuperClass;

public:
    typedef std::function<bool(const CT_OutAbstractModel*)> OutChildrenVisitor;
    using UniqueIndexType = int;

    /**
     * @brief Construct an output model that has a unique index (by default at -1 so it was invalid) and a displayable name
     * @param displayableName : a displayable name
     */
    CT_OutAbstractModel(const QString& displayableName);

    /**
     * @brief Copy constructor
     * @warning The parent model and the prototype item will not be copied !
     */
    CT_OutAbstractModel(const CT_OutAbstractModel& other);

    ~CT_OutAbstractModel();

    /**
     * @brief Modify the unique index of this model
     * @param uniqueIndex : the new unique index
     * @warning Not intended for direct use by plugin developper
     */
    void setUniqueIndex(const UniqueIndexType& uniqueIndex);

    /**
     * @brief Returns the unique index of this model or -1 if it was not valid
     */
    UniqueIndexType uniqueIndex() const;

    /**
     * @brief Returns the original model (recursively) if this model is a copy otherwise return 'this'
     */
    CT_OutAbstractModel* recursiveOriginalModel() const;

    /**
     * @brief Returns the original model (not recursively) if this model is a copy otherwise return 'this'
     */
    CT_OutAbstractModel* originalModel() const;

    /**
     * @brief Returns the original model (recursively) with a not nullptr result. If this model
     *        is not a copy and has a result the method return 'this'. If this model
     *        is not a copy and doesn't have a result the method return nullptr.
     */
    CT_OutAbstractModel* recursiveOriginalModelWithAResult() const;

    /**
     * @brief Returns the original model (recursively) with a step not null. If this model
     *        is not a copy and has a step the method return 'this'. If this model
     *        is not a copy and doesn't have a step the method return nullptr.
     */
    CT_OutAbstractModel* recursiveOriginalModelWithAStep() const;

    /**
     * @brief Returns true if at least one element that use this model is visible in at least one document
     */
    bool isVisible() const;

    /**
     * @brief Returns true if at least one element that use this model is visible in the specified document
     */
    bool isVisibleInDocument(const IDocumentForModel* doc) const;

    /**
     * @brief Returns the prototype item that must produce the step
     */
    IPrototypeForModel* prototype() const;

    /**
     * @brief Returns the result where the item that use this model is stocked.
     */
    IResultForModel* result() const;

    /**
     * @brief Returns the step statically cast in the class that you want
     */
    template<class T = CT_AbstractResult>
    T* resultStaticCastT() const { return static_cast<T*>(result()); }

    /**
     * @brief Call it to visit the childrens (output models) of this model and recursively childrens of children's, etc....
     * @param visitor : a lambda expression or a function member that receive a const pointer to a
     *                  CT_OutAbstractModel and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    bool recursiveVisitOutChildrens(const OutChildrenVisitor& visitor) const;

    /**
     * @brief Called to visit childrens (output models) of this model.
     * @param visitor : a lambda expression or a function member that receive a const pointer to a
     *                  CT_OutAbstractModel and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    bool visitOutChildrens(const OutChildrenVisitor& visitor) const;

    /**
     * @brief Used by model to set the original model when a copy is made
     * @warning Not intended for direct use by plugin developper
     */
    void setOriginalModel(const CT_OutAbstractModel* model);

    /**
     * @brief Set the result to this model and his childrens (recursively)
     * @warning Not intended for direct use by plugin developper (Used by CT_AbstractResult)
     */
    void setResult(const IResultForModel* res);

    /**
     * @brief Increment the counter that count the number of element (that use this model ) visible in the specified document
     * @warning Not intended for direct use by plugin developper (Used by IItemDrawableForModel)
     */
    void incrementVisibilityInDocument(const IDocumentForModel* doc);

    /**
     * @brief Decrement the counter that count the number of element (that use this model ) visible in the document passed in parameter
     * @warning Not intended for direct use by plugin developper (Used by IItemDrawableForModel)
     */
    void decrementVisibilityInDocument(const IDocumentForModel* doc);

    /**
     * @brief When all models has been created and added in the hierarchy and no others change will be made you must call this
     *        method.
     * @return Returns false if it was an error.
     * @warning Not intended for direct use by plugin developper (Used by CT_OutResultModelManager)
     */
    bool recursiveFinalize();

    /**
     * @brief Copy this model
     */
    virtual CT_OutAbstractModel* copy() const = 0;

protected:
    /**
     * @brief Used by model to set the prototype
     * @warning Not intended for direct use by plugin developper
     */
    void setPrototype(IPrototypeForModel* prototype);

    /**
     * @brief Called from this class to make some change in
     *        this model to complete it. Do nothing by default.
     * @return true if it was no error.
     */
    virtual bool finalize();

private:
    /**
     * @brief Unique index that must be really unique for each output model in hierarchy of steps. Except when it was copied : it must
     *        be the same than the original model
     */
    UniqueIndexType                     m_uniqueIndex;

    /**
     * @brief The original model is nullptr if this model was not a copied model otherwise it was the model that was used to produce the copy.
     */
    CT_OutAbstractModel*                m_originalModel;

    /**
     * @brief An map that count the number of times this model was added to a specific document. The key is a document, the value is the number
     *        of times this object is present in a document.
     *        When the value go from 0 to 1 or from 1 to 0 the signal "visibilityInDocumentChanged" is emitted.
     */
    QHash<IDocumentForModel*, int>      m_visibleInDocuments;

    /**
     * @brief The result that contains the item that use this model. nullptr if no item that use this model was added
     *        to a result. Can be nullptr or not if it was a copy of an original model
     */
    IResultForModel*                    m_realResult;

    /**
     * @brief A prototype of the item that will be produced by the step. Can be nullptr or not if it was a copy
     *        of an original model
     */
    IPrototypeForModel*                 m_item;

    /**
     * @brief Delete the item from the memory and from this model
     */
    void deleteAndClearItem();

private slots:
    /**
     * @brief Called when the original model is destroyed from memory, so the attribute m_originalModel is set to nullptr
     */
    void originalModelDestroyed();

signals:
    /**
     * @brief Emitted when the a first item that use this model is added to a first document. Or when a last item that use this model is removed from a document
     *        and no other item that use this model was present in a document.
     * @param v : true if the model is present in at least one document, false otherwise
     */
    void visibilityChanged(bool v);

    /**
     * @brief Emitted when the a first item that use this model is added to the document. Or when a last item that use this model is removed from the document.
     * @param doc : the document that receive the item that use this model
     * @param v : true if the model is present in the document, false otherwise
     */
    void visibilityInDocumentChanged(const IDocumentForModel* doc, bool v);
};

#endif // CT_OUTABSTRACTMODEL_H
