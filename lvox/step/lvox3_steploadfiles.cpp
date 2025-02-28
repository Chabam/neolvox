#include "lvox3_steploadfiles.h"

//Tools
#include "ct_itemdrawable/tools/scanner/ct_parallelshootingpatternfrompointcloud.h"
#include "ct_itemdrawable/tools/scanner/ct_shootingpatternfrompointcloud.h"
#include "ct_itemdrawable/tools/scanner/ct_thetaphishootingpattern.h"
#include "ct_log/ct_logmanager.h"
#include "lvox_steppluginmanager.h"
#include "tools/lvox3_scannerutils.h"
#include "ct_view/tools/ct_configurablewidgettodialog.h"
#include "ct_reader/extensions/ct_readerpointsfilteringextension.h"

//Models
#define DEF_outResult                "LVOX3_SLF_result"
#define DEF_outGroup                 "LVOX3_SLF_group"
#define DEF_outScannerForced         "LVOX3_SLF_scanner"


LVOX3_StepLoadFiles::LVOX3_StepLoadFiles() : CT_AbstractStep(), m_reader(nullptr)
{
    m_useUserScannerConfiguration = false;
    m_filterPointsOrigin = true;
    m_restrictScene = false;
    m_restrictRadius = 0;
}

LVOX3_StepLoadFiles::~LVOX3_StepLoadFiles()
{
    delete m_reader;
}

QString LVOX3_StepLoadFiles::description() const
{
    return tr("1 - Load files for LV3");
}

// Step detailed description
QString LVOX3_StepLoadFiles::detailledDescription() const
{
    return tr("This step allows loading files with Computree's readers and subsquently creating voxel grids from them.");
}

CT_VirtualAbstractStep* LVOX3_StepLoadFiles::createNewInstance() const
{
    // Creates an instance of this step
    return new LVOX3_StepLoadFiles();
}

void LVOX3_StepLoadFiles::savePostSettings(SettingsWriterInterface &writer) const
{
    CT_AbstractStep::savePostSettings(writer);

    if (m_reader != nullptr)
    {
        writer.addParameter(this, "ReaderUniqueName", m_reader->uniqueName());
        m_reader->saveSettings(writer);
    }

    writer.addParameter(this, "minuseScannerConfigurationValue", m_useUserScannerConfiguration);
    writer.addParameter(this, "filterPointsOrigin", m_filterPointsOrigin);
    writer.addParameter(this, "restrictScene", m_restrictScene);
    writer.addParameter(this, "restrictRadius", m_restrictRadius);


    int n = m_configuration.size();
    writer.addParameter(this, "nConfigurations", n);

    for(int i = 0; i < n ; ++i)
    {
        const LoadFileConfiguration::Configuration& c = m_configuration.at(i);
        const auto scanDef = LVOX3_ScannerUtils::getScannerDefinition(c.scannerId);

        writer.addParameterPath(this, "filepath", c.filepath);
        writer.addParameter(this, "scannerType", scanDef.label);
        writer.addParameter(this, "clockWise", c.clockWise);
        writer.addParameter(this, "radians", c.radians);
        writer.addParameter(this, "scannerPositionX", c.scannerPosition.x());
        writer.addParameter(this, "scannerPositionY", c.scannerPosition.y());
        writer.addParameter(this, "scannerPositionZ", c.scannerPosition.z());
        writer.addParameter(this, "scannerResolutionX", c.scannerResolution.x());
        writer.addParameter(this, "scannerResolutionY", c.scannerResolution.y());
        writer.addParameter(this, "scannerThetaStart", c.scannerThetaRange.x());
        writer.addParameter(this, "scannerThetaEnd", c.scannerThetaRange.y());
        writer.addParameter(this, "scannerPhiStart", c.scannerPhiRange.x());
        writer.addParameter(this, "scannerPhiEnd", c.scannerPhiRange.y());
        writer.addParameter(this, "scannerDirectionX", c.scannerDirection.x());
        writer.addParameter(this, "scannerDirectionY", c.scannerDirection.y());
        writer.addParameter(this, "scannerDirectionZ", c.scannerDirection.z());
    }
}

