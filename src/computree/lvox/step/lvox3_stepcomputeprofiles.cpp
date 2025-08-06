#include "lvox3_stepcomputeprofiles.h"

//In/Out
#include "ct_result/ct_resultgroup.h"

//Tool
#include "ct_view/tools/ct_configurablewidgettodialog.h"

//Drawables
#include "tools/lvox3_gridtype.h"
#include "tools/lvox3_errorcode.h"

#include <Eigen/Geometry>

//ChangeToVoxel method
#define MINProfile(a, b) ((a < b) ? b : a)
#define MAXProfile(a, b) ((a > b) ? b : a)


LVOX3_StepComputeProfiles::LVOX3_StepComputeProfiles() : CT_AbstractStep()
{
    //Voxel values
    m_configuration.minValue = 0.000000;
    m_configuration.maxValue = 999999.000000;

    m_configuration.ordonneeAxis = "Z";
    m_configuration.genAxis = "X";
    m_configuration.unitType = tr("Coordonnées");
    m_configuration.gridCustomized = false;
    m_configuration.genValuesInPourcent = false;
    m_configuration.averaged = true;
    m_configuration.minGen = -100.00;
    m_configuration.maxGen = 100.00;
    m_configuration.stepGen = 999999.00;
    m_configuration.terminalCustomized = false;
    m_configuration.abscisseOrdonneeValuesInPourcent = false;

    //coordinates to and from
    m_configuration.minAbscisse = -100.00;
    m_configuration.maxAbscisse = 100.00;
    m_configuration.minOrdonnee = -100.00;
    m_configuration.maxOrdonnee = 100.00;
}

QString LVOX3_StepComputeProfiles::description() const
{
    return tr("6 - Create profiles from voxels (LVOX3)");
}

// Step detailed description
QString LVOX3_StepComputeProfiles::detailledDescription() const
{
    return QString() +
            tr("This step allows creating density histograms for the whole grid (by default) or a chosen slice of it.") + "<br><br>" +
            tr("By default, the Z axis is considered the main axis and profiles are computed on the whole grid") + "<br>" +
            tr("Alternatively, the custom mode allows for chosing the northing and easting axis as well as limiting computations to part of the grid.") +
            tr("Values can either be expressed as a coordinates or voxel as well as in percentage.");
}

CT_VirtualAbstractStep* LVOX3_StepComputeProfiles::createNewInstance() const
{
    // Creates an instance of this step
    return new LVOX3_StepComputeProfiles();
}


void LVOX3_StepComputeProfiles::savePostSettings(SettingsWriterInterface &writer) const
{
    CT_AbstractStep::savePostSettings(writer);

    writer.addParameter(this, "version", 1);
    writer.addParameter(this, "minValue", m_configuration.minValue);
    writer.addParameter(this, "maxValue", m_configuration.maxValue);
    writer.addParameter(this, "ordonneeAxis", m_configuration.ordonneeAxis);
    writer.addParameter(this, "genAxis", m_configuration.genAxis);
    writer.addParameter(this, "unitType", m_configuration.unitType);
    writer.addParameter(this, "gridCustomized", m_configuration.gridCustomized);
    writer.addParameter(this, "genValuesInPourcent", m_configuration.genValuesInPourcent);
    writer.addParameter(this, "averaged", m_configuration.averaged);
    writer.addParameter(this, "minGen", m_configuration.minGen);
    writer.addParameter(this, "maxGen", m_configuration.maxGen);
    writer.addParameter(this, "stepGen", m_configuration.stepGen);
    writer.addParameter(this, "terminalCustomized", m_configuration.terminalCustomized);
    writer.addParameter(this, "abscisseOrdonneeValuesInPourcent", m_configuration.abscisseOrdonneeValuesInPourcent);
    writer.addParameter(this, "minAbscisse", m_configuration.minAbscisse);
    writer.addParameter(this, "maxAbscisse", m_configuration.maxAbscisse);
    writer.addParameter(this, "minOrdonnee", m_configuration.minOrdonnee);
    writer.addParameter(this, "maxOrdonnee", m_configuration.maxOrdonnee);
}

