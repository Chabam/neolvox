#include <memory>

#include "lvox3_stepinterpolatedistance.h"

//Workers

#include <liblvox/algorithms/interpolatedistances.h>
#include "ct_global/ct_context.h"
#include "datastructures/lvox3_implgrid3d.h"
#include "loginterface.h"
#include "tools/lvox3_logger.h"
using namespace std;

LVOX3_StepInterpolateDistance::LVOX3_StepInterpolateDistance()
    : CT_AbstractStep(),
      m_interpolateRadius(1.0),
      m_interpolateDensityThreshold(0.0),
      m_interpolatePower(0),
      m_trustLowThreshold(10),
      m_trustHighThreshold(30)
{

    m_numZCalculatedOn = 1; //number of z slices to used for average density
	m_effectiveRayThresh = 10; //number of shots needed to consider    m_interpolationMethod = tr("Distance");

    m_interpolationMethodCollection.insert(tr("Distance"), Distance);
    m_interpolationMethodCollection.insert(tr("Trust"), Trust);
    //Version from LVOX1, less accurate but faster
    m_interpolationMethodCollection.insert(tr("Z Axis Average"),ZAverage);
    // m_interpolationMethodCollection.insert(tr("Kriging Binomial (In Development)"),KrigingBinomial);
    // m_interpolationMethodCollection.insert(tr("Kriging Normal (In Development)"),KrigingNormal);
    // TODO : change compute method to enable do this !
    //m_interpolationMethodCollection.insert(tr("Distance and Trust"), DistanceAndTrust);
}

QString LVOX3_StepInterpolateDistance::description() const
{
    return tr("5 - NODATA interpolation (LVOX 3)");
}

// Step detailed description
QString LVOX3_StepInterpolateDistance::detailledDescription() const
{
    return tr("This step allows for interpolation of NODATA and error codes (shown as negative values). Three interpolation methods are provided. Two based on the neighboorhood (Distance and Trust) as well as one based on the Z average, faster but less precise.");
}

CT_VirtualAbstractStep*
LVOX3_StepInterpolateDistance::createNewInstance() const
{
    // Creates an instance of this step
    return new LVOX3_StepInterpolateDistance();
}