bool LVOX3_StepLoadFiles::restorePostSettings(SettingsReaderInterface &reader)
{
    if(!CT_AbstractStep::restorePostSettings(reader))
        return false;

    QVariant value;

    if(!reader.parameter(this, "ReaderUniqueName", value))
        return false;

    QString readerUniqueName = value.toString();
    CT_AbstractReader* fReader = pluginStaticCastT<LVOX_StepPluginManager>()->readerAvailableByUniqueName(readerUniqueName);

    if(fReader == nullptr)
        return false;

    if (m_reader != nullptr) {delete m_reader;}
    m_reader = fReader->copyFull();

    if (!m_reader->restoreSettings(reader)) {return false;}


    reader.parameter(this, "minuseScannerConfigurationValue", value); m_useUserScannerConfiguration = value.toBool();
    reader.parameter(this, "filterPointsOrigin", value); m_filterPointsOrigin = value.toBool();
    reader.parameter(this, "restrictScene", value); m_restrictScene = value.toBool();
    reader.parameter(this, "restrictRadius", value); m_restrictRadius = value.toInt();

    reader.parameter(this, "nConfigurations", value);
    int n = value.toInt();

    m_configuration.clear();

    for(int i = 0; i < n ; ++i)
    {
        LoadFileConfiguration::Configuration c;
        QString scannerType;

        reader.parameterPath(this, "filepath", c.filepath);
        reader.parameter(this, "scannerType", value); scannerType = value.toString();
        reader.parameter(this, "clockWise", value); c.clockWise = value.toBool();
        reader.parameter(this, "radians", value); c.radians = value.toBool();
        reader.parameter(this, "scannerPositionX", value); c.scannerPosition.x() = value.toDouble();
        reader.parameter(this, "scannerPositionY", value); c.scannerPosition.y() = value.toDouble();
        reader.parameter(this, "scannerPositionZ", value); c.scannerPosition.z() = value.toDouble();
        reader.parameter(this, "scannerResolutionX", value); c.scannerResolution.x() = value.toDouble();
        reader.parameter(this, "scannerResolutionY", value); c.scannerResolution.y() = value.toDouble();
        reader.parameter(this, "scannerThetaStart", value); c.scannerThetaRange.x() = value.toDouble();
        reader.parameter(this, "scannerThetaEnd", value); c.scannerThetaRange.y() = value.toDouble();
        reader.parameter(this, "scannerPhiStart", value); c.scannerPhiRange.x() = value.toDouble();
        reader.parameter(this, "scannerPhiEnd", value); c.scannerPhiRange.y() = value.toDouble();
        reader.parameter(this, "scannerDirectionX", value); c.scannerDirection.x() = value.toDouble();
        reader.parameter(this, "scannerDirectionY", value); c.scannerDirection.y() = value.toDouble();
        reader.parameter(this, "scannerDirectionZ", value); c.scannerDirection.z() = value.toDouble();

        const auto scanDef = LVOX3_ScannerUtils::getScannerDefinition(scannerType);
        c.scannerId = scanDef.id;

        m_configuration.append(c);
    }

    return true;
}