bool LVOX3_StepComputeProfiles::restorePostSettings(SettingsReaderInterface &reader)
{
    if(!CT_AbstractStep::restorePostSettings(reader))
        return false;

    QVariant value;

    reader.parameter(this, "minValue", value); m_configuration.minValue = value.toDouble();
    reader.parameter(this, "maxValue", value); m_configuration.maxValue = value.toDouble();
    reader.parameter(this, "ordonneeAxis", value); m_configuration.ordonneeAxis = value.toString();
    reader.parameter(this, "genAxis", value); m_configuration.genAxis = value.toString();
    reader.parameter(this, "unitType", value); m_configuration.unitType = value.toString();
    reader.parameter(this, "gridCustomized", value); m_configuration.gridCustomized = value.toBool();
    reader.parameter(this, "genValuesInPourcent", value); m_configuration.genValuesInPourcent = value.toBool();
    reader.parameter(this, "averaged", value); m_configuration.averaged = value.toBool();
    reader.parameter(this, "minGen", value); m_configuration.minGen = value.toDouble();
    reader.parameter(this, "maxGen", value); m_configuration.maxGen = value.toDouble();
    reader.parameter(this, "stepGen", value); m_configuration.stepGen = value.toInt();
    reader.parameter(this, "terminalCustomized", value); m_configuration.terminalCustomized = value.toBool();
    reader.parameter(this, "abscisseOrdonneeValuesInPourcent", value); m_configuration.abscisseOrdonneeValuesInPourcent = value.toBool();
    reader.parameter(this, "minAbscisse", value); m_configuration.minAbscisse = value.toDouble();
    reader.parameter(this, "maxAbscisse", value); m_configuration.maxAbscisse = value.toDouble();
    reader.parameter(this, "minOrdonnee", value); m_configuration.minOrdonnee = value.toDouble();
    reader.parameter(this, "maxOrdonnee", value); m_configuration.maxOrdonnee = value.toDouble();

    return true;
}

QString LVOX3_StepComputeProfiles::parametersDescription(bool _)
{
    return CT_AbstractStep::parametersDescription() +
            "<strong>" + tr("Step parameters:") + "</strong>"
            "<ul>"
                "<li>" + tr("Reset configuration") + ": " + tr("button") + "<em>" + tr("effect") + "</em> = " + tr("Compute profile on all grids along Z axis") + "</li>"
                "<li>" + tr("Main Axis") + ": " + tr("String") + " <em>" + tr("with the following possible values") + "</em> = "
                    "<ul>"
                        "<li>X : " + tr("Compute with") + " <em>max(nbi)</em></li>"
                        "<li>Y : " + tr("Compute with") + " <em>max(nt - nb)</em></li>"
                        "<li>Z : " + tr("Compute with") + " <em>max((nt - nb)/nt)</em></li>"
                    "</ul>"
                "</li>"
                "<li>" + tr("Generation axis") + ": " + tr("String") + " <em>" + tr("with the following possible values") + "</em> = "
                    "<ul>"
                        "<li>X</li>"
                        "<li>Y</li>"
                        "<li>Z</li>"
                    "</ul>"
                "</li>"
                "<li>" + tr("Calculation unit") + ": " + tr("String") + " <em>" + tr("with the following possible values") + "</em> = "
                    "<ul>"
                        "<li>" + tr("Coordinates") + " : " + tr("Compute according to coordinates") + "</li>"
                        "<li>" + tr("Voxels") + " : " + tr("Compute with ") + "</li>"
                    "</ul>"
                "</li>"
                "<li>" + tr("Calculation unit") + ": " + tr("String") + " <em>" + tr("with the following possible values") + "</em> = "
                    "<ul>"
                        "<li>" + tr("Coordinates") + " : " + tr("Compute with") + " <em>max(nbi)</em></li>"
                        "<li>" + tr("Voxels") + " : " + tr("Compute with") + " <em>max(nt - nb)</em></li>"
                    "</ul>"
                "</li>"
                "<li>" + tr("Average") + ": " + tr("boolean") + " <em>" + tr("default") + "</em> = " + tr("true") + "</li>"
                "<li>" + tr("Compute on") + ": " + tr("String") + " <em>" + tr("either one of those values") + "</em> = "
                    "<ul>"
                        "<li>" + tr("Whole grid") + "</li>" +
                        "<li>" + tr("Custom settings") + " : " + tr("Enables the following options") + " : "
                            "<ul>"
                                "<li>" + tr("Generation axis") + " : " + tr("one of the following except the current value of") + " <em>" + tr("Generation axis") + "</em> :"
                                    "<ul>"
                                        "<li>X</li>"
                                        "<li>Y</li>"
                                        "<li>Z</li>"
                                    "</ul>"
                                "</li>"
                                "<li>" + tr("Generaton axis bounds") + " : " + tr("2 double (boundInf, boundSup)") + " <em>" + tr("default") + "</em> = " + tr("(-100.0, 100.0)") + "</li>"
                                "<li>" + tr("Values in percentage") + " : " + tr("boolean") + " <em>" + tr("default") + "</em> = " + tr("false") + "</li>"
                                "<li>" + tr("Steps between two profiles") + " : " + tr("double") + " <em>" + tr("default") + "</em> = " + tr("999999.00") + "</li>"
                                "<li>" + tr("Restrict computations on part of the grid") + " : " + tr("boolean") + " <em>" + tr("default") + "</em> = " + tr("false") + " " + tr("true enables : ") +
                                    "<ul>"
                                        "<li>" + tr("Northing axis bounds") + ": " + tr("2 double (boundInf, boundSup)") + " <em>" + tr("default") + "</em> = " + tr("(-100.0, 100.0)") + "</li>"
                                        "<li>" + tr("Easting axis bounds") + ": " + tr("2 double (boundInf, boundSup)") + " <em>" + tr("default") + "</em> = " + tr("(-100.0, 100.0)") + "</li>"
                                        "<li>" + tr("Values in percentage") + ": " + tr("boolean") + "  <em>" + tr("default") + "</em> = " + tr("false") + "</li>"
                                    "</ul>"
                                "</li>"
                            "</ul>"
                        "</li>"
                    "</ul>"
                "</li>"
            "</ul>"
            "<em><strong>" + tr("Note") + "</strong> :" + tr("When \" values in percentage\" is checked, the range of bounds becomes 0 < x <= 100") + "</em>";
}

