#include "lvox3_stepgridnormalisationraster.h"
#include "ct_log/ct_logmanager.h"

LVOX3_StepGridNormalisationRaster::LVOX3_StepGridNormalisationRaster() : CT_AbstractStep()
{

}

QString LVOX3_StepGridNormalisationRaster::description() const
{
    return tr("Height normalization with DTM (WIP)");
}

// Step detailed description
QString LVOX3_StepGridNormalisationRaster::detailledDescription() const
{
    return tr("This step allows for reducing the voxels' height with respect to a DTM." "<br>"
              "<strong>Note :</strong> Both grids (imported and computed here) should have their parameters equal (namely dimension and resolution) to avoid incoherent results.");
}

CT_VirtualAbstractStep* LVOX3_StepGridNormalisationRaster::createNewInstance() const
{
    // Creates an instance of this step
    return new LVOX3_StepGridNormalisationRaster();
}

QString LVOX3_StepGridNormalisationRaster::outputDescription() const
{
    auto indent = [](QString s) -> QString { return "<div style=\"margin: 0 0 0 40px;\">" + s+ "</div>"; };

    return CT_AbstractStep::outputDescription() + "<br>Group [Group]:" +
            indent("...") +
            indent("LVOX3_Grid3D<float> [Normalized grid]");
}


void LVOX3_StepGridNormalisationRaster::declareInputModels(CT_StepInModelStructureManager& manager)
{
    manager.addResult(_inResult, "Grids", "", true);
    manager.setZeroOrMoreRootGroup(_inResult, _inZeroOrMoreRootGroup);
    manager.addGroup(_inZeroOrMoreRootGroup, _inGroup);
    manager.addItem(_inGroup, _inGrid, "Input grid");

    manager.addResult(_inResultDTM, "DTM", "", true);
    manager.setZeroOrMoreRootGroup(_inResultDTM, _inZeroOrMoreRootGroupDTM);
    manager.addGroup(_inZeroOrMoreRootGroupDTM, _inGroupDTM);
    manager.addItem(_inGroupDTM, _inDTM, "DTM");
}


void LVOX3_StepGridNormalisationRaster::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    manager.addResultCopy(_inResult);
    manager.addItem(_inGroup, _NORM, "Normalized grid");
}

void LVOX3_StepGridNormalisationRaster::compute()
{
    QList<const CT_Image2D<float>*> dtms;
    for (const CT_StandardItemGroup* groupDTM : _inGroupDTM.iterateInputs(_inResultDTM))
    {
        dtms.append(groupDTM->singularItem(_inDTM));
    }

    int counter = 0;
    for (CT_StandardItemGroup* group : _inGroup.iterateOutputs(_inResult))
    {
        if (isStopped()) {return;}

        const CT_AbstractGrid3D* inGrid = group->singularItem(_inGrid);

        if (inGrid != nullptr)
        {
            const CT_Image2D<float>* dtm = dtms.at(counter++);

            const size_t xdim = inGrid->xdim();
            const size_t ydim = inGrid->ydim();
            const size_t zdim = inGrid->zdim();
            const double res = inGrid->resolution();

            float NAin = inGrid->NAAsString().toFloat();

            lvox::Grid3Df* itemoutiConteur = new lvox::Grid3Df(
                    inGrid->minX(),
                    inGrid->minY(),
                    inGrid->minZ(),
                    static_cast<int>(xdim),
                    static_cast<int>(ydim),
                    static_cast<int>(zdim),
                    res,
                    NAin,
                    NAin);

            group->addSingularItem(_NORM, itemoutiConteur);

            //all voxel in dtm have the same z coordinate !!!
            double minZ = inGrid->minZ();
            double minDtmZ = std::numeric_limits<double>::max();
            //std::vector<bool> isNotEmptyColumns(xdim*ydim, false);
            for(size_t xx = 0; xx < xdim; xx++)
            {
                for(size_t yy = 0; yy < ydim; yy++)
                {
                    //nombre de voxels
                    //int nbValeur = div(MNT->valueAtCoords(xx,yy),res).quot;
                    //find first non null
                    double coordX = inGrid->getCellCenterX(xx);
                    double coordY = inGrid->getCellCenterY(yy);
                    float dtmZ = dtm->valueAtCoords(coordX, coordY);

                    //                size_t minZIndex = 0;
                    //                inGrid->index(xx, yy, 0, minZIndex);
                    //double minZ = inGrid->valueAtIndexAsDouble(minZIndex);
                    //compute the number of voxel must be descended
                    bool isNotEmptyColumn = false;

                    for(size_t zz = 0; zz < zdim; zz++)
                    {
                        size_t index = 0;
                        inGrid->index(xx, yy, zz, index);
                        if (inGrid->valueAtIndexAsDouble(index) >= 0){
                            isNotEmptyColumn = true;
                            break;
                        }
                    }
                    int diff = (dtmZ - minZ)/res;
                    if(diff > 0 && isNotEmptyColumn){
                        if( dtmZ < minDtmZ ){
                            minDtmZ = dtmZ;
                        }
                    }
                }
            }

            //correction of height
            for(size_t xx = 0; xx < xdim; xx++)
            {
                for(size_t yy = 0; yy < ydim; yy++)
                {
                    double coordX = inGrid->getCellCenterX(xx);
                    double coordY = inGrid->getCellCenterY(yy);
                    float dtmZ = dtm->valueAtCoords(coordX, coordY);

                    size_t minZIndex = 0;
                    inGrid->index(xx, yy, 0, minZIndex);
                    //double minZ = inGrid->valueAtIndexAsDouble(minZIndex);
                    //compute the number of voxel must be descended
                    int beginZlevel = (minDtmZ - minZ)/res;
                    int diff = (dtmZ - minDtmZ)/res;
                    //qDebug()<<"###diff"<< diff<< "minZ" << minZ << "dtmZ"<< dtmZ << "minDtm" << minDtmZ;
                    if(diff > 0){
                        for(size_t zz = beginZlevel + diff; zz < zdim; zz++)
                        {
                            size_t index = 0;
                            inGrid->index(xx, yy, zz, index);
                            float currentValue = inGrid->valueAtIndexAsDouble(index);
                            itemoutiConteur->setValue(xx, yy, zz - diff, currentValue);
                            if( static_cast<int>(zdim - zz) >= diff){
                                itemoutiConteur->setValue(xx, yy, zz, -1);
                            }
                        }
                    }
                }
            }

            itemoutiConteur->computeMinMax();
        } else {
            PS_LOG->addWarningMessage(LogInterface::step, tr("Erreur : Grille Nulle"));
        }
    }
}
