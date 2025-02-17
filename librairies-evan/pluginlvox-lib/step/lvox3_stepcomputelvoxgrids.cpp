#include "lvox3_stepcomputelvoxgrids.h"

// Workers
#include "datastructures/lvox3_implgrid3d.h"
#include "datastructures/lvox3_implpointcloud.h"
#include "datastructures/lvox3_implshootingpattern.h"

#include "tools/lvox3_gridtype.h"

#include "tools/lvox3_logger.h"

// Tools
#include <math.h>

// Core
#include <ct_log/ct_logmanager.h>
#include <ct_view/tools/ct_configurablewidgettodialog.h>

#include <liblvox/algorithms/computelvoxgrids.h>

/**
 * @brief Compute the LVOX Ni, Nb, Nt grids
 */
LVOX3_StepComputeLvoxGrids::LVOX3_StepComputeLvoxGrids() : SuperClass() {
  m_resolution.x() = 0.5;
  m_resolution.y() = 0.5;
  m_resolution.z() = 0.5;
  m_computeDistances = false;
  //_onlyHits = false;

  m_gridMode = lvox::ComputeLvoxGrids::GridMode::BoundingBoxOfTheScene;
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

  //_elementProjectedAreaMethodCollection.insert(tr("Needle (from Length and
  //Diameter)"), VegetationType::NeedleFromDimension);
  //_elementProjectedAreaMethodCollection.insert(tr("BroadLeaf (from Extension1
  //and Extension2)"), VegetationType::BroadLeafFromDimension);
  //_elementProjectedAreaMethodCollection.insert(tr("Needle Area (from flat
  //scan)"), VegetationType::NeedleFromFlatArea);
  //_elementProjectedAreaMethodCollection.insert(tr("BroadLead Area (from flat
  //scan)"), VegetationType::BroadLeafFromFlatArea);

  //_gridModeCollection.insert(tr("Scene's bounding box"),
  //lvox::GridMode::BoundingBoxOfTheScene);
  //_gridModeCollection.insert(tr("Relative to coordinates"),
  //lvox::GridMode::RelativeToCoordinates);
  //_gridModeCollection.insert(tr("Relative to following coordinates with custom
  //dimensions"), lvox::GridMode::RelativeToCoordinatesAndCustomDimensions);
  //_gridModeCollection.insert(tr("Centered on following coordinates with custom
  //dimensions"), lvox::GridMode::CenteredOnCoordinatesAndCustomDimensions);
  //_gridModeCollection.insert(tr("From other grid"),
  //lvox::GridMode::FromOtherGrid);

  _confidenceLevelCollection.insert(tr("0,5"), 0.5);
  _confidenceLevelCollection.insert(tr("0,9"), 0.9);
  _confidenceLevelCollection.insert(tr("0,95"), 0.95);
}

/**
 * @brief Return a short description of what this class does
 */
QString LVOX3_StepComputeLvoxGrids::description() const {
  return tr("2 - Compute LVOX grids (LVOX 3)");
}

/*! \brief Step detailed description
 *
 * Return a detailed description of the step function
 */
QString LVOX3_StepComputeLvoxGrids::detailledDescription() const {
  return tr("This step creates 3D grids for theoretical rays, actual hits rays "
            "and intercepted rays given a predefined voxel size.") +
         "<br><br>" +
         tr("All grids are created with the specified resolution.") +
         "<br>"
         "<em>" +
         tr("Note: ") + "</em>" +
         tr("Computing distances is mandatory in order to perform PAD "
            "computation");
}

/**
 * @brief Return a new empty instance of this class
 */
CT_VirtualAbstractStep *LVOX3_StepComputeLvoxGrids::createNewInstance() const {
  // Creates an instance of this step
  // LVOX3_StepComputeLvoxGrids* step = new LVOX3_StepComputeLvoxGrids();
  // return dynamic_cast<CT_VirtualAbstractStep*> (step);
  return new LVOX3_StepComputeLvoxGrids();
}