QString LVOX3_StepComputeProfiles::outputDescription() const
{
    auto indent = [](QString s) -> QString { return "<div style=\"margin: 0 0 0 40px;\">" + s+ "</div>"; };

    return CT_AbstractStep::outputDescription() + "<br>Group [Group]:" +
            indent("...") +
            indent("Group [Group]:") +
            indent(indent("Profile [Profile<double>]")) +
            indent(indent("Profile grid [LVOX3_Grid3D<float>]"));

}

void LVOX3_StepComputeProfiles::declareInputModels(CT_StepInModelStructureManager& manager)
{
    manager.addResult(_inResult, tr("Grilles"));
    manager.setZeroOrMoreRootGroup(_inResult, _inZeroOrMoreRootGroup);
    manager.addGroup(_inZeroOrMoreRootGroup, _inGroup);
    manager.addItem(_inGroup, _inGrid, tr("Grille 3D"));
}

bool LVOX3_StepComputeProfiles::postInputConfigure()
{
    ComputeProfilesConfiguration config;
    config.setConfiguration(m_configuration);
    if(CT_ConfigurableWidgetToDialog::exec(&config) == QDialog::Accepted) {

        m_configuration = config.getConfiguration();
        setSettingsModified(true);

        return true;
    }

    return false;
}

void LVOX3_StepComputeProfiles::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    manager.addResultCopy(_inResult);
    manager.addGroup(_inGroup, _outGroup);;
    manager.addItem(_outGroup, _outProfile, tr("Profil"));
    manager.addItem(_outGroup, _outGrid3D, tr("Profile Grid"));
}

