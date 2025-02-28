#include "lvox3_stepcomputelvoxgrids.h"

//Workers
#include "tools/worker/lvox3_computehits.h"
#include "tools/worker/lvox3_computebefore.h"
#include "tools/worker/lvox3_computetheoriticals.h"
#include "tools/worker/lvox3_computeall.h"

//Tools
#include <math.h>

//Core
#include <ct_log/ct_logmanager.h>
#include <ct_view/tools/ct_configurablewidgettodialog.h>

/**
 * @brief Compute the LVOX Ni, Nb, Nt grids
 */
LVOX3_StepComputeLvoxGrids::LVOX3_StepComputeLvoxGrids() : SuperClass()
{
    m_resolution = 0.5;
    m_computeDistances = false;
    //_onlyHits = false;

    m_gridMode = lvox::BoundingBoxOfTheScene;
    m_elementProjectedAreaMethod = VegetationType::NeedleFromDimension;
    m_coordinates.x() = 0.0;
    m_coordinates.y() = 0.0;
    m_coordinates.z() = 0.0;

    m_dimensions.x() = 80;
    m_dimensions.y() = 80;
    m_dimensions.z() = 80;

    m_lengthOrExtension1 = 6.0;
    m_diameterOrExtension2 = 2.0;
    m_flatProjectedArea = 0.1;

    //_elementProjectedAreaMethodCollection.insert(tr("Needle (from Length and Diameter)"), VegetationType::NeedleFromDimension);
    //_elementProjectedAreaMethodCollection.insert(tr("BroadLeaf (from Extension1 and Extension2)"), VegetationType::BroadLeafFromDimension);
    //_elementProjectedAreaMethodCollection.insert(tr("Needle Area (from flat scan)"), VegetationType::NeedleFromFlatArea);
    //_elementProjectedAreaMethodCollection.insert(tr("BroadLead Area (from flat scan)"), VegetationType::BroadLeafFromFlatArea);

    //_gridModeCollection.insert(tr("Scene's bounding box"), lvox::GridMode::BoundingBoxOfTheScene);
    //_gridModeCollection.insert(tr("Relative to coordinates"), lvox::GridMode::RelativeToCoordinates);
    //_gridModeCollection.insert(tr("Relative to following coordinates with custom dimensions"), lvox::GridMode::RelativeToCoordinatesAndCustomDimensions);
    //_gridModeCollection.insert(tr("Centered on following coordinates with custom dimensions"), lvox::GridMode::CenteredOnCoordinatesAndCustomDimensions);
    //_gridModeCollection.insert(tr("From other grid"), lvox::GridMode::FromOtherGrid);

    _confidenceLevelCollection.insert(tr("0,5"),0.5);
    _confidenceLevelCollection.insert(tr("0,9"),0.9);
    _confidenceLevelCollection.insert(tr("0,95"),0.95);
}

/**
 * @brief Return a short description of what this class does
 */
QString LVOX3_StepComputeLvoxGrids::description() const
{
    return tr("2 - Compute LVOX grids (LVOX 3)");
}

/*! \brief Step detailed description
 *
 * Return a detailed description of the step function
 */
QString LVOX3_StepComputeLvoxGrids::detailledDescription() const
{
    return tr("This step creates 3D grids for theoretical rays, actual hits rays and intercepted rays given a predefined voxel size.") + "<br><br>" +
           tr("All grids are created with the specified resolution.") + "<br>"
           "<em>" + tr("Note: ") + "</em>" + tr("Computing distances is mandatory in order to perform PAD computation")
        ;
}

/**
 * @brief Return a new empty instance of this class
 */
CT_VirtualAbstractStep* LVOX3_StepComputeLvoxGrids::createNewInstance() const
{
    // Creates an instance of this step
    //LVOX3_StepComputeLvoxGrids* step = new LVOX3_StepComputeLvoxGrids();
    //return dynamic_cast<CT_VirtualAbstractStep*> (step);
    return new LVOX3_StepComputeLvoxGrids();
}