void LVOX3_StepComputeLvoxGrids::savePostSettings(
    SettingsWriterInterface &writer) const {
  CT_AbstractStep::savePostSettings(writer);

  writer.addParameter(this, "gridResolutionX", m_resolution.x());
  writer.addParameter(this, "gridResolutionY", m_resolution.y());
  writer.addParameter(this, "gridResolutionZ", m_resolution.z());
  writer.addParameter(this, "computeDistances", m_computeDistances);

  writer.addParameter(this, "projectedAreaMethod",
                      m_elementProjectedAreaMethod);
  writer.addParameter(this, "lengthOrExtension1", m_lengthOrExtension1);
  writer.addParameter(this, "diameterOrExtension2", m_diameterOrExtension2);
  writer.addParameter(this, "projectedArea", m_flatProjectedArea);

  writer.addParameter(this, "gridReferenceMethod", static_cast<uint32_t>(m_gridMode));
  writer.addParameter(this, "coordinatesX", m_coordinates.x());
  writer.addParameter(this, "coordinatesY", m_coordinates.y());
  writer.addParameter(this, "coordinatesZ", m_coordinates.z());

  writer.addParameter(this, "dimensionsX", m_dimensions.x());
  writer.addParameter(this, "dimensionsY", m_dimensions.y());
  writer.addParameter(this, "dimensionsZ", m_dimensions.z());
}

bool LVOX3_StepComputeLvoxGrids::restorePostSettings(
    SettingsReaderInterface &reader) {
  if (!CT_AbstractStep::restorePostSettings(reader))
    return false;

  QVariant value;

  reader.parameter(this, "gridResolutionX", value);
  m_resolution.x() = value.toDouble();

  reader.parameter(this, "gridResolutionY", value);
  m_resolution.y() = value.toDouble();

  reader.parameter(this, "gridResolutionZ", value);
  m_resolution.z() = value.toDouble();

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

  reader.parameter(this, "gridReferenceMethod", value);
  m_gridMode = static_cast<GridMode>(value.toInt());

  reader.parameter(this, "coordinatesX", value);
  m_coordinates.x() = value.toDouble();
  reader.parameter(this, "coordinatesY", value);
  m_coordinates.y() = value.toDouble();
  reader.parameter(this, "coordinatesZ", value);
  m_coordinates.z() = value.toDouble();

  reader.parameter(this, "dimensionsX", value);
  m_dimensions.x() = value.toDouble();
  reader.parameter(this, "dimensionsY", value);
  m_dimensions.y() = value.toDouble();
  reader.parameter(this, "dimensionsZ", value);
  m_dimensions.z() = value.toDouble();

  return true;
}

