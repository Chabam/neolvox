#ifndef LVOX3_ABSTRACTIMPLGRID3D_H
#define LVOX3_ABSTRACTIMPLGRID3D_H

#include "liblvox/datastructures/types.h"
#include "tools/3dgrid/lvox3_grid3d.h"

template <typename DataT>
class LVOX3_ImplGrid3D : lvox::Grid3d<DataT>
{

public:
    LVOX3_ImplGrid3D();
    /** Getters */
    float getResolution() const;
    Grid3Di::GridTypeT getNAValue() const;
    float getLambda1() const;
    size_t getDimension() const;
    size_t cellCount() const;
    Grid3Di::GridTypeT getValueAtIndex(size_t index) const;
    Grid3Di::GridTypeT getValueAtCoordinates(int x, int y, int z);
    Eigen::Vector3d getCellCenterCoordinates(size_t index) const;
    Eigen::Vector3d getCellCenterCoordinates(size_t col, size_t line, size_t level) const;

    Grid3Di::GridTypeT getMinX() const;
    Grid3Di::GridTypeT getMinY() const;
    Grid3Di::GridTypeT getMinZ() const;

    Grid3Di::GridTypeT getMaxX() const;
    Grid3Di::GridTypeT getMaxY() const;
    Grid3Di::GridTypeT getMaxZ() const;

    Eigen::AlignedBox3d getBoundingBox() const;

    bool isValueFiltered(size_t index);
    void addValueAtIndex(size_t index, Grid3Di::GridTypeT value);
    void setValueAtIndex(size_t index, Grid3Di::GridTypeT value);
    void setValueAtCoordinates(int x, int y, int z, Grid3Di::GridTypeT value);
    void computeDistances(size_t index, float delta, float free);

    void addSommationDelta(size_t index, double delta);
    void addSommationFree(size_t index, double free);
    void addSommationEffectiveDelta(size_t index, double effectiveDelta);
    void addSommationEffectiveHits(size_t index, double effectiveHits);
    void addSommationEffectiveFree(size_t index, double effectiveFree);
    void addSommationDeltaSquare(size_t index, double deltaSquare);

private:
    LVOX3_Grid3D<Grid3Di::GridTypeT>* _grid;
};

template <typename DataT>
LVOX3_ImplGrid3D<DataT>::LVOX3_ImplGrid3D()
{

}

/** Getters */
template <typename DataT>
float LVOX3_ImplGrid3D<DataT>::getResolution() const
{
    return _grid->resolution();
}

template <typename DataT>
Grid3Di::GridTypeT LVOX3_ImplGrid3D<DataT>::getNAValue() const
{
    return _grid->NA();
}

template <typename DataT>
float LVOX3_ImplGrid3D<DataT>::getLambda1() const
{
    return _grid->getLambda1();
}
size_t LVOX3_ImplGrid3D::getDimension() const
{
    //TODO
    return 0;
}

size_t LVOX3_ImplGrid3D::cellCount() const
{
    return _grid->nCells();
}

Grid3Di::GridTypeT LVOX3_ImplGrid3D::getValueAtIndex(size_t index) const
{
    return _grid->valueAtIndex(index);
}

Grid3Di::GridTypeT LVOX3_ImplGrid3D::getValueAtCoordinates(int x, int y, int z)
{
    return _grid->valueAtXYZ(x,y,z);
}

Eigen::Vector3d LVOX3_ImplGrid3D::getCellCenterCoordinates(size_t index) const
{
    Eigen::Vector3d ret;
    _grid->getCellCenterCoordinates(index, ret);
    return ret;
}

Eigen::Vector3d LVOX3_ImplGrid3D::getCellCenterCoordinates(size_t col, size_t line, size_t level) const
{
    //TODO : Check original impl

    return Eigen::Vector3d{};
}

Grid3Di::GridTypeT LVOX3_ImplGrid3D::getMinX() const
{
    return _grid->minX();
}
Grid3Di::GridTypeT LVOX3_ImplGrid3D::getMinY() const
{
    return _grid->minY();
}
Grid3Di::GridTypeT LVOX3_ImplGrid3D::getMinZ() const
{
    return _grid->minZ();
}

Grid3Di::GridTypeT LVOX3_ImplGrid3D::getMaxX() const
{
    return _grid->maxX();
}
Grid3Di::GridTypeT LVOX3_ImplGrid3D::getMaxY() const
{
    return _grid->maxY();
}
Grid3Di::GridTypeT LVOX3_ImplGrid3D::getMaxZ() const
{
    return _grid->maxZ();
}

Eigen::AlignedBox3d LVOX3_ImplGrid3D::getBoundingBox() const
{
    Eigen::Vector3d bottom, top;
    _grid->boundingBox(bottom, top);

    Eigen::AlignedBox3d box {bottom, top};

    return box;
}

bool LVOX3_ImplGrid3D::isValueFiltered(size_t index)
{
    return isFiltered(_grid->valueAtIndex(index));
}
void LVOX3_ImplGrid3D::addValueAtIndex(size_t index, Grid3Di::GridTypeT value)
{
    _grid->addValueAtIndex(index, value);
}

void LVOX3_ImplGrid3D::setValueAtIndex(size_t index, Grid3Di::GridTypeT value)
{
    _grid->setValueAtIndex(index, value);
}

void LVOX3_ImplGrid3D::setValueAtCoordinates(int x, int y, int z, Grid3Di::GridTypeT value)
{
    _grid->setValueAtXYZ(x,y,z, value);
}

void LVOX3_ImplGrid3D::addSommationDelta(size_t index, double delta)
{
    _grid->addSommaDelta(index, delta);
}

void LVOX3_ImplGrid3D::addSommationFree(size_t index, double free)
{
    _grid->addSommaFree(index, free);
}

void LVOX3_ImplGrid3D::addSommationEffectiveDelta(size_t index, double effectiveDelta)
{
    _grid->addSommaEffectiveDelta(index, effectiveDelta);
}

void LVOX3_ImplGrid3D::addSommationEffectiveHits(size_t index, double effectiveHits)
{
    _grid->addSommaEffectiveHits(index, effectiveHits);
}

void LVOX3_ImplGrid3D::addSommationEffectiveFree(size_t index, double effectiveFree)
{
    _grid->addSommaEffectiveFree(index, effectiveFree);
}
void LVOX3_ImplGrid3D::addSommationDeltaSquare(size_t index, double deltaSquare)
{
    _grid->addSommaDeltaSquare(index, deltaSquare);
}

#endif // LVOX3_ABSTRACTIMPLGRID3D_H
