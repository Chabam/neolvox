#include "ct_step/abstract/ct_virtualabstractstep.h"
#include "lvox3_stepconfigurescanner.h"
#include "ct_global/ct_context.h"
#include "ct_itemdrawable/ct_scanner.h"
#include "loginterface.h"
#include "tools/scanner/lvox_shootingpatternformls.h"

LVOX3_StepConfigureScanner::LVOX3_StepConfigureScanner() : SuperClass()
{
    // Initialize member variables if needed
}

QString LVOX3_StepConfigureScanner::description() const
{
    return tr("1 - Configuration of the scanner");
}

QString LVOX3_StepConfigureScanner::detailledDescription() const
{
    return tr("Detailed description of the step");
}

QString LVOX3_StepConfigureScanner::inputDescription() const
{
    return SuperClass::inputDescription() + tr("<br><br>");
}

QString LVOX3_StepConfigureScanner::outputDescription() const
{
    return SuperClass::outputDescription() + tr("<br><br>");
}

QString LVOX3_StepConfigureScanner::detailsDescription() const
{
    return tr("");
}

CT_VirtualAbstractStep* LVOX3_StepConfigureScanner::createNewInstance() const
{
    // Create a copy of this step
    return new LVOX3_StepConfigureScanner();
}

/////////////////////// PROTECTED ///////////////////////

void LVOX3_StepConfigureScanner::declareInputModels(CT_StepInModelStructureManager& manager)
{
    manager.addResult(_inResult, tr("Scene(s)"), "", true);
    manager.setZeroOrMoreRootGroup(_inResult, _inZeroOrMoreRootGroup);
    manager.addGroup(_inZeroOrMoreRootGroup, _inGroup);
    manager.addItem(_inGroup, _inScene, tr("Scene(s)"));
    manager.addItem(_inGroup, _inAttLAS, tr("Attributs LAS"));

    manager.addResult(_inResultTraj, tr("Trajectory"), "", true);
    manager.setZeroOrMoreRootGroup(_inResultTraj, _inZeroOrMoreRootGroupTraj);
    manager.addGroup(_inZeroOrMoreRootGroupTraj, _inGroupTraj);
    manager.addItem(_inGroupTraj, _inTraj, tr("Trajectory"));
}

void LVOX3_StepConfigureScanner::fillPostInputConfigurationDialog(CT_StepConfigurableDialog* postInputConfigDialog)
{
}

void LVOX3_StepConfigureScanner::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    manager.addResultCopy(_inResult);
    manager.addItem(_inGroup, m_outScanner, tr("Scanner LVOX"), tr("Scanner position"));
    manager.addItem(_inGroup, m_outShootingPattern, tr("Shooting Pattern MLS"), tr("Shooting Pattern/Position/Direction/Angle forced by user"));
}

void LVOX3_StepConfigureScanner::compute()
{
    for (const CT_ScanPath* scanPath : _inTraj.iterateInputs(_inResultTraj))
    {
        for (CT_StandardItemGroup* group : _inGroup.iterateOutputs(_inResult))
        {
            int scanId = 0;
            for (const CT_AbstractItemDrawableWithPointCloud* inScene : group->singularItems(_inScene))
            {
                const CT_StdLASPointsAttributesContainer* attributeLAS = group->singularItem(_inAttLAS);
                CT_AbstractPointAttributesScalar* gpsAttributes = dynamic_cast<CT_AbstractPointAttributesScalar*>(attributeLAS->pointsAttributesAt(CT_LasDefine::GPS_Time));

                const CT_PCIR pcir = inScene->pointCloudIndexRegistered();

                CT_ShootingPattern *pattern = new LVOX_ShootingPatternForMLS(pcir, *scanPath, gpsAttributes);
                CT_Scanner *scanner = new CT_Scanner(scanId, pattern);
                group->addSingularItem(m_outScanner, scanner);

                CT_ShootingPatternD *patternD = new CT_ShootingPatternD(pattern);
                group->addSingularItem(m_outShootingPattern, patternD);

                ++scanId;
            }
        }

        setProgress(100.0f);
    }

}

