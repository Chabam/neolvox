/**
 * @author Michael Krebs (AMVALOR)
 * @author Van Tho Nguyen
 * @date 25.01.2017
 * @version 2
 */
#ifndef LVOX3_STEPCOMPUTELVOXGRIDS_H
#define LVOX3_STEPCOMPUTELVOXGRIDS_H

#include "ct_step/abstract/ct_abstractstep.h"
#include "ct_itemdrawable/ct_scene.h"
#include "ct_itemdrawable/ct_scanner.h"
#include "ct_itemdrawable/ct_shootingpatternd.h"
#include "tools/lvox3_gridtype.h"
#include "tools/lvox3_gridmode.h"
#include "view/computegridsconfiguration.h"

/**
 * @brief Compute the LVOX Ni, Nb, Nt grids
 */
class LVOX3_StepComputeLvoxGrids : public CT_AbstractStep
{
    Q_OBJECT
    using SuperClass = CT_AbstractStep;
public:
    using VegetationType = ComputeGridsConfiguration::VegetationType;
    using GridMode = lvox::GridMode;

    LVOX3_StepComputeLvoxGrids();

    QString description() const;

    QString detailledDescription() const;

    CT_VirtualAbstractStep* createNewInstance() const final;

    void savePostSettings(SettingsWriterInterface& writer) const final;

    bool restorePostSettings(SettingsReaderInterface &reader) final;

    QString parametersDescription(bool defaultValue) final;
    QString outputDescription() const final;

protected:
    bool postInputConfigure() final;

    void declareInputModels(CT_StepInModelStructureManager& manager) final;

    // void fillPostInputConfigurationDialog(CT_StepConfigurableDialog* configDialog) final;

    void declareOutputModels(CT_StepOutModelStructureManager& manager) final;

    void compute() final;

    float computeElementProjectedArea();

private:
    CT_HandleInResultGroupCopy<>                                                        _inResult;
    CT_HandleInStdZeroOrMoreGroup                                                       _inZeroOrMoreRootGroup;
    CT_HandleInStdGroup<>                                                               _inGroup;
    CT_HandleInSingularItem<CT_AbstractItemDrawableWithPointCloud>                      _inScene;
    CT_HandleInSingularItem<CT_Scanner>                                                 _inScan;
    CT_HandleInSingularItem<CT_ShootingPatternD>                                        _inShootingPattern;

    CT_HandleOutSingularItem<lvox::Grid3Di>                                             _outHits;
    CT_HandleOutSingularItem<lvox::Grid3Di>                                             _outTheo;
    CT_HandleOutSingularItem<lvox::Grid3Di>                                             _outBef;

    //********************************************//
    //              Attributes of LVox            //
    //********************************************//


    QHash<QString, double> _confidenceLevelCollection;

    QString _confidenceLevel;
    // Element characteristics (for projected area computation)
    double          m_lengthOrExtension1;
    double          m_diameterOrExtension2;
    double          m_flatProjectedArea;
    bool            m_computeDistances;         /*!< true if must compute distance */
    //bool _onlyHits;

    GridMode        m_gridMode;
    VegetationType  m_elementProjectedAreaMethod;
    double m_resolution;               /*!< voxel size on the x,y and z axis */
    Eigen::Vector3d m_coordinates;              /*!< coordinates if gridMode == ...Coordinates... */
    Eigen::Vector3d m_dimensions;               /*!< dimensions if gridMode == ...CustomDimensions */
    QStringList     m_gridFilePath;             /*!< Name of .grid L-Architect reference 3D grid */

private slots:
    /**
     * @brief Called from worker manager when progress changed
     */
    void progressChanged(int p);
};

#endif // LVOX3_STEPCOMPUTELVOXGRIDS_H
