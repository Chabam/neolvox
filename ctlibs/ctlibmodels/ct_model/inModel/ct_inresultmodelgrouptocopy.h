#ifndef CT_INRESULTMODELGROUPTOCOPY_H
#define CT_INRESULTMODELGROUPTOCOPY_H

#include "ct_model/inModel/ct_inresultmodelgroup.h"
#include "ct_model/outModel/tools/ct_outresultmodelgrouptocopypossibilities.h"

/**
 * @brief A input result model that represent a CT_ResultGroup that you want to search and COPY ! Per example if you want to
 *        add other information like Item or Attributes, etc...
 */
class CTLIBMODELS_EXPORT CT_InResultModelGroupToCopy : public CT_InResultModelGroup
{
    Q_OBJECT
    typedef CT_InResultModelGroup SuperClass;

public:
    using ToolToModifyResultModelCopiesType = CT_OutResultModelGroupToCopyPossibilities;

    /**
     * @brief Construct a input result group model
     * @param displayableName : a displayable name for this model. If not set use "Result (copy)" by default.
     * @param shortDescription : a short description of the result you search. If empty by default use the displayable name.
     * @param recursive : true if you want that this model research recusively results in all step in the tree (go up in hierarchy) or false
     *                    if you want just research in results of the step passed in parameter of the method "searchResultsInStep...". False by default.
     * @param minimumNumberOfPossibilityToSelect : the minimum number of possibility that must be selected
     * @param maximumNumberOfPossibilityThatCanBeSelected : the maximum number of possibility that can be selected. -1 if you want no limit.
     */
    CT_InResultModelGroupToCopy(const QString& displayableName = "Result (copy)",
                                const QString& shortDescription = "",
                                bool recursive = false,
                                quint8 minimumNumberOfPossibilityToSelect = 1,
                                int maximumNumberOfPossibilityThatCanBeSelected = -1);
    /**
     * @brief Copy constructor
     * @param other : the object to copy
     * @param withPossibilities : to true if possibilities of this object and of childrens must be copied
     */
    CT_InResultModelGroupToCopy(const CT_InResultModelGroupToCopy& other, const bool& withPossibilities);

    ~CT_InResultModelGroupToCopy() override;

    /**
     * @brief This will create a copy of all output result model that was selected by the user has a possibility and return a tool that can
     *        add/remove elements from all output result model in the same time. Check the documentation of the class
     *        CT_OutResultModelGroupToCopyPossibilities for more information.
     */
    ToolToModifyResultModelCopiesType* createOutResultModelCopiesAccordingToNumberOfPossibility();

    /**
     * @brief Returns the tool that must be used to add/remove elements in all output result models copied
     */
    ToolToModifyResultModelCopiesType* toolToModifyResultModelCopies() const;

    /**
     * @brief Delete from memory the tool created when the method "createOutResultsModelCopyAndToolAccordingToNumberOfPossibility" has been
     *        called
     */
    void deleteToolToModifyResultModelCopies();

    /**
     * @brief Returns the number of out models create when the method "createOutResultsModelCopyAndToolAccordingToNumberOfPossibility" has been
     *        called.
     */
    int nOutResultModelCopiesCreated() const;

    // INHERITED FROM CT_InAbstractModel //
    CT_InAbstractModel* copy(bool withPossibilities) const override;

private:
    ToolToModifyResultModelCopiesType* m_tool;
};

#endif // CT_INRESULTMODELGROUPTOCOPY_H