void LVOX3_StepComputeProfiles::compute()
{
    Vector3SizeT indexGenAbscisseOrdonnee;

    //Declaring different axis
    Eigen::Vector3i axeOrdonnee(1, 0, 0);
    Eigen::Vector3i axeAbscisse(1, 0, 0);
    Eigen::Vector3i axeGen(1, 0, 0);

    //Switch axis based on profile orientation
    if(m_configuration.ordonneeAxis.toLower() == "y")
        axeOrdonnee = Eigen::Vector3i(0, 1, 0);
    else if(m_configuration.ordonneeAxis.toLower() == "z")
        axeOrdonnee = Eigen::Vector3i(0, 0, 1);

    if(m_configuration.genAxis.toLower() == "y")
        axeGen = Eigen::Vector3i(0, 1, 0);
    else if(m_configuration.genAxis.toLower() == "z")
        axeGen = Eigen::Vector3i(0, 0, 1);

    //Cross product of ordonnee and gen to find the abscisse axis
    axeAbscisse = axeOrdonnee.cross(axeGen).array().abs();

    Eigen::Array3i colLinLevelIndex(0, 0, 0);
    colLinLevelIndex = colLinLevelIndex + (axeOrdonnee.cast<int>().array() * 2);
    colLinLevelIndex = colLinLevelIndex + (axeAbscisse.cast<int>().array() * 1);



    for (CT_StandardItemGroup* group : _inGroup.iterateOutputs(_inResult))
    {
        if (isStopped()) {return;}

        const CT_AbstractGrid3D* inGrid = group->singularItem(_inGrid);
        const double res = inGrid->resolution();

        QList<double> axisMinMax;


        if (inGrid != NULL)
        {
            if((inGrid->xdim() > 0)
                    && (inGrid->ydim() > 0)
                    && (inGrid->zdim() > 0)
                    && (res > 0)) {

                //Declaring output grid to be able to export personalized grid of profile
                lvox::Grid3Df *profileGrid = new lvox::Grid3Df(
                        inGrid->minX(),
                        inGrid->minY(),
                        inGrid->minZ(),
                        inGrid->xdim(),
                        inGrid->ydim(),
                        inGrid->zdim(),
                       res,
                        lvox::Max_Error_Code,
                        0);

                //Declaration of generation axii
                Vector3SizeT startEndStepGen(0, 0, 1);
                Vector3SizeT startEndStepOrdonnee(0, 0, 1);
                Vector3SizeT startEndStepAbscisse(0, 0, 1);

                Vector3SizeT gridDim(inGrid->xdim(), inGrid->ydim(), inGrid->zdim());

                Index genDim = gridDim.dot(axeGen.cast<Index>());
                Index ordonneeDim = gridDim.dot(axeOrdonnee.cast<Index>());
                Index abscisseDim = gridDim.dot(axeAbscisse.cast<Index>());

                Index* col = NULL;
                Index* lin = NULL;
                Index* level = NULL;

                bool ok;
                double NAValue = inGrid->NAAsString().toDouble(&ok);

                if (!ok)
                    NAValue = -std::numeric_limits<double>::max();

                col = &indexGenAbscisseOrdonnee(colLinLevelIndex(0));
                lin = &indexGenAbscisseOrdonnee(colLinLevelIndex(1));
                level = &indexGenAbscisseOrdonnee(colLinLevelIndex(2));

                //Transforms the values in meters into voxels if grid personnalized (TOSELF: There's so much repetition it actually hurts my brain, will need to fix that, if possible)
                if (m_configuration.gridCustomized){
                    if (m_configuration.unitType.toLower().at(0) == "c"){

                        axisMinMax = changeToVoxel(inGrid);
                        //If both gen and abs/ord aren't in percentages
                        if(!m_configuration.abscisseOrdonneeValuesInPourcent && !m_configuration.genValuesInPourcent){
                            setStartEnd(axisMinMax.at(0), axisMinMax.at(1), startEndStepOrdonnee, ordonneeDim, m_configuration.abscisseOrdonneeValuesInPourcent);
                            setStartEnd(axisMinMax.at(2), axisMinMax.at(3), startEndStepAbscisse, abscisseDim, m_configuration.abscisseOrdonneeValuesInPourcent);
                            setStartEnd(axisMinMax.at(4),axisMinMax.at(5), startEndStepGen, genDim, m_configuration.genValuesInPourcent);
                            setStep(axisMinMax.at(6), startEndStepGen, m_configuration.genValuesInPourcent);

                        } else if(!m_configuration.abscisseOrdonneeValuesInPourcent){
                            setStartEnd(axisMinMax.at(0),axisMinMax.at(1), startEndStepOrdonnee, ordonneeDim, m_configuration.abscisseOrdonneeValuesInPourcent);
                            setStartEnd(axisMinMax.at(2), axisMinMax.at(3), startEndStepAbscisse, abscisseDim, m_configuration.abscisseOrdonneeValuesInPourcent);
                            setStartEnd(m_configuration.minGen, m_configuration.maxGen, startEndStepGen, genDim, m_configuration.genValuesInPourcent);
                            setStep(m_configuration.stepGen, startEndStepGen, m_configuration.genValuesInPourcent);

                        } else if(!m_configuration.genValuesInPourcent){
                            setStartEnd(m_configuration.minOrdonnee, m_configuration.maxOrdonnee, startEndStepOrdonnee, ordonneeDim, m_configuration.abscisseOrdonneeValuesInPourcent);
                            setStartEnd(m_configuration.minAbscisse, m_configuration.maxAbscisse, startEndStepAbscisse, abscisseDim, m_configuration.abscisseOrdonneeValuesInPourcent);
                            setStartEnd(axisMinMax.at(0), axisMinMax.at(1), startEndStepGen, genDim, m_configuration.genValuesInPourcent);
                            setStep(axisMinMax.at(2), startEndStepGen, m_configuration.genValuesInPourcent);
                        }
                    } else if(m_configuration.unitType.toLower() == "voxel" || (m_configuration.abscisseOrdonneeValuesInPourcent && m_configuration.genValuesInPourcent)){
                        //if voxels is chosen or both are percentages, goes straight to startEndStep
                        setStartEnd(m_configuration.minOrdonnee, m_configuration.maxOrdonnee, startEndStepOrdonnee, ordonneeDim, m_configuration.abscisseOrdonneeValuesInPourcent);
                        setStartEnd(m_configuration.minAbscisse, m_configuration.maxAbscisse, startEndStepAbscisse, abscisseDim, m_configuration.abscisseOrdonneeValuesInPourcent);
                        setStartEnd(m_configuration.minGen, m_configuration.maxGen, startEndStepGen, genDim, m_configuration.genValuesInPourcent);
                        setStep(m_configuration.stepGen, startEndStepGen, m_configuration.genValuesInPourcent);
                    }
                } else { //FIXME : hardcoded for full grid because of new way to put coordinates in, which makes it not work properly (minus values)
                    setStartEnd(0, 999999, startEndStepOrdonnee, ordonneeDim, m_configuration.abscisseOrdonneeValuesInPourcent);
                    setStartEnd(0, 999999, startEndStepAbscisse, abscisseDim, m_configuration.abscisseOrdonneeValuesInPourcent);
                    setStartEnd(0, 999999, startEndStepGen, genDim, m_configuration.genValuesInPourcent);
                    setStep(999999, startEndStepGen, m_configuration.genValuesInPourcent);
                }


                CT_Profile<double>* outProfile;

                for (Index currentProfileIndex = startEndStepGen(0); currentProfileIndex < startEndStepGen(1); currentProfileIndex += startEndStepGen(2))
                {
                    outProfile = createProfile(inGrid, currentProfileIndex, axeGen, axeOrdonnee, NAValue);

                    Index begin = currentProfileIndex;
                    Index end = qMin(startEndStepGen(1), begin + startEndStepGen(2));
                    int nbVoxelsInSpot = 0;//nb voxels # 0
                    for(indexGenAbscisseOrdonnee(0) = begin; indexGenAbscisseOrdonnee(0) < end; indexGenAbscisseOrdonnee(0) += 1)
                    {
                        for (indexGenAbscisseOrdonnee(1) = startEndStepAbscisse(0);  indexGenAbscisseOrdonnee(1) < startEndStepAbscisse(1); indexGenAbscisseOrdonnee(1) += startEndStepAbscisse(2))
                        {
                            for (indexGenAbscisseOrdonnee(2) = startEndStepOrdonnee(0); indexGenAbscisseOrdonnee(2) < startEndStepOrdonnee(1); indexGenAbscisseOrdonnee(2) += startEndStepOrdonnee(2))
                            {
                                Index index;
                                inGrid->index(*col, *lin, *level, index);
                                double value = inGrid->valueAtIndexAsDouble(index);
                                if(value != lvox::ErrorOrWarningCode::Sky){
                                    nbVoxelsInSpot++;
                                    //qDebug()<<"###value is good" << value;
                                }else{
                                    //qDebug()<<"###value is not good" << value;
                                }
                                if (value > m_configuration.minValue && value < m_configuration.maxValue){
                                    profileGrid->addValueAtIndex(index,value);
                                    //if the value is below 0, it counts the number of them per level, instead of normal behaviour
                                    if(value >= 0){
                                        outProfile->addValueAtIndex(indexGenAbscisseOrdonnee(2), value);
                                    }else{
                                        //@TODO: ???
                                        outProfile->addValueAtIndex(indexGenAbscisseOrdonnee(2), 1);
                                    }
                                }
                            }
                        }
                    }
                    if(m_configuration.averaged){
                        int nbVoxelsInAProfileLevel = nbVoxelsInSpot/outProfile->nCells();
                        qDebug()<<"Nb voxel in a z level"<< nbVoxelsInAProfileLevel;

                        for (unsigned int i = 0; i < outProfile->nCells();i++){
                            //@TODO: check axis
                            //@TODO: check nbvoxel vs grid extract
                            outProfile->setValueAtIndex(i, outProfile->valueAtIndex(i)/ nbVoxelsInAProfileLevel);
                        }
                    }
                    addProfile(outProfile, group, profileGrid);
                }
            }
        }
    }
}