QString LVOX3_StepComputeLvoxGrids::parametersDescription() {
  return CT_AbstractStep::parametersDescription() + "<strong>" +
         tr("Step parameters:") +
         "</strong>"
         "<ul>"
         "<li>" +
         tr("Grids' resolution") + ": " + tr("3 double") + "<em>" +
         tr("default") +
         "</em> = (0.5, 0.5, 0.5) </li>"
         "<li>" +
         tr("Compute distances") + ": " + tr("boolean") + "<em>" +
         tr("default") + "</em> = " + tr("false") +
         "</li>"
         "<li>" +
         tr("Estimation of vegetation area method") + ": " + tr("String") +
         " <em>" + tr("with the following possible value") +
         "</em> = "
         "<ul>"
         "<li>" +
         tr("Broadleaf") + " : " + tr("Length/Diameter") + ": " +
         tr("2 doubles") + " <em>" + tr("default") +
         "</em> = " + tr("(0.0, 0.0)") +
         "</li>"
         "<li>" +
         tr("Needle") + " : " + tr("Extension1/Extension2") + ": " +
         tr("2 doubles") + " <em>" + tr("default") +
         "</em> = " + tr("(0.0, 0.0)") +
         "</li>"
         "<li>" +
         tr("Needle Area") + " : " + tr("Area") + ": " + tr("double") +
         " <em>" + tr("default") + "</em> = 0.0" +
         "</li>"
         "<li>" +
         tr("Broadleaf Area") + " : " + tr("Area") + ": " + tr("double") +
         " <em>" + tr("default") +
         "</em> = 0.0</li>"
         "</ul>"
         "</li>"
         "<li>" +
         tr("Grids' reference corner") + ": " + tr("String") + " <em>" +
         tr("with the following possible value") +
         "</em> = "
         "<ul>"
         "<li>" +
         tr("Scene's bounding box") + ": " + "<em>" + tr("No input required") +
         "</em>" +
         "</li>"
         "<li>" +
         tr("Relative to coordinates") + " : " + tr("Coordinates") + ": " +
         tr("3 doubles") + " <em>" + tr("default") +
         "</em> = " + tr("(0.0, 0.0, 0.0)") +
         "</li>"
         "<li>" +
         tr("Relative to following coordinates with custom dimensions") +
         " : " + tr("Coordinates") + ": " + tr("6 doubles") + " <em>" +
         tr("default") + "</em> = " + tr("(0.0, 0.0, 0.0) (0.0, 0.0, 0.0)") +
         "</li>"
         "<li>" +
         tr("Centered on following coordinates with custom dimensions") +
         " : " + tr("Coordinates") + ": " + tr("6 doubles") + " <em>" +
         tr("default") + "</em> = " + tr("(0.0, 0.0, 0.0) (0.0, 0.0, 0.0)") +
         "</li>"
         "<li>" +
         tr("From other grid") + ": " + "<em>" + tr("No input required") +
         "</em>" +
         "</li>"
         "</ul>"
         "</li>"
         "</ul>";
}

QString LVOX3_StepComputeLvoxGrids::outputDescription() const {
  auto indent = [](QString s) -> QString {
    return "<div style=\"margin: 0 0 0 40px;\">" + s + "</div>";
  };

  return CT_AbstractStep::outputDescription() +
         "<br>Group [Group]:" + indent("...") +
         indent("LVOX3_Grid3D<int32> [Theoretical]") +
         indent("LVOX3_Grid3D<int32> [Hits]") +
         indent("LVOX3_Grid3D<int32> [Before]");
}