void LVOX3_StepComputeLvoxGrids::savePostSettings(SettingsWriterInterface& writer) const
{
    CT_AbstractStep::savePostSettings(writer);

    writer.addParameter(this, "gridResolutionX", m_resolution);
    writer.addParameter(this, "gridResolutionY", m_resolution);
    writer.addParameter(this, "gridResolutionZ", m_resolution);
    writer.addParameter(this, "computeDistances", m_computeDistances);

    writer.addParameter(this, "projectedAreaMethod", m_elementProjectedAreaMethod);
    writer.addParameter(this, "lengthOrExtension1", m_lengthOrExtension1);
    writer.addParameter(this, "diameterOrExtension2", m_diameterOrExtension2);
    writer.addParameter(this, "projectedArea", m_flatProjectedArea);

    writer.addParameter(this,"gridReferenceMethod", m_gridMode);
    writer.addParameter(this,"coordinatesX", m_coordinates.x());
    writer.addParameter(this,"coordinatesY", m_coordinates.y());
    writer.addParameter(this,"coordinatesZ", m_coordinates.z());

    writer.addParameter(this,"dimensionsX", m_dimensions.x());
    writer.addParameter(this,"dimensionsY", m_dimensions.y());
    writer.addParameter(this,"dimensionsZ", m_dimensions.z());
}

bool LVOX3_StepComputeLvoxGrids::restorePostSettings(SettingsReaderInterface &reader)
{
    if(!CT_AbstractStep::restorePostSettings(reader))
        return false;

    QVariant value;

    reader.parameter(this, "gridResolutionX", value);
    m_resolution = value.toDouble();

    reader.parameter(this, "gridResolutionY", value);
    m_resolution = value.toDouble();

    reader.parameter(this, "gridResolutionZ", value);
    m_resolution = value.toDouble();

    reader.parameter(this, "computeDistances", value);
    m_computeDistances = value.toBool();

    reader.parameter(this, "projectedAreaMethod", value);
    m_elementProjectedAreaMethod = static_cast<VegetationType>(value.toInt());

    reader.parameter(this, "lengthOrExtension1", value);
    m_lengthOrExtension1 = value.toDouble();
    reader.parameter(this, "diameterOrExtension2", value);
    m_diameterOrExtension2 = value.toDouble();
    reader.parameter(this, "projectedArea", value);
    m_flatProjectedArea = value.toDouble();

    reader.parameter(this,"gridReferenceMethod", value);
    m_gridMode = static_cast<GridMode>(value.toInt());

    reader.parameter(this,"coordinatesX", value);
    m_coordinates.x() = value.toDouble();
    reader.parameter(this,"coordinatesY", value);
    m_coordinates.y() = value.toDouble();
    reader.parameter(this,"coordinatesZ", value);
    m_coordinates.z() = value.toDouble();

    reader.parameter(this,"dimensionsX", value);
    m_dimensions.x() = value.toDouble();
    reader.parameter(this,"dimensionsY", value);
    m_dimensions.y() = value.toDouble();
    reader.parameter(this,"dimensionsZ", value);
    m_dimensions.z() = value.toDouble();

    return true;
}

QString LVOX3_StepComputeLvoxGrids::parametersDescription(bool defaultValue)
{
    return CT_AbstractStep::parametersDescription() +
            "<strong>" + tr("Step parameters:") + "</strong>"
            "<ul>"
                "<li>" + tr("Grids' resolution") + ": " + tr("3 double") + "<em>" + tr("default") + "</em> = (0.5, 0.5, 0.5) </li>"
                "<li>" + tr("Compute distances") + ": " + tr("boolean") + "<em>" + tr("default") + "</em> = " + tr("false") + "</li>"
                "<li>" + tr("Estimation of vegetation area method") + ": " + tr("String") + " <em>" + tr("with the following possible value") + "</em> = "
                    "<ul>"
                        "<li>" + tr("Broadleaf") + " : " + tr("Length/Diameter") + ": " + tr("2 doubles") + " <em>" + tr("default") + "</em> = " + tr("(0.0, 0.0)") + "</li>"
                        "<li>" + tr("Needle") + " : " + tr("Extension1/Extension2") + ": " + tr("2 doubles") + " <em>" + tr("default") + "</em> = " + tr("(0.0, 0.0)") + "</li>"
                        "<li>" + tr("Needle Area") + " : " +  tr("Area") + ": " + tr("double") + " <em>" + tr("default") + "</em> = 0.0" + "</li>"
                        "<li>" + tr("Broadleaf Area") + " : " +  tr("Area") + ": " + tr("double") + " <em>" + tr("default") + "</em> = 0.0</li>"
                    "</ul>"
                "</li>"
                "<li>" + tr("Grids' reference corner") + ": " + tr("String") + " <em>" + tr("with the following possible value") + "</em> = "
                    "<ul>"
                        "<li>" + tr("Scene's bounding box") + ": " + "<em>" + tr("No input required") + "</em>" + "</li>"
                        "<li>" + tr("Relative to coordinates") + " : " + tr("Coordinates") + ": " + tr("3 doubles") + " <em>" + tr("default") + "</em> = " + tr("(0.0, 0.0, 0.0)") + "</li>"
                        "<li>" + tr("Relative to following coordinates with custom dimensions") + " : " + tr("Coordinates") + ": " + tr("6 doubles") + " <em>" + tr("default") + "</em> = " + tr("(0.0, 0.0, 0.0) (0.0, 0.0, 0.0)") + "</li>"
                        "<li>" + tr("Centered on following coordinates with custom dimensions") + " : " + tr("Coordinates") + ": " + tr("6 doubles") + " <em>" + tr("default") + "</em> = " + tr("(0.0, 0.0, 0.0) (0.0, 0.0, 0.0)") + "</li>"
                        "<li>" + tr("From other grid") + ": " + "<em>" + tr("No input required") + "</em>" + "</li>"
                    "</ul>"
                "</li>"
            "</ul>";
}

