#include "ct_inresultmodelgrouptocopy.h"

#include "ct_model/inModel/tools/ct_instdresultmodelpossibility.h"
#include "ct_model/inModel/tools/ct_instdmodelpossibilityselectiongroup.h"
#include "ct_model/outModel/abstract/ct_outabstractmodel.h"
#include "ct_model/outModel/ct_outresultmodelgroupcopy.h"

CT_InResultModelGroupToCopy::CT_InResultModelGroupToCopy(const QString& displayableName,
                                                         const QString& shortDescription,
                                                         bool recursive,
                                                         quint8 minimumNumberOfPossibilityToSelect,
                                                         int maximumNumberOfPossibilityThatCanBeSelected) : SuperClass(displayableName,
                                                                                                                       shortDescription,
                                                                                                                       recursive,
                                                                                                                       minimumNumberOfPossibilityToSelect,
                                                                                                                       maximumNumberOfPossibilityThatCanBeSelected),
    m_tool(nullptr)
{
}

CT_InResultModelGroupToCopy::CT_InResultModelGroupToCopy(const CT_InResultModelGroupToCopy& other,
                                                         const bool& withPossibilities) : SuperClass(other,
                                                                                                     withPossibilities),
    m_tool(nullptr)
{
}

CT_InResultModelGroupToCopy::~CT_InResultModelGroupToCopy()
{
    deleteToolToModifyResultModelCopies();
}

CT_InResultModelGroupToCopy::ToolToModifyResultModelCopiesType* CT_InResultModelGroupToCopy::createOutResultModelCopiesAccordingToNumberOfPossibility()
{
    delete m_tool;
    m_tool = new ToolToModifyResultModelCopiesType();

    const auto visitor = [this](const CT_InStdModelPossibility* possibility) -> bool {

        MODELS_ASSERT(dynamic_cast<const CT_InStdResultModelPossibility*>(possibility) != nullptr);
        if (dynamic_cast<const CT_InStdResultModelPossibility*>(possibility) == nullptr) {qDebug() << "CT_InResultModelGroupToCopy::createOutResultModelCopiesAccordingToNumberOfPossibility" << ", " <<  "dynamic_cast<const CT_InStdResultModelPossibility*>(possibility) == nullptr"; return false;}

        // the possibility is always a possibility for a result model
        const CT_InStdResultModelPossibility* resultPossibility = static_cast<const CT_InStdResultModelPossibility*>(possibility);

        // get the input result model (it was a copy of this input result model but with childrens that has possibilities)
        // and has only one possibility : resultPossibility (it was linked together)
        const CT_InResultModelGroupToCopy* inResultModel = static_cast<CT_InResultModelGroupToCopy*>(resultPossibility->inResultModel());

        // create a output result model that will be a copy of the specified input result model
        CT_OutResultModelGroupCopy* outResultCopy = new CT_OutResultModelGroupCopy(inResultModel);

        // add it to the tool object
        m_tool->addResulModel(outResultCopy);

        return true;
    };

    possibilitiesGroup()->visitSelectedPossibilities(visitor);

    return m_tool;
}

CT_InAbstractModel* CT_InResultModelGroupToCopy::copy(bool withPossibilities) const
{
    return new CT_InResultModelGroupToCopy(*this, withPossibilities);
}

int CT_InResultModelGroupToCopy::nOutResultModelCopiesCreated() const
{
    if(m_tool == nullptr)
        return 0;

    return m_tool->nResultModels();
}

CT_InResultModelGroupToCopy::ToolToModifyResultModelCopiesType* CT_InResultModelGroupToCopy::toolToModifyResultModelCopies() const
{
    return m_tool;
}

void CT_InResultModelGroupToCopy::deleteToolToModifyResultModelCopies()
{
    delete m_tool;
    m_tool = nullptr;
}
