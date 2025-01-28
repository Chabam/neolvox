#ifndef LVOX2_STEPCOMBINELVOXGRIDS_H
#define LVOX2_STEPCOMBINELVOXGRIDS_H

#include "ct_step/abstract/ct_abstractstep.h"

/*!
 * \class LVOX2_StepCombineLvoxGrids
 * \ingroup Steps_LVOX
 * \brief <b>Combines density grids from different points of view.</b>
 *
 * Different modes are possible :
 * - max (density)
 * - max (nt - nb)
 * - max (ni)
 *
 * \param _mode Choosen mode for combination
 *
 * <b>Input Models:</b>
 *
 * - CT_ResultGroup\n
 *     - CT_StandardItemGroup...\n
 *         - CT_RegularGridInt (hits)\n
 *         - CT_RegularGridInt (theorical)\n
 *         - CT_RegularGridInt (before)\n
 *         - CT_RegularGridDouble (density)\n
 *
 * <b>Output Models:</b>
 *
 * - CT_ResultGroup (grids)\n
 *     - CT_StandardItemGroup (grids)...\n
 *         - CT_RegularGridInt (hits)\n
 *         - CT_RegularGridInt (theorical)\n
 *         - CT_RegularGridInt (before)\n
 *         - CT_RegularGridDouble (density)\n
 *         - CT_RegularGridInt (scanId)\n
 *
 */

class LVOX2_StepCombineLvoxGrids: public CT_AbstractStep
{
    Q_OBJECT

public:

    enum combinaisonMode
    {
        maxDensity,
        maxNt_Nb,
        maxNt_Nb_div_Nt,
        maxNi,
        sumNiSumNtNb
    };

    /*! \brief Step constructor
     * 
     * Create a new instance of the step
     * 
     * \param dataInit Step parameters object
     */
    LVOX2_StepCombineLvoxGrids(CT_StepInitializeData &dataInit);

    /*! \brief Step description
     * 
     * Return a description of the step function
     */
    QString getStepDescription() const;

    /*! \brief Step copy
     * 
     * Step copy, used when a step is added by step contextual menu
     */
    CT_VirtualAbstractStep* createNewInstance(CT_StepInitializeData &dataInit);

protected:

    /*! \brief Input results specification
     * 
     * Specification of input results models needed by the step (IN)
     */
    void createInResultModelListProtected();

    /*! \brief Parameters DialogBox
     * 
     * DialogBox asking for step parameters
     */
    void createPostConfigurationDialog();

    /*! \brief Output results specification
     * 
     * Specification of output results models created by the step (OUT)
     */
    void createOutResultModelListProtected();

    /*! \brief Algorithm of the step
     * 
     * Step computation, using input results, and creating output results
     */
    void compute();

private:

    // Step parameters
    int    _mode;
    int    _effectiveRayThresh;
    bool   _UseOnlyNotEmptyCellsIf_maxNt_Nb_div_Nt_selected;

};

#endif // LVOX2_STEPCOMBINELVOXGRIDS_H