QString LVOX3_StepComputeLvoxGrids::outputDescription() const
{
    auto indent = [](QString s) -> QString { return "<div style=\"margin: 0 0 0 40px;\">" + s+ "</div>"; };

    return CT_AbstractStep::outputDescription() + "<br>Group [Group]:" +
            indent("...") +
            indent("LVOX3_Grid3D<int32> [Theoretical]") +
            indent("LVOX3_Grid3D<int32> [Hits]") +
            indent("LVOX3_Grid3D<int32> [Before]");
}

bool LVOX3_StepComputeLvoxGrids::postInputConfigure()
{
    ComputeGridsConfiguration c;

    // Methods must be set first
    c.setCurrentProjectedAreaMethod(m_elementProjectedAreaMethod);
    c.setCurrentGridMode(m_gridMode);

    c.setCoordinates(m_coordinates);
    c.setDimensions(m_dimensions);
    c.setComputeDistances(m_computeDistances);
    c.setResolution(m_resolution);
    c.setFlatArea(m_flatProjectedArea);
    c.setLengthOrExtension1(m_lengthOrExtension1);
    c.setWidthOrExtension2(m_diameterOrExtension2);

    if(CT_ConfigurableWidgetToDialog::exec(&c) == QDialog::Rejected)
        return false;

    m_elementProjectedAreaMethod = c.getElementProjectedAreaMethods();
    m_gridMode = c.getGridMode();

    m_coordinates = c.getCoordinates();
    m_dimensions = c.getDimensiosn();
    m_computeDistances = c.getComputeDistances();
    m_resolution = c.getResolution();
    m_flatProjectedArea = c.getFlatArea();
    m_lengthOrExtension1 = c.getLengthOrExtension1();
    m_diameterOrExtension2 = c.getWidthOrExtension2();

    return true;
}



/**
 * @brief This method defines what kind of input the step can accept
 */
void LVOX3_StepComputeLvoxGrids::declareInputModels(CT_StepInModelStructureManager& manager)
{
    manager.addResult(_inResult, "Scene");
    manager.setZeroOrMoreRootGroup(_inResult, _inZeroOrMoreRootGroup);
    manager.addGroup(_inZeroOrMoreRootGroup, _inGroup);
    manager.addItem(_inGroup, _inScene, "Scene");


    //manager.addItem(_inGroup, _inScan, tr("Scanner"), tr("The scanner is used to simulate the shooting pattern. If it was not present choose at least a shooting pattern !"), CT_InAbstractModel::C_ChooseOneIfMultiple, CT_InAbstractModel::F_IsOptional);
    manager.addItem(_inGroup, _inScan, tr("Scanner"),
                    tr("The scanner is used to simulate the shooting pattern. If it was not present choose at least a shooting pattern !"));
    manager.addItem(_inGroup, _inShootingPattern, tr("Shooting pattern"),
                    tr("The shooting pattern is used to simulate it. If it was not present choose at least a scanner !"));

}


/**
 * @brief This method defines what kind of output the step produces
 */
void LVOX3_StepComputeLvoxGrids::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    manager.addResultCopy(_inResult);
    manager.addItem(_inGroup, _outHits, "Hits");
    manager.addItem(_inGroup, _outTheo, "Theoretical");
    manager.addItem(_inGroup, _outBef, "Before");
}

/**
 * @brief This method does the job
 * It creates three 3D grids from point cloud information and user defined voxel size (Hits, Theoritical, and Before Rays)
 */
