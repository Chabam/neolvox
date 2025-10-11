#include "ct_step/abstract/ct_virtualabstractstep.h"
#include "lvox3_stepmakeshootingpatternformls.h"
#include "ct_global/ct_context.h"
#include "ct_itemdrawable/ct_scanner.h"
#include "loginterface.h"
#include "tools/scanner/lvox_shootingpatternformls.h"

LVOX3_StepMakeShootingPatternForMLS::LVOX3_StepMakeShootingPatternForMLS() : SuperClass()
{
    // Initialize member variables if needed
}

QString LVOX3_StepMakeShootingPatternForMLS::description() const
{
    return tr("Calcul des vecteurs directeurs");
}

QString LVOX3_StepMakeShootingPatternForMLS::detailledDescription() const
{
    return tr("Detailed description of the step");
}

QString LVOX3_StepMakeShootingPatternForMLS::inputDescription() const
{
    return SuperClass::inputDescription() + tr("<br><br>");
}

QString LVOX3_StepMakeShootingPatternForMLS::outputDescription() const
{
    return SuperClass::outputDescription() + tr("<br><br>");
}

QString LVOX3_StepMakeShootingPatternForMLS::detailsDescription() const
{
    return tr("");
}

CT_VirtualAbstractStep* LVOX3_StepMakeShootingPatternForMLS::createNewInstance() const
{
    // Create a copy of this step
    return new LVOX3_StepMakeShootingPatternForMLS();
}

/////////////////////// PROTECTED ///////////////////////

void LVOX3_StepMakeShootingPatternForMLS::declareInputModels(CT_StepInModelStructureManager& manager)
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

void LVOX3_StepMakeShootingPatternForMLS::fillPostInputConfigurationDialog(CT_StepConfigurableDialog* postInputConfigDialog)
{
}

void LVOX3_StepMakeShootingPatternForMLS::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    manager.addResultCopy(_inResult);
    manager.addItem(_inGroup, m_outScanner, tr("Scanner LVOX MLS"), tr("Scanner position"));
    manager.addItem(_inGroup, m_outShootingPattern, tr("Shooting Pattern MLS"), tr("Shooting Pattern/Position/Direction/Angle forced by user"));
}

void LVOX3_StepMakeShootingPatternForMLS::compute()
{
    // Perform the computation steps
    const CT_ScanPath* trajectorie;

    for (const CT_ScanPath* scanPath : _inTraj.iterateInputs(_inResultTraj))
    {
        trajectorie = scanPath;
    }

    auto iterator = _inScene.iterateOutputs(_inResult);

    auto it = iterator.begin();
    auto end = iterator.end();

    int i = 0;
    while(it != end)
    {
        // i++;
        // PS_LOG->addInfoMessage(LogInterface::step, QString::number(i));
        const CT_AbstractItemDrawableWithPointCloud* scene = *it;

        CT_StandardItemGroup* grp = it.currentParent();

        const CT_StdLASPointsAttributesContainer* attributeLAS = nullptr;
        CT_AbstractPointAttributesScalar* attributeGPS = nullptr;

        CT_PCIR pcir = nullptr;

        for(const CT_StdLASPointsAttributesContainer* attLAS : grp->singularItems(_inAttLAS))
        {
            i++;

            attributeGPS          = dynamic_cast<CT_AbstractPointAttributesScalar*>(attLAS->pointsAttributesAt(CT_LasDefine::GPS_Time));

            if (attributeGPS != nullptr)
            {
                attributeLAS = attLAS;
                break;
            }
        }

        if (scene != nullptr)
        {
            //Gets the scene's pointcloudindexregistered to avoid duplicating point clouds.
            pcir = scene->pointCloudIndexRegistered();
        }

        CT_ShootingPattern *pattern = new LVOX_ShootingPatternForMLS(pcir, *trajectorie, attributeGPS);
        if(pattern != nullptr)
        {
            CT_Scanner *scanner = new CT_Scanner(i, pattern);
            grp->addSingularItem(m_outScanner, scanner);

            CT_ShootingPatternD *patternD = new CT_ShootingPatternD(pattern);
            grp->addSingularItem(m_outShootingPattern, patternD);
        }

        ++it;
    }

    setProgress(100.0f);
}

