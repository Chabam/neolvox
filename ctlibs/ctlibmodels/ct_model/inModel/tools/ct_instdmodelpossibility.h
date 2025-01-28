#ifndef CT_INSTDMODELPOSSIBILITY_H
#define CT_INSTDMODELPOSSIBILITY_H

#include "ctlibmodels_global.h"

#include <QObject>

class CT_OutAbstractModel;
class CT_InStdModelPossibilitySelectionGroup;

/**
 * @brief Represent a possibility for a input model. When an input model search an output model that match
 *        he can find a lot of output model => a lot of POSSIBILITY.
 *
 *        You must use this class with the class CT_InStdModelPossibilityGroup
 */
class CTLIBMODELS_EXPORT CT_InStdModelPossibility : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Create a possibility without a output model or a selection group.
     */
    CT_InStdModelPossibility();

    /**
     * @brief Copy constructor. The output model is not copied but the pointer is keeped and "mustAutoDeleteOutModel" will return false.
     */
    CT_InStdModelPossibility(const CT_InStdModelPossibility& other);

    virtual ~CT_InStdModelPossibility();

    /**
     * @brief Define the output model for this possibility
     */
    void setOutModel(const CT_OutAbstractModel& model);

    /**
     * @brief Set to true if you want that the output model of this posibility must be deleted from memory when this possibility is deleted.
     */
    void setAutoDeleteOutModel(bool e);

    /**
     * @brief Returns true if the output model of this posibility must be deleted from memory when this possibility is deleted. false by default.
     */
    bool mustAutoDeleteOutModel() const;

    /**
     * @brief Returns the output model of this possibility
     */
    CT_OutAbstractModel* outModel() const;

    /**
     * @brief Returns if this possibility is selected
     */
    bool isSelected() const;

    /**
     * @brief Define the selection group that manage the selection of this possibility
     */
    void setSelectionGroup(const CT_InStdModelPossibilitySelectionGroup* selectionGroup);

    /**
     * @brief Returns the selection group that manage the selection of this possibility
     */
    CT_InStdModelPossibilitySelectionGroup* selectionGroup() const;

    /**
     * @brief Copy this possibility. The output model is not copied but the pointer is keeped and "mustAutoDeleteOutModel" will return false.
     */
    virtual CT_InStdModelPossibility* copy() const;

public slots:
    /**
     * @brief Select/DeSelect this possibility. You cannot select a possibility if it was not in a group.
     * @return Returns false if you cannot modify this possibility
     */
    virtual bool setSelected(bool check);

signals:
    /**
     * @brief Selected state changed
     */
    void selectStateChange(bool check);

private:
    CT_OutAbstractModel*                        m_outModel;
    CT_InStdModelPossibilitySelectionGroup*     m_selectionGroup;
    bool                                        m_autoDeleteOutModel;

protected:
    /**
     * @brief Delete or not from memory the output model according to the variable "m_autoDeleteOutModel"
     */
    void clearOutModel();
};

#endif // CT_INSTDMODELPOSSIBILITY_H