CT_ShootingPattern *LVOX3_StepLoadFiles::makeShootingPattern(
        const LoadFileConfiguration::Configuration &conf, CT_PCIR pcir)
{
    /* TODO: Move this factory to the CT_Scanner class. Requires to move the
     * Configuration class used by the UI (simplifies passing arguments).
     */
    CT_ShootingPattern *pattern = nullptr;
    double hFov = 0.0;
    double vFov = 0.0;

    switch (conf.scannerId) {
    case ScannerSphericPointCloud:
    {
        return new CT_ShootingPatternFromPointCloud(conf.scannerPosition, pcir);
        break;
    }
    case ScannerPlanePointCloud:
        return new CT_ParallelShootingPatternFromPointCloud(conf.scannerPosition,
                                                            conf.scannerDirection,
                                                            pcir);
        break;
    case ScannerSphericTheoreticScanner:
        // TODO : zVector !
        hFov = conf.scannerThetaRange.y() - conf.scannerThetaRange.x();
        vFov = conf.scannerPhiRange.y() - conf.scannerPhiRange.x();
        pattern = new CT_ThetaPhiShootingPattern(conf.scannerPosition,
                                                    hFov, vFov,
                                                    conf.scannerResolution.x(),
                                                    conf.scannerResolution.y(),
                                                    conf.scannerThetaRange.x(),
                                                    conf.scannerPhiRange.x(),
                                                    Eigen::Vector3d(0, 0, 1),
                                                    conf.clockWise,
                                                    conf.radians);
        break;
    case ScannerSphericTheoreticCustom:
    {
        // TODO : zVector !
        hFov = conf.scannerThetaRange.y() - conf.scannerThetaRange.x();
        vFov = conf.scannerPhiRange.y() - conf.scannerPhiRange.x();
        pattern = new CT_ThetaPhiShootingPattern(conf.scannerPosition,
                                                    hFov, vFov,
                                                    conf.scannerResolution.x(),
                                                    conf.scannerResolution.y(),
                                                    conf.scannerThetaRange.x(),
                                                    conf.scannerPhiRange.x(),
                                                    Eigen::Vector3d(0, 0, 1),
                                                    conf.clockWise,
                                                    conf.radians);
        break;
    }
    default:
        break;
    }
    return pattern;
}

QString LVOX3_StepLoadFiles::parametersDescription()
{
    return CT_AbstractStep::parametersDescription() +
            "<strong>" + tr("Step parameters:") + "</strong>" +
            "<ul>" +
                "<li>" + tr("Reader") + ": " + tr("Reader") + " <em>" + tr("default") + "</em> = CT_Reader_XYB" + "</li>" +
                "<li>" + tr("Delete points with coordinates (0,0,0)") + ": " + tr("boolean") + " <em>" + tr("default") + "</em> = " + tr("true") + "</li>" +
                "<li>" + tr("Restrict scene") + ": " + tr("boolean") + " <em>" + tr("default") + "</em> = " + tr("false") + "</li>" +
                "<li>" + tr("Restrict scene radius") + ": " + tr("integer") + " <em>" + tr("default") + "</em> = " + "0" + "</li>" +
                "<li>" + tr("Files") + ": " + tr("List of paths") + " <em> "+ tr("for each paths, the following settings applies") +"</em>"
                    "<ul>"  +
                        "<li>" + tr("Scanner type") + ": " + tr("String") + " <em>" + tr("default") + "</em> = " + "Real shots - Spherical" + "</li>" +
                        "<li>" + tr("Position in meters") + ": " + tr("3 doubles (x,y,z)") + " " + tr("sh") + "<em>" + tr("default") + "</em> = " + "Real shots - Spherical" + "</li>" +
                        "<li>" + tr("Angles clockwise") + ": " + tr("boolean") + " <em>" + tr("default") + "</em> = " + tr("false") + "</li>" +
                        "<li>" + tr("Angles in radians") + ": " + tr("boolean") + " <em>" + tr("default") + "</em> = " + tr("false") + "</li>" +
                        "<li>" + tr("Angular resolution (Horizontal, Vertical)") + ": " + tr("2 doubles") + " <em>" + tr("default") + "</em> = " + tr("(0.0, 0.0)") + "</li>" +
                        "<li>" + tr("Openning angle H - Range") + ": " + tr("2 doubles") + " <em>" + tr("default") + "</em> = " + tr("(0.0, 0.0)") + "</li>" +
                        "<li>" + tr("Openning angle V - Range") + ": " + tr("2 doubles") + " <em>" + tr("default") + "</em> = " + tr("(0.0, 0.0)") + "</li>" +
                        "<li>" + tr("Direction") + ": " + tr("3 double") + " <em>" + tr("default") + "</em> = " + "(0.0, 0.0, 0.0)" + "</li>" +
                    "</ul>" +
                "</li>" +
            "</ul>";
}

