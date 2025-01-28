#include "lvox3_stepextractcirculargrid.h"

//Tools
#include "tools/lvox3_errorcode.h"

//Models
#define DEF_SearchInSourceResult      "rs"
#define DEF_SearchInSourceGroup       "gs"
#define DEF_SearchInGroup             "grp"
#define DEF_SearchInGrid              "grid"

LVOX3_StepExtractCircularGrid::LVOX3_StepExtractCircularGrid() : CT_AbstractStep()
{
    _x = 0.00;
    _y = 0.00;
    _radiusmin = 0.00;
    _radius = 11.28;
    _zmin = -10000;
    _zmax = 10000;
    _isSquare = false;
}

QString LVOX3_StepExtractCircularGrid::description() const
{
    return tr("Circular grid extraction");
}

// Step detailed description
QString LVOX3_StepExtractCircularGrid::detailledDescription() const
{
    return tr("This step allows for the extraction of a circular 3D grid from an input grid. This tool provides a mean to get closer to what is found in forestry.");
}

CT_VirtualAbstractStep* LVOX3_StepExtractCircularGrid::createNewInstance() const
{
    // Creates an instance of this step
    return new LVOX3_StepExtractCircularGrid();
}

QString LVOX3_StepExtractCircularGrid::outputDescription() const
{
    auto indent = [](QString s) -> QString { return "<div style=\"margin: 0 0 0 40px;\">" + s+ "</div>"; };

    return CT_AbstractStep::outputDescription() + "<br>Root group [Group]:" +
            indent("...") +
            indent("LVOX3_Grid3D<float> [Extracted grid]");

}

void LVOX3_StepExtractCircularGrid::declareInputModels(CT_StepInModelStructureManager& manager)
{
    manager.addResult(_inResult, "Input grid", "", true);
    manager.setZeroOrMoreRootGroup(_inResult, _inZeroOrMoreRootGroup);
    manager.addGroup(_inZeroOrMoreRootGroup, _inGroup, "Reference group");
    manager.addItem(_inGroup, _inGrid, "3D grid");
}

void LVOX3_StepExtractCircularGrid::fillPostInputConfigurationDialog(CT_StepConfigurableDialog* configDialog)
{
    configDialog->addDouble(tr("X coordinates of the new grid's center"), "m", -1e+10, 1e+10, 4, _x);
    configDialog->addDouble(tr("Y coordinates of the new grid's center"), "m", -1e+10, 1e+10, 4, _y);
    configDialog->addDouble(tr("Minimum radius of the new grid"), "m", 0, 1e+10, 4, _radiusmin);
    configDialog->addDouble(tr("Maximum radius of the new grid"), "m", 0.01, 1e+10, 4, _radius);
    configDialog->addDouble(tr("Minimum Z level"), "m", -1e+10, 1e+10, 4, _zmin);
    configDialog->addDouble(tr("Maximum Z level"), "m", -1e+10, 1e+10, 4, _zmax);
    //configDialog->addBool  (tr("Extract square grid :"), "m", false, _isSquare);
}

void LVOX3_StepExtractCircularGrid::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    manager.addResultCopy(_inResult);
    manager.addItem(_inGroup, _outGrid, "Extracted grid");
}

