#include "lvox3_filtervoxelsbyzvaluesofraster.h"

//Tools
#include "tools/lvox3_gridtools.h"
#include "tools/lvox3_errorcode.h"

//This class is a legacy class used to filtering voxels either with a DTM or a Sky raster(or any raster)
LVOX3_FilterVoxelsByZValuesOfRaster::LVOX3_FilterVoxelsByZValuesOfRaster(const QList<LVOX3_AbstractGrid3D *> &grids,
                                                               const CT_AbstractImage2D *zValues,
                                                               FilterType filter,
                                                               double replacementValue) : LVOX3_Worker()
{
    m_grids = grids.toVector();
    m_zValues = (CT_AbstractImage2D*)zValues;
    m_filter = filter;
    m_replacementValue = replacementValue;
}

void LVOX3_FilterVoxelsByZValuesOfRaster::doTheJob()
{
    int nGrids = m_grids.size();

    if(nGrids == 0)
        return;

    LVOX3_AbstractGrid3D* refGrid = m_grids[0];

    size_t nCells = refGrid->nCells();

    if(nCells == 0)
        return;

    Eigen::Vector3d cellTopCoordinate;
    LVOX3_GridTools gridTools(refGrid);

    double NAValue = m_zValues->NAAsDouble();
    size_t index = 0;
    int j;
    bool ok;

    for(size_t i=0; i<nCells; ++i) {
        gridTools.computeCellTopMiddleCoordsAtIndex(i, cellTopCoordinate);

        m_zValues->indexAtCoords(cellTopCoordinate.x(), cellTopCoordinate.y(), index);
        const double zValue = m_zValues->valueAtIndexAsDouble(index); // TODO : bug in image 2D => return not Nan but quiet_nan !!!

        if(zValue != NAValue) {
            if(m_filter == Below) {
                ok = cellTopCoordinate.z() < zValue;
            } else {
                ok = cellTopCoordinate.z() > zValue;
            }

            if(ok) {
                for(j=0; j<nGrids; ++j)
                    m_grids[j]->setValueAtIndexFromDouble(i, m_replacementValue);
            }
        }
    }
}
