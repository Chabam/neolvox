#include "lvox3_stepmakeshootingpaternformls.h"
#include "ct_global/ct_context.h"
#include "ct_itemdrawable/ct_scanner.h"
#include "loginterface.h"
#include "tools/scanner/lvox_shootingpatternformls.h"

lvox3_stepmakeshootingpaternformls::lvox3_stepmakeshootingpaternformls() : SuperClass()
{
    // Initialize member variables if needed
}

QString lvox3_stepmakeshootingpaternformls::description() const
{
    return tr("Calcul des vecteurs directeurs");
}

QString lvox3_stepmakeshootingpaternformls::detailledDescription() const
{
    return tr("Detailed description of the step");
}

QString lvox3_stepmakeshootingpaternformls::inputDescription() const
{
    return SuperClass::inputDescription() + tr("<br><br>");
}

QString lvox3_stepmakeshootingpaternformls::outputDescription() const
{
    return SuperClass::outputDescription() + tr("<br><br>");
}

QString lvox3_stepmakeshootingpaternformls::detailsDescription() const
{
    return tr("");
}

CT_VirtualAbstractStep* lvox3_stepmakeshootingpaternformls::createNewInstance() const
{
    // Create a copy of this step
    return new lvox3_stepmakeshootingpaternformls();
}

/////////////////////// PROTECTED ///////////////////////

void lvox3_stepmakeshootingpaternformls::declareInputModels(CT_StepInModelStructureManager& manager)
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

void lvox3_stepmakeshootingpaternformls::fillPostInputConfigurationDialog(CT_StepConfigurableDialog* postInputConfigDialog)
{
}

void lvox3_stepmakeshootingpaternformls::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    manager.addResultCopy(_inResult);
    manager.addItem(_inGroup, m_outScanner, tr("Scanner LVOX MLS"), tr("Scanner position"));
    manager.addItem(_inGroup, m_outShootingPattern, tr("Shooting Pattern MLS"), tr("Shooting Pattern/Position/Direction/Angle forced by user"));
}

void lvox3_stepmakeshootingpaternformls::compute()
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
            PS_LOG->addInfoMessage(LogInterface::step, QString::number(i));

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