QString LVOX3_StepLoadFiles::outputDescription() const
{
    auto indent = [](QString s) -> QString { return "<div style=\"margin: 0 0 0 40px;\">" + s+ "</div>"; };

    return CT_AbstractStep::outputDescription() + "<br>Result Group [Group]:" +
            indent("Scene(s) [Point scene]") +
            indent("LVOX Scanner [Scan position]") +
            indent("Shooting pattern [Shooting pattern]");
}


void LVOX3_StepLoadFiles::declareInputModels(CT_StepInModelStructureManager& manager)
{
    manager.setNotNeedInputResult();
}

bool LVOX3_StepLoadFiles::postInputConfigure()
{
    QList<CT_AbstractReader*> validReaders;
    QList<CT_AbstractReader*> availableReaders = pluginStaticCastT<LVOX_StepPluginManager>()->readersAvailable();

    for (int i = 0 ; i < availableReaders.size() ; i++)
    {
        CT_AbstractReader* reader = availableReaders.at(i);
        CT_ReaderPointsFilteringExtension* readerCast = dynamic_cast<CT_ReaderPointsFilteringExtension*>(reader);
        if (readerCast != nullptr)
        {
            validReaders.append(reader);
        }
    }

    LoadFileConfiguration c;
    c.setReaders(validReaders);
    c.setCurrentReaderByClassName(m_reader != nullptr ? m_reader->uniqueName() : "");
    c.setConfiguration(m_configuration);
    c.setFilterPointsOrigin(m_filterPointsOrigin);
    c.setRestrictScene(m_restrictScene);
    c.setRestrictRadius(m_restrictRadius);

    if(CT_ConfigurableWidgetToDialog::exec(&c) == QDialog::Rejected)
        return false;

    QList<LoadFileConfiguration::Configuration> configs = c.getConfiguration();
    CT_AbstractReader* reader = c.getReaderToUse()->copyFull();

    if(reader->setFilePath(configs.first().filepath)) {
        reader->setFilePathCanBeModified(false);
        bool ok = reader->configure();
        reader->setFilePathCanBeModified(true);

        if(ok) {
            delete m_reader;
            m_reader = reader;
            m_configuration = configs;
            m_useUserScannerConfiguration = false;

            foreach(const LoadFileConfiguration::Configuration& config, configs) {
                if(LVOX3_ScannerUtils::isCustomScannerConfiguration(config.scannerId)) {

                    m_useUserScannerConfiguration = true;
                    break;
                }
            }

            m_filterPointsOrigin = c.isFilterPointsOrigin();
            m_restrictScene = c.isRestrictScene();
            m_restrictRadius = c.getRestrictRadius();
            setSettingsModified(true);
            return true;
        }
    }

    return false;
}


void LVOX3_StepLoadFiles::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    if(m_reader == nullptr)
        return;

    manager.addResult(m_hOutResult);
    manager.setRootGroup(m_hOutResult, m_hOutRootGroup);
    m_reader->declareOutputModelsInGroupWithHeader(manager, m_hOutRootGroup);

    manager.addItem(m_hOutRootGroup, m_outScanner, tr("Scanner LVOX"), tr("Scanner position"));
    manager.addItem(m_hOutRootGroup, m_outShootingPattern, tr("Shooting Pattern"), tr("Shooting Pattern/Position/Direction/Angle forced by user"));

}