//Creates the density profile of the inGrid + personnalized information
CT_Profile<double>* LVOX3_StepComputeProfiles::createProfile(const CT_AbstractGrid3D* grid,
                                                             const Index& currentIndex,
                                                             const Eigen::Vector3i& axeNormal,
                                                             const Eigen::Vector3i& axeOrdonnee,
                                                             double NAValue) const
{
    Eigen::Array3d gridMin(grid->minX(), grid->minY(), grid->minZ());
    Eigen::Array3d finalMin;
    double resolution = grid->resolution();
    finalMin.x() = gridMin.x() + (axeNormal.cast<double>().x()*(((double)currentIndex)*resolution));
    finalMin.y() = gridMin.y() + (axeNormal.cast<double>().y()*(((double)currentIndex)*resolution));
    finalMin.z() = gridMin.z() + (axeNormal.cast<double>().z()*(((double)currentIndex)*resolution));

    Index profileSize = Vector3SizeT(grid->xdim(), grid->ydim(), grid->zdim()).dot(axeOrdonnee.cast<Index>());

    // TODO : add an orientation vector to the class CT_Profile !
    return new CT_Profile<double>(finalMin.x(),
                                  finalMin.y(),
                                  finalMin.z(),
                                  axeOrdonnee.x(),
                                  axeOrdonnee.y(),
                                  axeOrdonnee.z(),
                                  profileSize,
                                  resolution,
                                  NAValue,
                                  0.0);
}