QString LVOX3_StepInterpolateDistance::parametersDescription()
{
    return QString() +
            "<strong>" + tr("Pre-configuration settings") +  "</strong> " + "(cannot be changed once the step is added)" + " :" +
                "<ul>" +
                    "<li>" + tr("Interpolation type") + " : " + tr("String") + " <em>" + tr("with the following possible values") + "</em> = " +
                        "<ul>" +
                            "<li>" + tr("Distance") + "<em>" + tr("is default") + "</em>" +"</li>" +
                            "<li>" + tr("Trust") + "</li>" +
                            "<li>" + tr("Z Axis Average") + "</li>" +
                            "<li>" + tr("Kriging Binomial (In Development)") + "</li>" +
                            "<li>" + tr("Kriging Normal (In Development)") + "</li>" +
                        "</ul>" +
                    "</li>" +
                "</ul>" +
            "<strong>" + tr("Step parameters:") + "</strong>" +
            "<br><em>" + tr("The following settings depends on the interpolation type chosen") + "</em>" +
            "<ul>" +
                "<li>" + tr("Distance") +
                    "<ul>" +
                        "<li>" + tr("Minimum density") + " : " + tr("double") + " <em>" + tr("default") + "</em> " + "0.000" + "</li>" +
                        "<li>" + tr("Interpolation radius") + " : " + tr("double") + " <em>" + tr("default") + "</em> " + "1.000" +  "</li>" +
                        "<li>" + tr("Decay power") + " : " + tr("integer") + " <em>" + tr("default") + "</em> " + "0" +  "</li>" +
                    "</ul>" +
                "</li>" +
                "<li>" + tr("Trust") +
                    "<ul>" +
                        "<li>" + tr("Minimum density") + " : " + tr("double") + " <em>" + tr("default") + "</em> " + "0.000" +  "</li>" +
                        "<li>" + tr("Interpolation radius") + " : " + tr("double") + " <em>" + tr("default") + "</em> " + "1.000" +  "</li>" +
                        "<li>" + tr("Lower bound") + " : " + tr("integer") + " <em>" + tr("default") + "</em> " + "10" +  "</li>" +
                        "<li>" + tr("Upper bound") + " : " + tr("integer") + " <em>" + tr("default") + "</em> " + "30" +  "</li>" +
                    "</ul>" +
                "</li>" +
                "<li>" + tr("Z Axis Average") +
                    "<ul>" +
                        "<li>" + tr("Effective rays (Nt - Nb) Threshold") + " : " + tr("integer") + " <em>" + tr("default") + "</em> " + "10" +  "</li>" +
                        "<li>" + tr("Number of heights Z axis average is computed on") + " : " + tr("integer") + " <em>" + tr("defaul") + "</em> " + "1" +  "</li>" +
                    "</ul>" +
                "</li>" +
                "<li>" + tr("Kriging Binomial (In Development)") +
                    "<ul>" +
                        "<li>" + tr("Effective rays (Nt - Nb) Threshold") + " : " + tr("integer") + " <em>" + tr("default") + "</em> " + "10" +  "</li>" +
                        "<li>" + tr("Interpolation radius") + " : " + tr("double") + " <em>" + tr("default") + "</em> " + "1.000" +  "</li>" +
                    "</ul>" +
                "</li>" +
                "<li>" + tr("Kriging Normal (In Development)") +
                    "<ul>" +
                        "<li>" + tr("Effective rays (Nt - Nb) Threshold") + " : " + tr("integer") + " <em>" + tr("default") + "</em> " + "10" +  "</li>" +
                        "<li>" + tr("Interpolation radius") + " : " + tr("double") + " <em>" + tr("default") + "</em> " + "1.000" +  "</li>" +
                    "</ul>" +
                "</li>" +
            "</ul>"
            ;

}

QString LVOX3_StepInterpolateDistance::outputDescription() const
{
    auto indent = [](QString s) -> QString { return "<div style=\"margin: 0 0 0 40px;\">" + s+ "</div>"; };

    return CT_AbstractStep::outputDescription() + "<br>Group [Group]:" +
            indent("...") +
            indent("LVOX3_Grid3D<float> [Density - interpolated]");
}

// Choose the grid type to give a little more flexibility in grid choice MNT or 3D Grid
void LVOX3_StepInterpolateDistance::fillPreInputConfigurationDialog(CT_StepConfigurableDialog* configDialog)
{
    configDialog->addStringChoice(tr("Interpolation type"), "", m_interpolationMethodCollection.keys(), m_interpolationMethod);
}

void LVOX3_StepInterpolateDistance::declareInputModels(CT_StepInModelStructureManager& manager)
{
    /*
     * Requirements:
     *  - relative density 3D grid
     *  - theoretical rays 3D grid
     *  - blocked rays 3D grid
     */

    manager.addResult(_inResult, "Grids");
    manager.setZeroOrMoreRootGroup(_inResult, _inZeroOrMoreRootGroup);
    manager.addGroup(_inZeroOrMoreRootGroup, _inGroup);
    manager.addItem(_inGroup, _inGrid_ni, "Hits");
    manager.addItem(_inGroup, _inGrid_nt, "Theoretical");
    manager.addItem(_inGroup, _inGrid_nb, "Before");
    manager.addItem(_inGroup, _inGrid_density, "Density grid");

    /*
     * The theoretical and before grid required for the trust interpolation.
     *
     * FIXME: the user must select the proper grids, otherwise the computation
     * will not be valid. Is there a way to specifically request a default
     * grid?
     *
     * ATM, the output grids are matched in order, and the default order (Nt,
     * Nb) is correct, but this is obviously flaky, the user may make a mistake easily.
     *
     * In lvox_stepcombinedentisygrids.cpp, an item attribute is requested with
     * addItemAttributeModel, but lvox3_stepgenericcomputegrids.cpp does not
     * add such attributes that would allow matching.
     *
     * FIX : No way to choose a default model. But warning ! you must not set it as optional because you can
     *       use it potentially in your compute. The consequence is that the user must always
     *       have it. No other solution for this. Otherwise we must create another step.
     */
}

