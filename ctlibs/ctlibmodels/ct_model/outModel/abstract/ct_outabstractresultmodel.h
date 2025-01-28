#ifndef CT_OUTABSTRACTRESULTMODEL_H
#define CT_OUTABSTRACTRESULTMODEL_H

#include "ct_model/outModel/abstract/ct_outabstractmodel.h"

/**
 * @brief Represent a model that represent a result
 */
class CTLIBMODELS_EXPORT CT_OutAbstractResultModel : public CT_OutAbstractModel
{
    Q_OBJECT
    typedef CT_OutAbstractModel SuperClass;

public:

    /**
     * @brief Construct an output model that represent a result
     * @param displayableName : a displayable name
     */
    CT_OutAbstractResultModel(const QString& displayableName);

    /**
     * @brief Copy constructor
     * @warning The parent model and the prototype item will not be copied !
     */
    CT_OutAbstractResultModel(const CT_OutAbstractResultModel& other) = default;

    /**
     * @brief Set the result name that will be set when a new result will be created
     */
    void setResultName(const QString& resultName);

    /**
     * @brief Returns the name that will be set to the generated result
     */
    QString resultName() const;

    /**
     * @brief Create the result and return it. The result of this model will be changed to the created result.
     * @return nullptr if it was a problem
     */
    virtual IResultForModel* createResult() = 0;

private:
    /**
     * @brief The result name to set when a new result will be created
     */
    QString     m_resultName;
};

#endif // CT_OUTABSTRACTRESULTMODEL_H
