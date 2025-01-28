/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_STEPCOMPUTEPROFILES_H
#define LVOX3_STEPCOMPUTEPROFILES_H

#include "ct_step/abstract/ct_abstractstep.h"

#include "tools/3dgrid/abstract/lvox3_abstractgrid3d.h"
#include "ct_itemdrawable/ct_profile.h"
#include "ct_itemdrawable/ct_standarditemgroup.h"
#include "tools/lvox3_gridtype.h"

#include "view/computeprofilesconfiguration.h"

/**
 * @brief Compute profile from grid
 */
class LVOX3_StepComputeProfiles : public CT_AbstractStep
{
    Q_OBJECT

public:
    LVOX3_StepComputeProfiles();

    QString description() const;

    QString detailledDescription() const;

    CT_VirtualAbstractStep* createNewInstance() const final;

    void savePostSettings(SettingsWriterInterface &writer) const;

    bool restorePostSettings(SettingsReaderInterface &reader);

    QString parametersDescription(bool defaultValue) final;

    QString outputDescription() const final;

protected:

    void declareInputModels(CT_StepInModelStructureManager& manager) final;

    void declareOutputModels(CT_StepOutModelStructureManager& manager) final;

    void compute() final;

    bool postInputConfigure();

private:
    typedef Eigen::Matrix<size_t, 3, 1> Vector3SizeT;


    CT_HandleInResultGroupCopy<>                        _inResult;
    CT_HandleInStdZeroOrMoreGroup                       _inZeroOrMoreRootGroup;
    CT_HandleInStdGroup<>                               _inGroup;
    CT_HandleInSingularItem<LVOX3_AbstractGrid3D >      _inGrid;

    CT_HandleOutStdGroup                                _outGroup;
    CT_HandleOutSingularItem<CT_Profile<double> >       _outProfile;
    CT_HandleOutSingularItem<lvox::Grid3Df>             _outGrid3D;

    ComputeProfilesConfiguration::Configuration m_configuration;

    /**
     * @brief Create a new profile and return it
     */
    CT_Profile<double>* createProfile(const LVOX3_AbstractGrid3D* grid,
                                      const size_t& currentIndex,
                                      const Eigen::Vector3i& axeNormal,
                                      const Eigen::Vector3i& axeOrdonnee,
                                      double NAValue) const;

    /**
     * @brief Add the profile
     */
    void addProfile(CT_Profile<double>* profile, CT_StandardItemGroup* group, lvox::Grid3Df* grid);

    /**
     * @brief Modify start and end by using min, max, dim and minAndMaxInPourcent values
     */
    static void setStartEnd(size_t min, size_t max, Vector3SizeT& startEndStep, const size_t& dim, bool minAndMaxInPourcent);

    /**
     * @brief Modify step by using step and stepInPourcent values
     */
    static void setStep(size_t step, Vector3SizeT& startEndStep, bool stepInPourcent);

    /**
     * @brief Changes the values entered to compute the profile (meters to voxels)
     */
    QList<double> changeToVoxel(const LVOX3_AbstractGrid3D* inGrid);
};

#endif // LVOX3_STEPCOMPUTEPROFILES_H