void LVOX3_StepInterpolateDistance::fillPostInputConfigurationDialog(CT_StepConfigurableDialog* configDialog)
{
    /*
     * FIXME:
     * - The settings for an algorithm are not disabled if the corresponding
     *   algorithm is not checked.
     * - The low/high range bound is not validated, meaning that the user can
     *   enter a lower bound greater than the higher bound.
     * - We should use a combobox instead of the checkbox for each algorithm.
     *   However, I haven't found a way to register a signal to it (not returned from the add)
     * - addWidget() is more generic, but is protected.
     *
     * FIX :
     * - Use addStringChoice() to add a combobox
     * - No way to enable/disable some widget if a specific action in the combox is choosed. The only way is
     *   to do a custom configuration dialog but it is heavier to implement.
     */

    if (m_interpolationMethodCollection.value(m_interpolationMethod) == Distance) {
        configDialog->addText(tr("Parameter for distance interpolation"));
        configDialog->addDouble(tr("Minimum density"), tr(""), 0.0, 1.0, 3,
                m_interpolateDensityThreshold, 0.0, tr("Minimum neighbor value to consider the voxel in the average"));
        configDialog->addDouble(tr("Interpolation radius"), tr("meter"), 0.0, 10000.0, 3, m_interpolateRadius);
        configDialog->addInt(tr("Decay power"), tr(""), 0, 100, m_interpolatePower);

    }else if(m_interpolationMethodCollection.value(m_interpolationMethod) == Trust) {
        configDialog->addText(tr("Parameters for trust interpolation"));
        configDialog->addDouble(tr("Minimum density"), tr(""), 0.0, 1.0, 3,
                m_interpolateDensityThreshold, 0.0, tr("Minimum neighbor value to consider the voxel in the average"));
        configDialog->addDouble(tr("Interpolation radius"), tr("meter"), 0.0, 10000.0, 3, m_interpolateRadius);        
        configDialog->addInt(tr("Lower bound"), tr("effective rays (Nt - Nb)"), 0, 1000, m_trustLowThreshold, tr("Voxel with lower effective rays are not trusted"));
        configDialog->addInt(tr("Higher bound"), tr("effective rays (Nt - Nb)"), 0, 1000, m_trustHighThreshold, tr("Voxel with higher effective rays are fully trusted"));

    }else if(m_interpolationMethodCollection.value(m_interpolationMethod) == ZAverage){
        configDialog->addText(tr("Parameters for Z axis average interpolation"));
        configDialog->addInt(tr("Effective rays (Nt - Nb) Threshold"),"",0,1000, m_effectiveRayThresh,tr("Voxel with lower effective rays are considered as having a density of 0"));
        //configDialog->addBool(tr("Is relative threshold, use Nb/Nt instead of N"),"","", m_isRelativeThreshold);
        configDialog->addInt(tr("Number of height Z axis average calculated on"),"",1,1000,m_numZCalculatedOn);
    }
    //else if(m_interpolationMethodCollection.value(m_interpolationMethod) == KrigingBinomial)
    //{
    //    configDialog->addText("Please note that this current interpolation method is still in development");
    //    configDialog->addText("Parameters for binomial Kriging interpolation");
    //    configDialog->addInt(tr("effective rays (Nt - Nb) Threshold"),"",0,1000, m_effectiveRayThresh,tr("Voxel with lower effective rays are considered as having a density of 0"));
    //    configDialog->addDouble(tr("Interpolation radius"), tr("meter"), 0.0, 10000.0, 3, m_interpolateRadius);
    //}else if(m_interpolationMethodCollection.value(m_interpolationMethod) == KrigingNormal)
    //{
    //    configDialog->addText("Please note that this current interpolation method is still in development");
    //    configDialog->addText("Parameters for normal Kriging interpolation");
    //    configDialog->addInt(tr("effective rays (Nt - Nb) Threshold"),"",0,1000, m_effectiveRayThresh,tr("Voxel with lower effective rays are considered as having a density of 0"));
    //    configDialog->addDouble(tr("Interpolation radius"), tr("meter"), 0.0, 10000.0, 3, m_interpolateRadius);
    //}
}

