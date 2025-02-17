#ifndef LVOX3_IMPLGRID3D_H
#define LVOX3_IMPLGRID3D_H

#include <liblvox/datastructures/grid3d.h>
#include "tools/3dgrid/lvox3_grid3d.h"

template<typename DataT>
class LVOX3_ImplGrid3d : public lvox::Grid3d<DataT>
{

public:
    LVOX3_ImplGrid3d(LVOX3_Grid3D<DataT>* grid) : _grid(grid) {};

    double getResolution() const override { return _grid->xresolution(); }
    // double getResolutionY() const override { return _grid->yresolution(); }
    // double getResolutionZ() const override { return _grid->zresolution(); }
    size_t getDimensionX() const override { return _grid->xdim(); };
    size_t getDimensionY() const override { return _grid->ydim(); };
    size_t getDimensionZ() const override { return _grid->zdim(); };

    DataT getNAValue() const override { return _grid->NA(); }
    float getLambda1() const override { return _grid->getLambda1(); };
    void setLambda1(double elementProjectedArea) override { _grid->setLambda1(static_cast<float>(elementProjectedArea)); }
    size_t cellCount() const override { return _grid->nCells(); };
    DataT getValueAtIndex(size_t index) const override { return _grid->valueAtIndex(index); } ;
    DataT getValueAtCoordinates(int x, int y, int z) const override { return _grid->valueAtXYZ(x, y, z); };

    Eigen::Vector3d getCellCenterCoordinates(size_t index) const override { Eigen::Vector3d coords; _grid->getCellCenterCoordinates(index, coords); return coords; };

    std::pair<Eigen::Vector3d,Eigen::Vector3d> getBoundingBox() const override { return { _grid->minCoordinates(), _grid->maxCoordinates() }; }
    void resizeGrid(Eigen::Vector3d& min, Eigen::Vector3d& max, double resolution) override
    {
        _grid = LVOX3_Grid3D<DataT>::createGrid3DFromXYZCoords(min.x(), min.y(), min.z(), max.x(), max.y(), max.z(), resolution, resolution, resolution, -9, 0);
    }
    double getMinX() const override { return _grid->minX(); }
    double getMinY() const override { return _grid->minY(); }
    double getMinZ() const override { return _grid->minZ(); }

    double getMaxX() const override { return _grid->maxX(); };
    double getMaxY() const override { return _grid->maxY(); };
    double getMaxZ() const override { return _grid->maxZ(); };

    void addValueAtIndex(size_t index, DataT value) override {
        //qDebug() << "Add "<< value << " at " << index;
        _grid->addValueAtIndex(index, value); }
    void addValueAtCoordinates(double x, double y, double z, DataT value) override { _grid->addValueAtXYZ(x, y, z, value); }

    void setValueAtIndex(size_t index, DataT value) override { _grid->setValueAtIndex(index, value); }
    void setValueAtCoordinates(double x, double y, double z, DataT value) override { _grid->setValue(x, y, z, value); }

    void addSommationDelta(size_t index, double delta) override { _grid->addSommaDelta(index, static_cast<float>(delta)); }
    void addSommationFree(size_t index, double free) override { _grid->addSommaFree(index, static_cast<float>(free)); }
    void addSommationEffectiveDelta(size_t index, double effectiveDelta) override { _grid->addSommaEffectiveDelta(index, static_cast<float>(effectiveDelta)); }
    void addSommationEffectiveHits(size_t index, double effectiveHits) override { _grid->addSommaEffectiveHits(index, static_cast<float>(effectiveHits)); }
    void addSommationEffectiveFree(size_t index, double effectiveFree) override { _grid->addSommaEffectiveFree(index, static_cast<float>(effectiveFree)); }
    void addSommationDeltaSquare(size_t index, double deltaSquare) override { _grid->addSommaDeltaSquare(index, static_cast<float>(deltaSquare)); }

    float getSommationDeltaAtIndex(size_t index) override { return _grid->getSommaDelta(index); }
    float getSommationFreeAtIndex(size_t index) override { return _grid->getSommaFree(index); }
    float getSommationEffectiveDeltaAtIndex(size_t index) override { return _grid->getSommaEffectiveDelta(index); }
    float getSommationEffectiveHitsAtIndex(size_t index) override { return _grid->getSommaEffectiveHits(index); }
    float getSommationEffectiveFreeAtIndex(size_t index) override { return _grid->getSommaEffectiveFree(index); }
    float getSommationDeltaSquareAtIndex(size_t index) override { return _grid->getSommaDeltaSquare(index); }
    LVOX3_Grid3D<DataT>* getMemberGridType() { return _grid; }

private:
    LVOX3_Grid3D<DataT>* _grid;
};

#endif // LVOX3_IMPLGRID3D_H