void LVOX3_StepComputeLvoxGrids::compute()
{

    PS_LOG->addInfoMessage(LogInterface::step, tr("Input coord %1 %2 ; Input dimension %3 %4 %5")
        .arg(m_coordinates.x())
        .arg(m_coordinates.z())
        .arg(m_dimensions.x())
        .arg(m_dimensions.y())
        .arg(m_dimensions.z()));

    for (CT_StandardItemGroup* group : _inGroup.iterateOutputs(_inResult))
    {
        const CT_Scene* scene = group->singularItem(_inScene);
        const CT_Scanner* scanner = group->singularItem(_inScan);
        const CT_ShootingPattern* pattern = scanner->shootingPattern();

        if (!scene || !scanner)
            continue;

        Eigen::AlignedBox3d sceneBBox;
        Eigen::AlignedBox3d scannerBBox;
        Eigen::AlignedBox3d voxelsBBox;

        /* Union of all scanners
         *
         * FIXME: center coordinate may not be enough to compute the
         * bounding box of the parallel shooting pattern.
         */
        const Eigen::Vector3d& scanPos = pattern->centerCoordinate();
        scannerBBox.extend(scanPos);

        qDebug() << "ID " << scanner->id() << " Pos : ("  << scanPos.x() << "," << scanPos.y() << "," << scanPos.z() << ")";

        /* Union of all scenes */
        Eigen::Vector3d sceneBBOXMin, sceneBBOXMax;
        scene->boundingBox(sceneBBOXMin, sceneBBOXMax);
        sceneBBox.extend(sceneBBOXMin);
        sceneBBox.extend(sceneBBOXMax);

        PS_LOG->addInfoMessage(LogInterface::trace, "Mode " + lvox::gridModeToString(m_gridMode));

        //Grid mode, default or custom mode
        switch(m_gridMode) {
        case lvox::BoundingBoxOfTheScene:
            voxelsBBox.extend(sceneBBox);
            break;
        case lvox::RelativeToCoordinates:
            voxelsBBox.extend(m_coordinates);
            voxelsBBox.extend(sceneBBox.max());
            break;
        case lvox::RelativeToCoordinatesAndCustomDimensions:
        case lvox::FromOtherGrid:
            // custom bounding box, where useless voxels are removed
            voxelsBBox.extend(m_coordinates);
            voxelsBBox.extend(m_coordinates + m_dimensions);
            voxelsBBox.clamp(sceneBBox);
            break;
        case lvox::CenteredOnCoordinatesAndCustomDimensions:
            voxelsBBox.extend(m_coordinates - m_dimensions);
            voxelsBBox.extend(m_coordinates + m_dimensions);
            break;
        default:
            break;
        }

        // extends the bounding box to include the scanner
        if (!voxelsBBox.contains(scannerBBox)) {
            PS_LOG->addMessage(LogInterface::warning, LogInterface::step,
                               QObject::tr("Grid dimensions has been enlarged to include scanner and/or their range"));
            //TEMPORARY FIX BY FP 11/2017 TO AVOID THAT THE GRID CENTER IS SHIFTED WHEN SCANNER ARE NOT IN THE GRID
            if (m_gridMode==lvox::CenteredOnCoordinatesAndCustomDimensions) {
                Eigen::Vector3d v1s = scannerBBox.min().array();
                Eigen::Vector3d v2s = scannerBBox.max().array();
                Eigen::Vector3d v1 = voxelsBBox.min().array();
                Eigen::Vector3d v2 = voxelsBBox.max().array();
                while(v1(0) > v1s(0)) {v1(0) -= m_resolution;}
                while(v1(1) > v1s(1)) {v1(1) -= m_resolution;}
                while(v1(2) > v1s(2)) {v1(2) -= m_resolution;}
                while(v2(0) < v2s(0)) {v2(0) += m_resolution;}
                while(v2(1) < v2s(1)) {v2(1) += m_resolution;}
                while(v2(2) < v2s(2)) {v2(2) += m_resolution;}
                voxelsBBox.extend(v1);
                voxelsBBox.extend(v2);
            } else {
                voxelsBBox.extend(scannerBBox);
            }
        }

        //setting response dimensions
        const Eigen::Vector3d minBBox = voxelsBBox.min();
        const Eigen::Vector3d maxBBox = voxelsBBox.max();

        PS_LOG->addMessage(LogInterface::info, LogInterface::step,
                           QObject::tr("Voxel bounding box: (%1,%2,%3), (%4,%5,%6)")
                            .arg(minBBox.x(), 5, 'f', 1)
                            .arg(minBBox.y(), 5, 'f', 1)
                            .arg(minBBox.z(), 5, 'f', 1)
                            .arg(maxBBox.x(), 5, 'f', 1)
                            .arg(maxBBox.y(), 5, 'f', 1)
                            .arg(maxBBox.z(), 5, 'f', 1)
                           );

        LVOX3_ComputeAll workersManager; //Worker manager (Thread and progress manager)

        lvox::Grid3Di* hitGrid = lvox::Grid3Di::createGrid3DFromXYZCoords(minBBox.x(), minBBox.y(), minBBox.z(),
                                                           maxBBox.x(), maxBBox.y(), maxBBox.z(),
                                                           m_resolution, -9, 0, false);
        lvox::Grid3Di* theoriticalGrid = new lvox::Grid3Di(hitGrid->minX(), hitGrid->minY(), hitGrid->minZ(), hitGrid->xdim(),
                                            hitGrid->ydim(), hitGrid->zdim(), m_resolution,
                                            -9, 0);
        lvox::Grid3Di* beforeGrid =  new lvox::Grid3Di(hitGrid->minX(), hitGrid->minY(), hitGrid->minZ(), hitGrid->xdim(),
                                       hitGrid->ydim(), hitGrid->zdim(), m_resolution,
                                       -9, 0);

        //Added to the group for UI drawing purposes
        group->addSingularItem(_outHits, hitGrid);
        group->addSingularItem(_outTheo, theoriticalGrid);
        group->addSingularItem(_outBef, beforeGrid);

        // and if must compute distance, grids with distance
        if (m_computeDistances)
        {
            //Creating the grids for mean distances
            const double elementProjectedArea = computeElementProjectedArea();
            hitGrid->setLambda1(elementProjectedArea);
            theoriticalGrid->setLambda1(elementProjectedArea);
            beforeGrid->setLambda1(elementProjectedArea);
        } else
        {
            PS_LOG->addWarningMessage(LogInterface::step, tr("PAD computation will produce invalid results if called without computing distances"));
        }

        qDebug() << "Shooting pattern : " << scene->id() << " " << pattern->numberOfShots();


        LVOX3_ComputeTheoriticals* theoriticalWorker = new LVOX3_ComputeTheoriticals(pattern, theoriticalGrid, m_computeDistances);
        LVOX3_ComputeHits* hitsWorker =  new LVOX3_ComputeHits(pattern, scene->pointCloudIndex(), hitGrid, m_computeDistances);
        LVOX3_ComputeBefore* beforeWorker = new LVOX3_ComputeBefore(pattern, scene->pointCloudIndex(), beforeGrid, m_computeDistances);

        //Added to manager for progress bar and mutex multithreading
        workersManager.addWorker(1, beforeWorker);
        workersManager.addWorker(1, hitsWorker);
        workersManager.addWorker(1, theoriticalWorker);

        //Connected to show progress of the workers
        connect(&workersManager, SIGNAL(progressChanged(int)), this, SLOT(progressChanged(int)), Qt::DirectConnection);
        connect(this, SIGNAL(stopped()), &workersManager, SLOT(cancel()), Qt::DirectConnection);
        workersManager.compute();
    }

}

