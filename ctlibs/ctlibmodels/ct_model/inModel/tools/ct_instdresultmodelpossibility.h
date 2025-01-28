#ifndef CT_INSTDRESULTMODELPOSSIBILITY_H
#define CT_INSTDRESULTMODELPOSSIBILITY_H

#include "ct_model/inModel/tools/ct_instdmodelpossibility.h"

class CT_InAbstractResultModel;
class CT_OutAbstractResultModel;

/**
 * @brief Represent a possibility for an input result model.
 *
 * Each CT_InStdResultModelPossibility contains an input model that was a copy of the original
 * result model but with one possibility (one output model). And childrens of the input result
 * model can have many possibilities.
 */
class CTLIBMODELS_EXPORT CT_InStdResultModelPossibility : public CT_InStdModelPossibility
{
    Q_OBJECT
    typedef CT_InStdModelPossibility SuperClass;

public:
    CT_InStdResultModelPossibility();

    /**
     * @brief Copy constructor. The output model is not copied but the pointer is keeped
     *        and "mustAutoDeleteOutModel" will return false. The input result model
     *        is copied.
     */
    CT_InStdResultModelPossibility(const CT_InStdResultModelPossibility& other);
    ~CT_InStdResultModelPossibility();

    /**
     * @brief Set the input result model. It must be copy of an original result model and must have only
     *        one possibility !. The life in memory of this model is managed by this class so it will be
     *        automatically deleted when it was replaced or in the destructor of this class.
     * @param model : the new model to use
     * @warning There is no verification of the number of possibility or if it was really a copy.
     */
    void setInResultModel(CT_InAbstractResultModel* model);

    /**
     * @brief Returns the input result model. It was a copy of an original input result model and has
     *        only one possibility
     */
    CT_InAbstractResultModel* inResultModel() const;

    /**
     * @brief Set if we must auto delete the input result model when we change it or when the destructor is called
     * @param autoDeleteIt
     */
    void setAutoDeleteInResultModel(bool autoDeleteIt);

    /**
     * @brief Returns true if we must auto delete the input result model when we change it or when the destructor is called
     */
    bool mustAutoDeleteInResultModel() const;

    /**
     * @brief Returns the output result model of this possibility
     */
    CT_OutAbstractResultModel* outResultModel() const;

    /**
     * @brief Copy this possibility. The output model is not copied but the pointer is keeped
     *        and "mustAutoDeleteOutModel" will return false. The input result model
     *        and his childrens is copied with possibilities and theirs state (selected or not).
     */
    CT_InStdModelPossibility* copy() const override;

public slots:
    /**
     * @brief Select/DeSelect this possibility. You cannot select a possibility if it was not in a group.
     * @return Returns false if you cannot modify this possibility
     */
    bool setSelected(bool check) override;

private:
    /**
     * @brief The input result model
     */
    CT_InAbstractResultModel*   m_inModel;

    /**
     * @brief True if must delete the in model in destructor or when we change it. true by default
     */
    bool                        m_autoDeleteInResultModel;
};

#endif // CT_INSTDRESULTMODELPOSSIBILITY_H
