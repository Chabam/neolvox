/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_STEPINTERPOLATEDISTANCE_H
#define LVOX3_STEPINTERPOLATEDISTANCE_H


#include "ct_step/abstract/ct_abstractstep.h"

#include "ct_itemdrawable/ct_scene.h"
#include "tools/lvox3_gridtype.h"
#include "tools/lvox3_gridtools.h"

/**
 * @brief Compute the sky of a scene
 */
class LVOX3_StepInterpolateDistance : public CT_AbstractStep
{
    Q_OBJECT

public:
    enum InterpolationMethod {
        Distance = 0,
        Trust,
        DistanceAndTrust,
        ZAverage,
        KrigingBinomial,
        KrigingNormal
    };

    LVOX3_StepInterpolateDistance();

    QString description() const;

    QString detailledDescription() const;

    CT_VirtualAbstractStep* createNewInstance() const final;

    QString parametersDescription(bool defaultValue) final;
    QString outputDescription() const final;

protected:

    void declareInputModels(CT_StepInModelStructureManager& manager) final;

    void fillPreInputConfigurationDialog(CT_StepConfigurableDialog* configDialog) final;

    void fillPostInputConfigurationDialog(CT_StepConfigurableDialog* configDialog) final;

    void declareOutputModels(CT_StepOutModelStructureManager& manager) final;

    void compute() final;

    /**
     * @brief This method interpolates the density grid based on the LVOX1 version of Z axis average interpolation
     */
    void ZAverageInterpolation(const lvox::Grid3Df *INdensityGrid,lvox::Grid3Df* const OUTdensityGrid,const lvox::Grid3Di *hitGrid,const lvox::Grid3Di *theoreticalGrid,const lvox::Grid3Di *beforeGrid);

private:
    QMap<QString, InterpolationMethod> m_interpolationMethodCollection;

    double  m_interpolateRadius;
    QString m_interpolationMethod;

    /**
     * Distance interpolation
     */
    double  m_interpolateDensityThreshold;
    int     m_interpolatePower;

    /**
     * Trust interpolation
     */
    bool    m_enableInterpolateTrust;
    int     m_trustLowThreshold;
    int     m_trustHighThreshold;

    /**
     * Z Average interpolation
     */
    int     m_effectiveRayThresh;
    //bool m_isRelativeThreshold
    int     m_numZCalculatedOn;



    CT_HandleInResultGroupCopy<>                                    _inResult;
    CT_HandleInStdZeroOrMoreGroup                                   _inZeroOrMoreRootGroup;
    CT_HandleInStdGroup<>                                           _inGroup;
    CT_HandleInSingularItem<lvox::Grid3Df >                         _inGrid_density;

    CT_HandleInSingularItem<lvox::Grid3Di >                         _inGrid_ni;
    CT_HandleInSingularItem<lvox::Grid3Di >                         _inGrid_nt;
    CT_HandleInSingularItem<lvox::Grid3Di >                         _inGrid_nb;


    CT_HandleOutSingularItem<lvox::Grid3Df>                         _outGrid_density;

//    CT_AutoRenameModels m_outInterpolatedGridModelName;

private slots:
    /**
     * @brief Called when the progress changed of the worker that compute number of points in the grid
     */
    void workerProgressChanged(int p);
};

#endif // LVOX3_STEPINTERPOLATEDISTANCE_H