void LVOX3_StepLoadFiles::compute()
{
    int i = 0;
    size_t sceneTotalPoints = 0;
    size_t nShiftedPoints = 0;
    int nFilesProgress = 0; //Used to count the number of files and give estimate progress bar

    QListIterator<LoadFileConfiguration::Configuration> it(m_configuration);

    CT_ReaderPointsFilteringExtension* readerWithFilter = dynamic_cast<CT_ReaderPointsFilteringExtension*>(m_reader);

    // Configure the filter of the reader based on the selected option
    if (m_filterPointsOrigin && readerWithFilter != nullptr) {
        readerWithFilter->setPointsFilter([](const CT_Point &pt) {
            return (pt == Eigen::Vector3d::Zero());
        });
    }

    //Counts the number of files (There has to be a more efficient way) used for progress bar
    while(it.hasNext()){
        nFilesProgress++;
        it.next();
    }
    it.toFront();//Brings pointer back to front after counting

    for (CT_ResultGroup* outRes : m_hOutResult.iterateOutputs())
    {
        // Load all files
        while(it.hasNext() && !isStopped())
        {
            LoadFileConfiguration::Configuration config = it.next();
            m_reader->setFilePath(config.filepath);

            CT_StandardItemGroup* rootGroup = new CT_StandardItemGroup();
            outRes->addRootGroup(m_hOutRootGroup, rootGroup);

            bool ok = m_reader->readFile(rootGroup);

            //Most of this loop has to do with repositioning points in the cloud if the point cloud is not filtered (to lessen the computing range to 60 meters)
            if (ok)
            {
                CT_AbstractItemDrawableWithPointCloud* scene = readerWithFilter->scene();

                if (scene != nullptr)
                {
                    double xmin = std::numeric_limits<double>::max();
                    double ymin = std::numeric_limits<double>::max();
                    double zmin = std::numeric_limits<double>::max();

                    double xmax = -std::numeric_limits<double>::max();
                    double ymax = -std::numeric_limits<double>::max();
                    double zmax = -std::numeric_limits<double>::max();

                    //Gets the scene's pointcloudindexregistered to avoid duplicating point clouds.
                    CT_PCIR pcir = scene->pointCloudIndexRegistered();
                    CT_Point shiftedPoint;
                    CT_MutablePointIterator *itNewPC = nullptr;

                    //Test to see which points are filtered, it doesn't seem to filter anything, since the filter is looking for points on 0,0,0 and there are none.
                    CT_PointIterator itPointCloud(scene->pointCloudIndex());
                    sceneTotalPoints += itPointCloud.size();

                    //Test to see if the scanner position is too far from the point cloud.
                    double distance = sqrt(pow(config.scannerPosition.x()-scene->centerX(),2.0)+pow(config.scannerPosition.y()-scene->centerY(),2.0)+pow(config.scannerPosition.z()-scene->centerZ(),2.0));

                    if(distance > 800)
                    {
                        if (config.scannerId == ScannerPlanePointCloud)
                        {
                            config.scannerPosition.x() = scene->centerX();
                            config.scannerPosition.y() = scene->centerY();
                            config.scannerPosition.z() = scene->maxZ() + 1;
                            PS_LOG->addErrorMessage(LogInterface::step, tr("The scanner position is over 800 m for the center of the scene. It has now been set to values center over the scene."));
                        } else
                        {
                            config.scannerPosition.x() = scene->minX();
                            config.scannerPosition.y() = scene->minY();
                            config.scannerPosition.z() = scene->minZ();
                            PS_LOG->addErrorMessage(LogInterface::step, tr("The scanner position is over 800 m for the center of the scene. It has now been set to values at the minimum of the scene."));
                        }

                        PS_LOG->addErrorMessage(LogInterface::step, tr("New Scanner X : %1").arg(config.scannerPosition.x()));
                        PS_LOG->addErrorMessage(LogInterface::step, tr("New Scanner Y : %1").arg(config.scannerPosition.y()));
                        PS_LOG->addErrorMessage(LogInterface::step, tr("New Scanner Z : %1").arg(config.scannerPosition.z()));
                    }

                    itNewPC = new CT_MutablePointIterator(pcir);

                    //If the user checked to restrict the scene
                    if (m_restrictScene) {

                        //If the radius is higher than 0 (to be fair, anything under 25-30 meters is mostly pointless, because it will most certainly
                        //move tree points towards the scanner)
                        if(m_restrictRadius != 0) {

                            while(itPointCloud.hasNext() && !isStopped())
                            {
                                const CT_Point &point = itPointCloud.next().currentPoint();
                                double distance = (point - config.scannerPosition).norm();

                                //Evaluate if the point is in the specified radius
                                if (distance > m_restrictRadius) {
                                    Eigen::Vector3d newPointCoordinates = config.scannerPosition + m_restrictRadius * (point - config.scannerPosition).normalized();

                                    shiftedPoint(0) = newPointCoordinates(0);
                                    shiftedPoint(1) = newPointCoordinates(1);
                                    shiftedPoint(2) = newPointCoordinates(2);

                                    itNewPC->next();
                                    itNewPC->replaceCurrentPoint(shiftedPoint);
                                    ++nShiftedPoints;

                                } else {
                                    shiftedPoint = point;
                                    itNewPC->next();
                                }

                                // In case the user inserts a filtered scene and all the points are inside the default massive bbox
                                if (shiftedPoint(0) < xmin) {xmin = shiftedPoint(0);}
                                if (shiftedPoint(0) > xmax) {xmax = shiftedPoint(0);}
                                if (shiftedPoint(1) < ymin) {ymin = shiftedPoint(1);}
                                if (shiftedPoint(1) > ymax) {ymax = shiftedPoint(1);}
                                if (shiftedPoint(2) < zmin) {zmin = shiftedPoint(2);}
                                if (shiftedPoint(2) > zmax) {zmax = shiftedPoint(2);}
                            }

                            scene->setBoundingBox(xmin, ymin, zmin, xmax, ymax, zmax);
                            scene->setPointCloudIndexRegistered(pcir);

                        }else {
                            PS_LOG->addMessage(LogInterface::warning, LogInterface::step, QObject::tr("Cancelling scene narrowing : radius of 0 meter"));
                        }
                    }

                    CT_ShootingPattern *pattern = makeShootingPattern(config, pcir);
                    //Creates Scanner LVOX, to take in account custom shooting patterns and scanner position shifts
                    if(pattern != nullptr) {
                        //adds scanner LVOX to the model viewer
                        CT_Scanner *scanner = new CT_Scanner(i, pattern);
                        rootGroup->addSingularItem(m_outScanner, scanner);

                        CT_ShootingPatternD *patternD = new CT_ShootingPatternD(pattern);
                        rootGroup->addSingularItem(m_outShootingPattern, patternD);
                    }
                } else {
                    PS_LOG->addErrorMessage(LogInterface::step, tr("CT_Scene object not found or loaded object has no scene."));
                }

            } else {
                PS_LOG->addErrorMessage(LogInterface::step, tr("CT_Scene object not found or loaded object has no scene."));
            }

            ++i;
            //Each file is an equal percentage of loading
            setProgress((100.00/nFilesProgress)*i);
        }
    }


    //Statistic for scene restriction
    if(m_restrictScene&&m_restrictRadius != 0)
        PS_LOG->addMessage(LogInterface::info, LogInterface::step, QString(tr("Restriction de %1 points.")).arg(nShiftedPoints));

}



//Test to see if any part of the voxel is inside the radius of the extracted grid (If it is, it is added to the extracted grid)
bool LVOX3_StepLoadFiles::evaluatePoint(const Eigen::Vector3d scannerCenterCoords, const CT_Point &point, Eigen::Vector3d &newPointCoordinates){

    double distance = sqrt(pow(scannerCenterCoords(0)-point(0),2.0)+pow(scannerCenterCoords(1)-point(1),2.0)+pow(scannerCenterCoords(2)-point(2),2.0));
    //Find a point in 3D that is m_restrictRadius meters away from the scanner position in the direction of the point cloud point
    //Too complicated
    if(distance > m_restrictRadius){
        double uFactor = m_restrictRadius/distance;
        double  xc = (1-uFactor)*scannerCenterCoords(0)+uFactor*point(0);
        double yc = (1-uFactor)*scannerCenterCoords(1)+uFactor*point(1);
        double  zc = (1-uFactor)*scannerCenterCoords(2)+uFactor*point(2);

        newPointCoordinates(0) = xc;
        newPointCoordinates(1) = yc;
        newPointCoordinates(2) = zc;

        return true;
    }
    return false;
}