void LVOX3_StepInterpolateDistance::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    /*
     * addItemModel() declares the outputs, the object lvox::Grid3Df is only a
     * stub and is not actually used.
     *
     * FIXME: depending on the interpolation algorithm selected by the user,
     * only one grid will be produced and the output name should be different.
     *
     * FIX : just use one and choose a name that can be used for all selected algorithm....
     */

    manager.addResultCopy(_inResult);
    manager.addItem(_inGroup, _outGrid_density, "NODATA-Interpolated");
}

void LVOX3_StepInterpolateDistance::compute()
{
    InterpolateDistances::Settings settings;
    settings.EnableInterpolateTrust = m_enableInterpolateTrust;
    settings.InterpolateDensityThreshold = m_interpolateDensityThreshold;
    settings.TrustLowThreshold = m_trustLowThreshold;
    settings.TrustHighThreshold = m_trustHighThreshold;
    settings.EffectiveRayThresh = m_effectiveRayThresh;
    settings.InterpolatePower = m_interpolatePower;
    settings.NumZCalculatedOn = m_numZCalculatedOn;
    settings.InterpolateRadius = m_interpolateRadius;

    InterpolateDistances::Method selectedMethod;
    switch (m_interpolationMethodCollection.value(m_interpolationMethod)) {
    case ZAverage:
        selectedMethod = InterpolateDistances::Method::ZAverage;
        break;
    case Distance:
        selectedMethod = InterpolateDistances::Method::Distance;
        break;
    case Trust:
        selectedMethod = InterpolateDistances::Method::Trust;
        break;
    }

    std::vector<InterpolateDistances::Input> inputs;


    for (CT_StandardItemGroup* group : _inGroup.iterateOutputs(_inResult))
    {
        InterpolateDistances::Input in;
        const pluginlvox::Grid3Df* grid_density_in = group->singularItem(_inGrid_density);
        const pluginlvox::Grid3Df* grid_density_out = new pluginlvox::Grid3Df(*grid_density_in);
        const pluginlvox::Grid3Di* grid_ni = group->singularItem(_inGrid_ni);
        const pluginlvox::Grid3Di* grid_nt = group->singularItem(_inGrid_nt);
        const pluginlvox::Grid3Di* grid_nb = group->singularItem(_inGrid_nb);

        LVOX3_ImplGrid3d<float>* i_grid_density_in = new LVOX3_ImplGrid3d<float>(const_cast<pluginlvox::Grid3Df*>(grid_density_in));
        LVOX3_ImplGrid3d<float>* i_grid_density_out = new LVOX3_ImplGrid3d<float>(const_cast<pluginlvox::Grid3Df*>(grid_density_out));
        LVOX3_ImplGrid3d<int>* i_grid_ni = new LVOX3_ImplGrid3d<int>(const_cast<pluginlvox::Grid3Di*>(grid_ni));
        LVOX3_ImplGrid3d<int>* i_grid_nt = new LVOX3_ImplGrid3d<int>(const_cast<pluginlvox::Grid3Di*>(grid_nt));
        LVOX3_ImplGrid3d<int>* i_grid_nb = new LVOX3_ImplGrid3d<int>(const_cast<pluginlvox::Grid3Di*>(grid_nb));

        in.DensityIn = i_grid_density_in;
        in.DensityOut = i_grid_density_out;
        in.Nb = i_grid_nb;
        in.Ni = i_grid_ni;
        in.Nt = i_grid_nt;

        inputs.push_back(in);
    }

    LVOX3_Logger log;

    InterpolateDistances algo(inputs, selectedMethod, settings, &log);


    algo.compute();
}

void LVOX3_StepInterpolateDistance::workerProgressChanged(int p)
{
    setProgress(p);
}