void LVOX3_StepExtractCircularGrid::compute()
{

    for (CT_StandardItemGroup* group : _inGroup.iterateOutputs(_inResult))
    {
        if (isStopped()) {return;}

        const LVOX3_AbstractGrid3D* inGrid = group->singularItem(_inGrid);

        size_t counterVoxels = 0;
        size_t n_voxels;


        //If grid has data
        if (inGrid != NULL)
        {
            //Number of voxels depending on grid resolution
            n_voxels = (inGrid->xdim()*inGrid->ydim()*inGrid->zdim());

            qDebug()<< "inGrid not null";
            //If grid has dimensions and voxel resolution
            if((inGrid->xdim() > 0)
                    && (inGrid->ydim() > 0)
                    && (inGrid->zdim() > 0)
                    && (inGrid->xresolution() > 0)
                    && (inGrid->yresolution() > 0)
                    && (inGrid->zresolution() > 0)) {
                //Declaring output grid to be able to export personalized grid of profile
                lvox::Grid3Df *outGrid = new lvox::Grid3Df(inGrid->minX(), inGrid->minY(), inGrid->minZ(), inGrid->xdim(), inGrid->ydim(), inGrid->zdim(), inGrid->xresolution(),inGrid->yresolution(),inGrid->zresolution(), lvox::Max_Error_Code, 0);


                group->addSingularItem(_outGrid, outGrid);
                //Iterates through the 3d ingrid
                for(size_t col = 0;col <inGrid->xdim() && (!isStopped());col++){
                    for(size_t lin = 0;lin <inGrid->ydim() && (!isStopped());lin++){
                        for(size_t level = 0;level <inGrid->zdim() && (!isStopped());level++){
                            Eigen::Vector3d centerCoordVoxel;
                            size_t index = 0;
                            inGrid->index(col, lin, level, index);
                            double value = inGrid->valueAtIndexAsDouble(index);
                            inGrid->getCellCenterCoordinates(index,centerCoordVoxel);
                            //Affects values in the outGrid
                            if(evaluateVoxel(centerCoordVoxel, inGrid->xresolution(), inGrid->yresolution())){
                                outGrid->addValueAtIndex(index,value);
                            }else{
                                //set to sky if voxel is outside of the outGrid
                                //@TODO: may change to other value ?
                                outGrid->addValueAtIndex(index, lvox::ErrorOrWarningCode::Sky);
                            }
                            ++counterVoxels;
                            // progres de 0 à 100
                            setProgress((100.0*counterVoxels)/n_voxels);
                        }
                    }
                }
                outGrid->computeMinMax(); //computeMinMax at the end of the work on one grid to set visibility and colour coding
            }
        }
    }
}

//Test to see if any part of the voxel is inside the radius of the extracted grid (If it is, it is added to the extracted grid)
bool LVOX3_StepExtractCircularGrid::evaluateVoxel(Eigen::Vector3d centerCoords, double gridResolutionX, double gridResolutionY){

    //2D plane visualization of voxel square (topleft,topright,center,bottomleft,bottomright points)
    //If any of those points are in the distance, they are added to the grid.
    //The algorithm doesn't care if part of the voxel isn't in the zradius and another is,
    //because very big voxels will undoubtedly have a part that is outside
    double distance = sqrt(pow(_x-centerCoords(0),2.0)+pow(_y-centerCoords(1),2.0));
    if(distance>= _radiusmin && distance <= _radius){
        if(centerCoords(2)>=_zmin && centerCoords(2)<=_zmax)
            return true;
    }
    distance = sqrt(pow(_x-(centerCoords(0)-gridResolutionX),2.0)+pow(_y-(centerCoords(1)+gridResolutionY),2.0));
    if(distance>= _radiusmin && distance <= _radius){
        if(centerCoords(2)>=_zmin && centerCoords(2)<=_zmax)
            return true;
    }
    distance = sqrt(pow(_x-(centerCoords(0)+gridResolutionX),2.0)+pow(_y-(centerCoords(1)+gridResolutionY),2.0));
    if(distance>= _radiusmin && distance <= _radius){
        if(centerCoords(2)>=_zmin && centerCoords(2)<=_zmax)
            return true;
    }
    distance = sqrt(pow(_x-(centerCoords(0)-gridResolutionX),2.0)+pow(_y-(centerCoords(1)-gridResolutionY),2.0));
    if(distance>= _radiusmin && distance <= _radius){
        if(centerCoords(2)>=_zmin && centerCoords(2)<=_zmax)
            return true;
    }
    distance = sqrt(pow(_x-(centerCoords(0)+gridResolutionX),2.0)+pow(_y-(centerCoords(1)-gridResolutionY),2.0));
    if(distance>= _radiusmin && distance <= _radius){
        if(centerCoords(2)>=_zmin && centerCoords(2)<=_zmax)
            return true;
    }

    return false;
}
