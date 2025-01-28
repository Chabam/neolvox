#ifndef CT_ABSTRACTMODEL_H
#define CT_ABSTRACTMODEL_H

#include "ctlibmodels_global.h"

#include "tools/modelsassert.h"
#include "ct_model/interfacesformodels.h"

#include <QString>
#include <QList>

class CT_VirtualAbstractStep;
/**
 * @brief Models are useful for describing something without creating it. A model can
 *        have zero or multiple childrens so the structure is a tree. A model can have
 *        a short description, a detailled description and a displayable name.
 *
 *        In the case of this application you have steps that wants something in input and
 *        produce outputs. And we want to describe this before execute steps.
 *
 *        So a model can :
 *          - describe an input object that is necessary for the step
 *          - or an output object that is produced by the step.
 *
 *        Input models can search compatible output models and add it into a collection of
 *        possibility.
 *
 *        So for example if each input model of the step B has found at least one possibility (one
 *        output model) in the step A, it means that step B can be added after the step A because
 *        it was compatible.
 *
 *        A CT_AbstractModel is the base class of input or output models. But with this base class
 *        you can not know if it was an output or an input.
 *
 *        A model know which step has created it.
 */
class CTLIBMODELS_EXPORT CT_AbstractModel : public QObject
{
    Q_OBJECT

public:
    typedef std::function<bool(const CT_AbstractModel*)> ChildrenVisitor;

    /**
     * @brief Construct a model with a displayable name
     * @param displayableName : a displayable name
     */
    CT_AbstractModel(const QString& displayableName);

    /**
     * @brief Copy constructor.
     * @warning The parent model will not be copied !
     */
    CT_AbstractModel(const CT_AbstractModel& other);

    virtual ~CT_AbstractModel();

    /**
     * @brief Returns the short description of the model
     */
    QString shortDescription() const;

    /**
     * @brief Returns the detailled description of the model
     */
    QString detailledDescription() const;

    /**
     * @brief Returns the displayable name of the model
     */
    QString displayableName() const;

    /**
     * @brief Set the displayable name
     */
    void setDisplayableName(const QString& displayableName);

    /**
     * @brief Set the short description
     */
    void setShortDescription(const QString& shortDescription);

    /**
     * @brief Set the detailled description
     */
    void setDetailledDescription(const QString& fullDescription);

    /**
     * @brief Call it to visit the childrens of this model and recursively childrens of children's, etc....
     * @param visitor : a lambda expression or a function member that receive a const pointer to a
     *                  CT_AbstractModel and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    bool recursiveVisitChildrens(const ChildrenVisitor& visitor) const;

    /**
     * @brief Call it to visit the childrens of this model.
     *        Because nature of childrens can be different this class doesn't contains childrens
     *        so the visit is deleguate to subclass.
     * @param visitor : a lambda expression or a function member that receive a const pointer to a
     *                  CT_AbstractModel and return a bool (true to continue the visit, false to stop the visit)
     * @return Returns false if the visitor has stopped the visit
     */
    virtual bool visitChildrens(const ChildrenVisitor& visitor) const = 0;

    /**
     * @brief Returns true if the model don't have childrens
     *        Because nature of childrens can be different this class doesn't contains childrens
     *        so this method is deleguate to subclass.
     */
    virtual bool isEmpty() const = 0;

    /**
     * @brief Returns true if the specified model exist in the hierarchy (begin from the root model and go down recursively)
     */
    bool existInTree(const CT_AbstractModel* model) const;

    /**
     * @brief Returns the parent model of this model
     */
    CT_AbstractModel* parentModel() const;

    /**
     * @brief Returns the root model of the hierarchy.
     *        Go up recursively in the structure and stop when the method "parentModel()" return nullptr.
     */
    CT_AbstractModel* rootModel() const;

    /**
     * @brief Returns the step which has created it
     */
    IStepForModel* step() const;

    /**
     * @brief Returns the step statically cast in the class that you want
     */
    template<class T = CT_VirtualAbstractStep>
    T* stepStaticCastT() const { return static_cast<T*>(step()); }

    /**
     * @brief Set the parent model of this model
     * @warning Not intended for direct use by plugin developper
     */
    void setParentModel(const CT_AbstractModel* parent);

    /**
     * @brief Set the step that create this model to this model and to childrens recursively
     * @warning Not intended for direct use by plugin developper
     */
    void setStep(const IStepForModel* step);

private:
    /**
     * @brief The displayable name used in GUI
     */
    QString                 m_displayableName;

    /**
     * @brief The short description used in GUI
     */
    QString                 m_shortDescription;

    /**
     * @brief The full description used in GUI
     */
    QString                 m_fullDescription;

    /**
     * @brief The parent model (nullptr if it was the root of the hierarchy)
     */
    CT_AbstractModel*       m_parentModel;

    /**
     * @brief The step which has created this model (nullptr if this model was not created in a step)
     */
    IStepForModel*          m_step;
};

#endif // CT_ABSTRACTMODEL_H
