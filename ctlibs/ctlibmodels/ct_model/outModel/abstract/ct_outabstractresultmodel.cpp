#include "ct_model/outModel/abstract/ct_outabstractresultmodel.h"

CT_OutAbstractResultModel::CT_OutAbstractResultModel(const QString& displayableName) : SuperClass(displayableName),
    m_resultName(displayableName)
{
}

void CT_OutAbstractResultModel::setResultName(const QString& resultName)
{
    m_resultName = resultName;
}

QString CT_OutAbstractResultModel::resultName() const
{
    return m_resultName;
}
