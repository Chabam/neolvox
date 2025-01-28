#ifndef LVOX2_STEPCOMPUTEHEIGHTPROFILE_H
#define LVOX2_STEPCOMPUTEHEIGHTPROFILE_H

#ifdef USE_OPENCV

#include "ct_step/abstract/ct_abstractstep.h"
#include "ct_tools/model/ct_autorenamemodels.h"



class LVOX2_StepComputeHeightProfile: public CT_AbstractStep
{
    Q_OBJECT

public:

    /*! \brief Step constructor
     * 
     * Create a new instance of the step
     * 
     * \param dataInit Step parameters object
     */
    LVOX2_StepComputeHeightProfile(CT_StepInitializeData &dataInit);

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

    CT_AutoRenameModels _outProfileGrp_ModelName;
    CT_AutoRenameModels _outProfile_ModelName;
    CT_AutoRenameModels _outProfileThreshold_ModelName;
    CT_AutoRenameModels _outProfileStrata1_ModelName;

    double _min;
    double _max;
    double _step;
    double _limStata1;

};

#endif

#endif // LVOX2_STEPCOMPUTEHEIGHTPROFILE_H
