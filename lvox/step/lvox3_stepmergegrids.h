#ifndef LVOX3_STEPMERGEGRIDS_H
#define LVOX3_STEPMERGEGRIDS_H

#include "ct_step/abstract/ct_abstractstep.h"

#include "tools/lvox3_gridtype.h"
#include "tools/lvox3_gridtools.h"
#include "tools/lvox3_mergegrids.h"


class LVOX3_StepMergeGrids : public CT_AbstractStep
{
    Q_OBJECT

public:
    LVOX3_StepMergeGrids();

    QString description() const;

    QString detailledDescription() const;

    CT_VirtualAbstractStep* createNewInstance() const final;

    void savePostSettings(SettingsWriterInterface& writer) const final;

    bool restorePostSettings(SettingsReaderInterface &reader) final;

    QString parametersDescription(bool defaultValue) final;

    QString outputDescription() const final;


protected:
    void declareInputModels(CT_StepInModelStructureManager& manager) final;

    void declareOutputModels(CT_StepOutModelStructureManager& manager) final;

    void compute() final;

    bool postInputConfigure();

private slots:
    /**
     * @brief Called when the worker progress change
     */
    void workerProgressChanged(int p);

private:
    VoxelReducerOptions m_reducerOptions;

    CT_HandleInResultGroup<>                                        _inResult;
    CT_HandleInStdZeroOrMoreGroup                                   _inZeroOrMoreRootGroup;
    CT_HandleInStdGroup<>                                           _inGroup;

    CT_HandleInSingularItem<lvox::Grid3Di >                         _inGrid_ni;
    CT_HandleInSingularItem<lvox::Grid3Di >                         _inGrid_nt;
    CT_HandleInSingularItem<lvox::Grid3Di >                         _inGrid_nb;
    CT_HandleInSingularItem<lvox::Grid3Df >                         _inGrid_density;


    CT_HandleOutResultGroup                                         _outResult;
    CT_HandleOutStdGroup                                            _outRootGroup;
    CT_HandleOutSingularItem<lvox::Grid3Di>                         _outGrid_ni;
    CT_HandleOutSingularItem<lvox::Grid3Di>                         _outGrid_nt;
    CT_HandleOutSingularItem<lvox::Grid3Di>                         _outGrid_nb;
    CT_HandleOutSingularItem<lvox::Grid3Df>                         _out_Density;


//    CT_AutoRenameModels m_mergedGridHits;
//    CT_AutoRenameModels m_mergedGridBefore;
//    CT_AutoRenameModels m_mergedGridTheoretic;
//    CT_AutoRenameModels m_mergedGridDensity;

//    CT_AutoRenameModels m_mergedNiFlag;
//    CT_AutoRenameModels m_mergedNbFlag;
//    CT_AutoRenameModels m_mergedNtFlag;
};

#endif // LVOX3_STEPMERGEGRIDS_H
