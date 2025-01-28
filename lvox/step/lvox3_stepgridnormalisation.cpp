#include "lvox3_stepgridnormalisation.h"

LVOX3_StepGridNormalisation::LVOX3_StepGridNormalisation() : CT_AbstractStep()
{

}

QString LVOX3_StepGridNormalisation::description() const
{
    return tr("Normalisation des hauteur d'une grille (en developpement)");
}

// Step detailed description
QString LVOX3_StepGridNormalisation::detailledDescription() const
{
    return tr("Cette étape permet diminuer la hauteur des voxel d'une grille selon un raster de hauteur. "
              "À noter que les paramètres devraient être les mêmes pour la grille 3D importée et les grilles 3D calculées présentement pour empêcher des résultats incohérents. (résolution de grille et dimension)");
}

CT_VirtualAbstractStep* LVOX3_StepGridNormalisation::createNewInstance() const
{
    // Creates an instance of this step
    return new LVOX3_StepGridNormalisation();
}


void LVOX3_StepGridNormalisation::declareInputModels(CT_StepInModelStructureManager& manager)
{
    manager.addResult(_inResult, "Grids", "", true);
    manager.setZeroOrMoreRootGroup(_inResult, _inZeroOrMoreRootGroup);
    manager.addGroup(_inZeroOrMoreRootGroup, _inGroup);
    manager.addItem(_inGroup, _inGrid, "Grids to nomalize");

    manager.addResult(_inResultDTM, tr("Grille de MNT (.GRD3DLVOX)"), "", true);
    manager.setZeroOrMoreRootGroup(_inResultDTM, _inZeroOrMoreRootGroupDTM);
    manager.addGroup(_inZeroOrMoreRootGroupDTM, _inGroupDTM);
    manager.addItem(_inGroupDTM, _inGridDTM, tr("Grille MNT"));
}

void LVOX3_StepGridNormalisation::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    manager.addResultCopy(_inResult);
    manager.addItem(_inGroup, _NORM, tr("Grille normalisée"));
    manager.addItem(_inGroup, _ELevation, tr("Grille d'élévation"));
}

void LVOX3_StepGridNormalisation::compute()
{
    QList<const LVOX3_AbstractGrid3D*> dtms;
    for (CT_StandardItemGroup* groupDTM : _inGroupDTM.iterateOutputs(_inResultDTM))
    {
        dtms.append(groupDTM->singularItem(_inGridDTM));
    }

    int counter = 0;
    for (CT_StandardItemGroup* group : _inGroup.iterateOutputs(_inResult))
    {
        if (isStopped()) {return;}

        const LVOX3_AbstractGrid3D* inGrid = group->singularItem(_inGrid);
        const LVOX3_AbstractGrid3D* dtmGrid = dtms.at(counter++);

        float xres = inGrid->xresolution();
        float yres = inGrid->yresolution();
        float zres = inGrid->zresolution();
        float NAd = 0;

        size_t xdim = inGrid->xdim();
        size_t ydim = inGrid->ydim();
        size_t zdim = inGrid->zdim();

        float NAin = inGrid->NAAsString().toFloat();


        lvox::Grid3Df* itemoutiConteur = new lvox::Grid3Df(inGrid->minX(), inGrid->minY(), inGrid->minZ(), xdim, ydim, zdim, xres, yres, zres, NAin, NAin);
        lvox::Grid3Df* elevationGrid = new lvox::Grid3Df(inGrid->minX(), inGrid->minY(), inGrid->minZ(), xdim, ydim, zdim, xres, yres, zres, NAd, NAd);

        group->addSingularItem(_NORM, itemoutiConteur);
        group->addSingularItem(_ELevation, elevationGrid);

        //all voxel in dtm have the same z coordinate !!!
        double coordZ = dtmGrid->valueAtIndexAsDouble(0);
        double minZ = inGrid->minZ();
        double minDtmZ = std::numeric_limits<double>::max();
        //std::vector<bool> isNotEmptyColumns(xdim*ydim, false);
        for(size_t xx = 0; xx < xdim; xx++)
        {
            for(size_t yy = 0; yy < ydim; yy++)
            {
                //nombre de voxels
                //int nbValeur = div(MNT->valueAtCoords(xx,yy),zres).quot;
                //find first non null
                double coordX = inGrid->getCellCenterX(xx);
                double coordY = inGrid->getCellCenterY(yy);

                size_t dtmIndex;
                dtmGrid->indexAtXYZ(coordX, coordY, coordZ, dtmIndex);

                double dtmZ = dtmGrid->valueAtIndexAsDouble(dtmIndex);

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
                        //isNotEmptyColumns[xx*ydim + yy] = true;
                        break;
                    }
                }
                int diff = (dtmZ - minZ)/zres;
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

                size_t dtmIndex;
                dtmGrid->indexAtXYZ(coordX, coordY, coordZ, dtmIndex);

                double dtmZ = dtmGrid->valueAtIndexAsDouble(dtmIndex);

                size_t minZIndex = 0;
                inGrid->index(xx, yy, 0, minZIndex);
                //double minZ = inGrid->valueAtIndexAsDouble(minZIndex);
                //compute the number of voxel must be descended
                int beginZlevel = (minDtmZ - minZ)/zres;
                int diff = (dtmZ - minDtmZ)/zres;
                //qDebug()<<"###diff"<< diff<< "minZ" << minZ << "dtmZ"<< dtmZ << "minDtm" << minDtmZ;
                if(diff > 0){
                    for(size_t zz = 0; zz < diff; zz++)
                    {
                        if( zz + beginZlevel < zdim){
                            elevationGrid->setValue(xx, yy, zz + beginZlevel, 1);
                        }
                    }
                    for(size_t zz = beginZlevel + diff; zz < zdim; zz++)
                    {
                        size_t index = 0;
                        inGrid->index(xx, yy, zz, index);
                        float currentValue = inGrid->valueAtIndexAsDouble(index);
                        itemoutiConteur->setValue(xx, yy, zz - diff, currentValue);
                        if (zdim - zz >= diff) {
                            itemoutiConteur->setValue(xx, yy, zz, -1);
                        }
                    }
                }
            }
        }

        itemoutiConteur->computeMinMax();
        elevationGrid->computeMinMax();
    }

}
