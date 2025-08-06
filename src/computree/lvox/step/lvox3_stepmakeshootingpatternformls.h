#ifndef LVOX3_STEPMAKESHOOTINGPATERNFORMLS_H
#define LVOX3_STEPMAKESHOOTINGPATERNFORMLS_H

#include "ct_itemdrawable/ct_scanner.h"
#include "ct_step/abstract/ct_abstractstep.h"

#include "ct_itemdrawable/ct_pointsattributesscalartemplated.h"
#include "ct_itemdrawable/ct_scene.h"
#include "ct_itemdrawable/ct_scanpath.h"
#include "ctliblas/itemdrawable/las/ct_stdlaspointsattributescontainer.h"

#include "ct_itemdrawable/ct_shootingpatternd.h"
#include "ct_itemdrawable/tools/scanner/ct_shootingpattern.h"

class LVOX3_StepMakeShootingPatternForMLS : public CT_AbstractStep
{
    Q_OBJECT
    using SuperClass = CT_AbstractStep;

public:

    LVOX3_StepMakeShootingPatternForMLS();

    QString description() const override;
    QString detailledDescription() const override;
    QString inputDescription() const override;
    QString outputDescription() const override;
    QString detailsDescription() const override;

    CT_VirtualAbstractStep* createNewInstance() const final;

protected:

    void declareInputModels(CT_StepInModelStructureManager& manager) final;

    void fillPostInputConfigurationDialog(CT_StepConfigurableDialog* postInputConfigDialog) final;

    void declareOutputModels(CT_StepOutModelStructureManager& manager) final;

    void compute() final;

private:

    // Declare member variables if needed
    CT_HandleInResultGroupCopy<>                                                        _inResult;
    CT_HandleInStdZeroOrMoreGroup                                                       _inZeroOrMoreRootGroup;
    CT_HandleInStdGroup<>                                                               _inGroup;
    CT_HandleInSingularItem<CT_AbstractItemDrawableWithPointCloud>                      _inScene;
    CT_HandleInSingularItem<CT_StdLASPointsAttributesContainer>                         _inAttLAS;

    CT_HandleInResultGroup<>                                                            _inResultTraj;
    CT_HandleInStdZeroOrMoreGroup                                                       _inZeroOrMoreRootGroupTraj;
    CT_HandleInStdGroup<>                                                               _inGroupTraj;
    CT_HandleInSingularItem<CT_ScanPath>                                                _inTraj;


    CT_HandleOutSingularItem<CT_StdLASPointsAttributesContainer>                        _lasAttributesOut;
    CT_HandleOutPointScalarWithDenseManager<double>                                     _outIntensityAttribute;

    CT_HandleOutResultGroup                                                             m_hOutResult;
    CT_HandleOutStdGroup                                                                m_hOutRootGroup;
    CT_HandleOutSingularItem<CT_Scanner>                                                m_outScanner;
    CT_HandleOutSingularItem<CT_ShootingPatternD>                                       m_outShootingPattern;
};

#endif // LVOX3_STEPMAKESHOOTINGPATERNFORMLS_H