bool LVOX3_StepComputeLvoxGrids::postInputConfigure() {
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

  if (CT_ConfigurableWidgetToDialog::exec(&c) == QDialog::Rejected)
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
void LVOX3_StepComputeLvoxGrids::declareInputModels(
    CT_StepInModelStructureManager &manager) {
  manager.addResult(_inResult, "Scene");
  manager.setZeroOrMoreRootGroup(_inResult, _inZeroOrMoreRootGroup);
  manager.addGroup(_inZeroOrMoreRootGroup, _inGroup);
  manager.addItem(_inGroup, _inScene, "Scene");

  // manager.addItem(_inGroup, _inScan, tr("Scanner"), tr("The scanner is used
  // to simulate the shooting pattern. If it was not present choose at least a
  // shooting pattern !"), CT_InAbstractModel::C_ChooseOneIfMultiple,
  // CT_InAbstractModel::F_IsOptional);
  manager.addItem(
      _inGroup, _inScan, tr("Scanner"),
      tr("The scanner is used to simulate the shooting pattern. If it was not "
         "present choose at least a shooting pattern !"));
  manager.addItem(_inGroup, _inShootingPattern, tr("Shooting pattern"),
                  tr("The shooting pattern is used to simulate it. If it was "
                     "not present choose at least a scanner !"));
}

/**
 * @brief This method defines what kind of output the step produces
 */
void LVOX3_StepComputeLvoxGrids::declareOutputModels(
    CT_StepOutModelStructureManager &manager) {
  manager.addResultCopy(_inResult);
  manager.addItem(_inGroup, _outHits, "Hits");
  manager.addItem(_inGroup, _outRefHits, "RefHits");
  manager.addItem(_inGroup, _outTheo, "Theoretical");
  manager.addItem(_inGroup, _outBef, "Before");
}

/**
 * @brief This method does the job
 * It creates three 3D grids from point cloud information and user defined voxel
 * size (Hits, Theoritical, and Before Rays)
 */
void LVOX3_StepComputeLvoxGrids::compute()
{

    LVOX3_Logger logger;

    struct GroupedData {
        GroupedData(CT_StandardItemGroup* group,
                LVOX3_ImplGrid3d<int>* hitGrid,
                LVOX3_ImplGrid3d<int>* beforeGrid,
                LVOX3_ImplGrid3d<int>* theoGrid):
            group(group),
            hitGrid(hitGrid),
            beforeGrid(beforeGrid),
            theoGrid(theoGrid) {}
        CT_StandardItemGroup* group;
        LVOX3_ImplGrid3d<int>* hitGrid;
        LVOX3_ImplGrid3d<int>* beforeGrid;
        LVOX3_ImplGrid3d<int>* theoGrid;
    };



    std::vector<std::pair<CT_StandardItemGroup*, lvox::ComputeLvoxGrids::ItemPreparator>> groupedPreparator;

    for (CT_StandardItemGroup* group : _inGroup.iterateOutputs(_inResult))
    {
        // Extracting what's needed from the group
        const CT_Scene* scene = (const CT_Scene*)group->singularItem(_inScene);
        const CT_Scanner* scanner = (const CT_Scanner*)group->singularItem(_inScan);
        const CT_ShootingPattern* pattern = scanner->shootingPattern();

        logger.logDebug(std::to_string(group->id()) + " " +
                        std::to_string(pattern->centerCoordinate().x()) + " " + std::to_string(pattern->centerCoordinate().y()) + " " + std::to_string(pattern->centerCoordinate().z()));

        LVOX3_ImplShootingPattern* lib_shootingPattern = new LVOX3_ImplShootingPattern{const_cast<CT_ShootingPattern*>(pattern)};
        LVOX3_ImplPointCloud* lib_pcd = new LVOX3_ImplPointCloud(scene);
        //LVOX3_ImplScanner* lib_scanner = new LVOX3_ImplScanner(const_cast<CT_Scanner*>(scanner));


        lvox::ComputeLvoxGrids::ItemPreparator ip { lib_pcd, lib_shootingPattern};

        groupedPreparator.push_back({group, ip});
    }



    lvox::ComputeLvoxGrids::Settings settings;
    settings.GridMode = m_gridMode;
    settings.GridResolution = m_resolution.x();
    settings.Dimension = m_dimensions;
    settings.MustComputeDistances = m_computeDistances;
    settings.Position = m_coordinates;
    if(m_computeDistances)
    {
        switch (m_elementProjectedAreaMethod) {
        case ComputeGridsConfiguration::NeedleFromDimension:
            settings.VegetationType = lvox::ComputeLvoxGrids::NeedleFromDimension;
            settings.VegetationSettings.Length = m_lengthOrExtension1;
            settings.VegetationSettings.Diameter = m_diameterOrExtension2;
            break;
        case ComputeGridsConfiguration::BroadLeafFromDimension:
            settings.VegetationType = lvox::ComputeLvoxGrids::BroadLeafFromDimension;
            settings.VegetationSettings.Extension1 = m_lengthOrExtension1;
            settings.VegetationSettings.Extension2 = m_diameterOrExtension2;
            break;
        case ComputeGridsConfiguration::NeedleFromFlatArea:
            settings.VegetationType = lvox::ComputeLvoxGrids::NeedleFromFlatArea;
            settings.VegetationSettings.FlatProjectedArea = m_flatProjectedArea;
            break;
        case ComputeGridsConfiguration::BroadLeafFromFlatArea:
            settings.VegetationType = lvox::ComputeLvoxGrids::BroadLeafFromFlatArea;
            settings.VegetationSettings.FlatProjectedArea = m_flatProjectedArea;
            break;
        }
    }

    std::vector<lvox::ComputeLvoxGrids::ItemPreparator> extractedValues;

    std::transform(groupedPreparator.begin(), groupedPreparator.end(), std::back_inserter(extractedValues),
                   [](const std::pair<CT_StandardItemGroup*, lvox::ComputeLvoxGrids::ItemPreparator>& pair) { return pair.second; });

    Eigen::Vector3d minVoxelGridBox, maxVoxelGridBox;
    lvox::ComputeLvoxGrids algo(extractedValues, settings, &logger);

    algo.getInitBoundingBox(minVoxelGridBox, maxVoxelGridBox);
    qDebug() << minVoxelGridBox(0) << " " << minVoxelGridBox(1) << " " << minVoxelGridBox(2) << " " <<
                maxVoxelGridBox(0) << " " << maxVoxelGridBox(1) << " " << maxVoxelGridBox(2);
    std::vector<lvox::ComputeLvoxGrids::ComputeItem> vs;
    std::vector<pluginlvox::Grid3Di*> minMaxs;
    for(auto& gp : groupedPreparator)
    {
        pluginlvox::Grid3Di* before = pluginlvox::Grid3Di::createGrid3DFromXYZCoords(minVoxelGridBox(0), minVoxelGridBox(1), minVoxelGridBox(2),
                                                                                         maxVoxelGridBox(0), maxVoxelGridBox(1), maxVoxelGridBox(2),
                                                                                         settings.GridResolution, settings.GridResolution, settings.GridResolution, -9, 0);
        minMaxs.push_back(before);
        pluginlvox::Grid3Di* hit = new pluginlvox::Grid3Di{*before};
        minMaxs.push_back(hit);
        pluginlvox::Grid3Di* theo = new pluginlvox::Grid3Di{*before};
        minMaxs.push_back(theo);

        gp.first->addSingularItem(_outHits, hit);
        gp.first->addSingularItem(_outTheo, theo);
        gp.first->addSingularItem(_outBef, before);

        CT_Grid3D<int>* refhit = new CT_Grid3D<int>{minVoxelGridBox(0), minVoxelGridBox(1), minVoxelGridBox(2),
                maxVoxelGridBox(0), maxVoxelGridBox(1), maxVoxelGridBox(2),
                settings.GridResolution, -9, 0};
        gp.first->addSingularItem(_outRefHits, refhit);

        size_t size = gp.second.PointCloudScene->getSize();
        logger.logInfo(std::to_string(size));

        for (size_t i = 0; i < size; i++)
        {
            Eigen::Vector3d p = gp.second.PointCloudScene->getPointByIndex(i);
            refhit->addValueAtXYZ(p.x(), p.y(), p.z(), 1);
        }
        refhit->computeMinMax();


        LVOX3_ImplGrid3d<int>* hitGrid = new LVOX3_ImplGrid3d(hit);
        LVOX3_ImplGrid3d<int>* beforeGrid = new LVOX3_ImplGrid3d(before);
        LVOX3_ImplGrid3d<int>* theoGrid = new LVOX3_ImplGrid3d(theo);

        lvox::ComputeLvoxGrids::ComputeItem ci {gp.second.PointCloudScene,
                                                gp.second.ShootingPattern,
                                                hitGrid,
                                                theoGrid,
                                                beforeGrid };
        vs.push_back(ci);
    }


    algo.compute(vs);

    for(const auto& item : minMaxs)
    {
        item->computeMinMax();
    }
}

/**
 * @brief Called from worker manager when progress changed
 */
void LVOX3_StepComputeLvoxGrids::progressChanged(int p) { setProgress(p); }