//Adds the profile and the profile grid to the view
void LVOX3_StepComputeProfiles::addProfile(CT_Profile<double> *profile, CT_StandardItemGroup *group, lvox::Grid3Df* grid)
{
    CT_StandardItemGroup* newGroup = new CT_StandardItemGroup();


    profile->computeMinMax();
    grid->computeMinMax();

    group->addGroup(_outGroup, newGroup);
    newGroup->addSingularItem(_outProfile, profile);
    newGroup->addSingularItem(_outGrid3D, grid);

}

//set the start and end in voxels of each axis
void LVOX3_StepComputeProfiles::setStartEnd(Index min, Index max, LVOX3_StepComputeProfiles::Vector3SizeT &startEndStep, const Index &dim, bool minAndMaxInPourcent)
{
    if(minAndMaxInPourcent) {
        min = (dim*min)/100;
        max = (dim*max)/100;
    }

    startEndStep(0) = qMin(dim-1, min);
    startEndStep(1) = qMax(startEndStep(0), qMin(dim, max));
}

//Sets the voxel value of each step
void LVOX3_StepComputeProfiles::setStep(Index step, LVOX3_StepComputeProfiles::Vector3SizeT &startEndStep, bool stepInPourcent)
{
    Index dim = startEndStep(1)-startEndStep(0);

    if(stepInPourcent) {
        step = (dim*step)/100;
    }

    startEndStep(2) = qMax((Index)1, qMin(startEndStep(1)-startEndStep(0), step));
}