/**
 * @brief Called from worker manager when progress changed
 */
void LVOX3_StepComputeLvoxGrids::progressChanged(int p)
{
    setProgress(p);
}

/**
 * @brief LVOX3_StepComputeLvoxGrids::computeElementProjectedArea
 * @return this function computes the random projected area of vegetation elements.
 * which is equal to lambda1*resx*resy*resz
 * This can be computed according to four different methods
 *
 */
float LVOX3_StepComputeLvoxGrids::computeElementProjectedArea()
{
    switch (m_elementProjectedAreaMethod) {
    case ComputeGridsConfiguration::NeedleFromDimension:
        return (2 * M_PI * (m_lengthOrExtension1/100) * (m_diameterOrExtension2/100)/2) / 4;//= (2pi *  lenght * radius) / 4
    case ComputeGridsConfiguration::BroadLeafFromDimension:
        return (2 * M_PI * pow((m_lengthOrExtension1/100),2)) / 4;//= (2pi * radius^2) / 4
    case ComputeGridsConfiguration::NeedleFromFlatArea:
        return (M_PI *(m_flatProjectedArea/10000)) / 4;//= (pi*flatarea) / 4
    case ComputeGridsConfiguration::BroadLeafFromFlatArea:
        return (2 *(m_flatProjectedArea/10000)) / 4;//= (2halfarea) / 4
    default:
        #ifdef __GNUC__
            __builtin_unreachable();
        #elif defined(_MSC_VER)
            __assume(false);
        #endif
    }
}