//Changes the user entered values for axis limits from coordinates to voxels (to remove user calculation)
QList<double> LVOX3_StepComputeProfiles::changeToVoxel(const CT_AbstractGrid3D* inGrid){
    QList<double> axisMinMax;
    QList<double> coordinates;
    double voxelGridLowerX;
    double voxelGridLowerY;
    double voxelGridLowerZ;
    const double resolution = inGrid->resolution();

    //Have to round because it does floor or ceiling in the function setStartEnd, which plays with data accuracy
    voxelGridLowerX = std::fabs(inGrid->minX());
    voxelGridLowerY = std::fabs(inGrid->minY());
    voxelGridLowerZ = std::fabs(inGrid->minZ());

    //Ordonnee axis meter distance from grid lower left point calculation (MINProfile and MAXProfile are declared at the top of this file)
    if(m_configuration.ordonneeAxis.toLower() == "x"){
        if(inGrid->minX()>= 0){
            coordinates.append(MINProfile(m_configuration.minOrdonnee,inGrid->minX()) - voxelGridLowerX);
            coordinates.append(MAXProfile(m_configuration.maxOrdonnee,inGrid->maxX()) - voxelGridLowerX);

        }else{
            coordinates.append(MINProfile(m_configuration.minOrdonnee,inGrid->minX()) + voxelGridLowerX);
            coordinates.append(MAXProfile(m_configuration.maxOrdonnee,inGrid->maxX()) + voxelGridLowerX);
        }
        //If the grid calculations are not based on percentages
        if(!m_configuration.abscisseOrdonneeValuesInPourcent){
            axisMinMax.append(coordinates.at(0)/resolution);
            axisMinMax.append(coordinates.at(1)/resolution);
        }
    }else if(m_configuration.ordonneeAxis.toLower() == "y"){
        if(inGrid->minY()>= 0){
            coordinates.append(MINProfile(m_configuration.minOrdonnee,inGrid->minY()) - voxelGridLowerY);
            coordinates.append(MAXProfile(m_configuration.maxOrdonnee,inGrid->maxY()) - voxelGridLowerY);

        }else{
            coordinates.append(MINProfile(m_configuration.minOrdonnee,inGrid->minY()) + voxelGridLowerY);
            coordinates.append(MAXProfile(m_configuration.maxOrdonnee,inGrid->maxY()) + voxelGridLowerY);
        }
        //If the grid calculations are not based on percentages
        if(!m_configuration.abscisseOrdonneeValuesInPourcent){
            axisMinMax.append(coordinates.at(0)/resolution);
            axisMinMax.append(coordinates.at(1)/resolution);
        }
    }else if(m_configuration.ordonneeAxis.toLower() == "z"){
        if(inGrid->minZ()>= 0){
            coordinates.append(MINProfile(m_configuration.minOrdonnee,inGrid->minZ()) - voxelGridLowerZ);
            coordinates.append(MAXProfile(m_configuration.maxOrdonnee,inGrid->maxZ()) - voxelGridLowerZ);

        }else{
            coordinates.append(MINProfile(m_configuration.minOrdonnee,inGrid->minZ()) + voxelGridLowerZ);
            coordinates.append(MAXProfile(m_configuration.maxOrdonnee,inGrid->maxZ()) + voxelGridLowerZ);
        }
        //If the grid calculations are not based on percentages
        if(!m_configuration.abscisseOrdonneeValuesInPourcent){
            axisMinMax.append(coordinates.at(0)/resolution);
            axisMinMax.append(coordinates.at(1)/resolution);
        }
    }

    //Abscisse axis meter distance from grid lower left point calculation
    if((m_configuration.ordonneeAxis.toLower() == "x" && m_configuration.genAxis.toLower()=="y")||
            (m_configuration.ordonneeAxis.toLower() == "y" && m_configuration.genAxis.toLower()=="x")){
        if(inGrid->minZ()>= 0){
            coordinates.append(MINProfile(m_configuration.minAbscisse,inGrid->minZ()) - voxelGridLowerZ);
            coordinates.append(MAXProfile(m_configuration.maxAbscisse,inGrid->maxZ()) - voxelGridLowerZ);

        }else{
            coordinates.append(MINProfile(m_configuration.minAbscisse,inGrid->minZ()) + voxelGridLowerZ);
            coordinates.append(MAXProfile(m_configuration.maxAbscisse,inGrid->maxZ()) + voxelGridLowerZ);
        }
        //If the grid calculations are not based on percentages
        if(!m_configuration.abscisseOrdonneeValuesInPourcent){
            axisMinMax.append(coordinates.at(2)/resolution);
            axisMinMax.append(coordinates.at(3)/resolution);
        }
    }else if(m_configuration.ordonneeAxis.toLower() == "z" && m_configuration.genAxis.toLower()=="x"){
        if(inGrid->minY()>= 0){
            coordinates.append(MINProfile(m_configuration.minAbscisse,inGrid->minY()) - voxelGridLowerY);
            coordinates.append(MAXProfile(m_configuration.maxAbscisse,inGrid->maxY()) - voxelGridLowerY);

        }else{
            coordinates.append(MINProfile(m_configuration.minAbscisse,inGrid->minY()) + voxelGridLowerY);
            coordinates.append(MAXProfile(m_configuration.maxAbscisse,inGrid->maxY()) + voxelGridLowerY);
        }
        //If the grid calculations are not based on percentages
        if(!m_configuration.abscisseOrdonneeValuesInPourcent){
            axisMinMax.append(coordinates.at(2)/resolution);
            axisMinMax.append(coordinates.at(3)/resolution);
        }
    }else if(m_configuration.ordonneeAxis.toLower() == "z" && m_configuration.genAxis.toLower()=="y"){
        if(inGrid->minX()>= 0){
            coordinates.append(MINProfile(m_configuration.minAbscisse,inGrid->minX()) - voxelGridLowerX);
            coordinates.append(MAXProfile(m_configuration.maxAbscisse,inGrid->maxX()) - voxelGridLowerX);

        }else{
            coordinates.append(MINProfile(m_configuration.minAbscisse,inGrid->minX()) + voxelGridLowerX);
            coordinates.append(MAXProfile(m_configuration.maxAbscisse,inGrid->maxX()) + voxelGridLowerX);
        }
        //If the grid calculations are not based on percentages
        if(!m_configuration.abscisseOrdonneeValuesInPourcent){
            axisMinMax.append(coordinates.at(2)/resolution);
            axisMinMax.append(coordinates.at(3)/resolution);
        }
    }

    //Gen axis meter distance from grid lower left point calculation
    if(m_configuration.genAxis.toLower()=="x"){
        if(inGrid->minX()>= 0){
            coordinates.append(MINProfile(m_configuration.minGen,inGrid->minX()) - voxelGridLowerX);
            coordinates.append(MAXProfile(m_configuration.maxGen,inGrid->maxX()) - voxelGridLowerX);

        }else{
            coordinates.append(MINProfile(m_configuration.minGen,inGrid->minX()) + voxelGridLowerX);
            coordinates.append(MAXProfile(m_configuration.maxGen,inGrid->maxX()) + voxelGridLowerX);
        }
        if(!m_configuration.genValuesInPourcent){
            axisMinMax.append(coordinates.at(4)/resolution);
            axisMinMax.append(coordinates.at(5)/resolution);
            axisMinMax.append(m_configuration.stepGen/resolution); //Step Gen is in meters when coordinates is taken
        }
    }else if(m_configuration.genAxis.toLower()=="y"){
        if(inGrid->minY()>= 0){
            coordinates.append(MINProfile(m_configuration.minGen,inGrid->minY()) - voxelGridLowerY);
            coordinates.append(MAXProfile(m_configuration.maxGen,inGrid->maxY()) - voxelGridLowerY);

        }else{
            coordinates.append(MINProfile(m_configuration.minGen,inGrid->minY()) + voxelGridLowerY);
            coordinates.append(MAXProfile(m_configuration.maxGen,inGrid->maxY()) + voxelGridLowerY);
        }
        if(!m_configuration.genValuesInPourcent){
            axisMinMax.append(coordinates.at(4)/resolution);
            axisMinMax.append(coordinates.at(5)/resolution);
            axisMinMax.append(m_configuration.stepGen/resolution); //Step Gen is in meters when coordinates is taken
        }
    }

    return axisMinMax;
}
