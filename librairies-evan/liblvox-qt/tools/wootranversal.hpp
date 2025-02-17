#ifndef WOOTRANVERSAL_H
#define WOOTRANVERSAL_H

#include <functional>
#include <inttypes.h>
#include <vector>

#include "../datastructures/grid3d.h"
#include "../datastructures/ray.h"
#include "../tools/logger.h"
/**
 * @class WooTraversal
 * @brief A ray traversal algorithm for a 3D grid with a given visitor function.
 *
 * This class provides a ray traversal algorithm for a 3D grid. It traces a given ray through the grid
 * and applies a visitor function to each voxel that the ray intersects.
 *
 * @tparam T The data type of the grid elements.
 */
template <typename T> class WooTraversal
{
public:
    /**
     * @struct Context
     * @brief A context for storing information during the traversal.
     *
     * This structure represents a context for storing information during the ray traversal.
     * It contains data such as the current voxel index, column, line, and level of the current voxel,
     * the ray object being traversed, and the near and far impact points of the ray with the grid's bounding box.
     */
    struct Context
    {
        using Vector3SizeT = Eigen::Matrix<size_t, 3, 1>;

        Context(const lvox::Ray& ray_);

        size_t currentVoxelIndex; /*! voxel index in the grid */
        Vector3SizeT colLinLevel; /*! vector with X, Y, Z = column, line, z level of the current voxel in grid */
        const lvox::Ray& ray;     /*! the ray object */
        Eigen::Vector3d nearImpactPointWithGrid; /*! first impact point of the ray with the grid (bbox of the grid) */
        Eigen::Vector3d farImpactPointWithGrid;  /*! second impact point of the ray with the grid (bbox of the grid) */
    };

    using VisitorFunction = std::function<void(lvox::Grid3d<T>&, Context&)>; ///< Function type for the visitor.

    static const VisitorFunction CountVisitor;   ///< Visitor function for counting voxels intersected by the ray.
    static const VisitorFunction DistanceVisitor; ///< Visitor function for computing distances along the ray.

    /**
     * @brief Constructor for the WooTraversal class.
     * @param grid The 3D grid to be traversed.
     * @param compute The visitor function to be applied to each intersected voxel.
     * @param visitFirstVoxelTouched Flag to specify whether to visit the first voxel touched by the ray.
     */
    explicit WooTraversal(lvox::Grid3d<T>& grid, VisitorFunction& compute, bool visitFirstVoxelTouched, lvox::Logger* logger);

    /**
     * @brief Traverse the grid along the specified ray.
     * @param ray The ray object to be traced through the grid.
     */
    void traverse(const lvox::Ray& ray);

private:
    static const double MAX_DOUBLE_VALUE;  ///< Maximum double value used for initialization.
    static const uint8_t CHOOSE_AXIS[8];   ///< Array used for selecting the axis for the next step.

    lvox::Grid3d<T>& _grid;                 ///< Reference to the 3D grid to be traversed.
    VisitorFunction& _computeFunction;      ///< Reference to the visitor function to be applied.
    bool _visitFirstVoxelTouched;           ///< Flag to specify whether to visit the first voxel touched by the ray.
    lvox::Logger* _log;
};

template <typename T> WooTraversal<T>::Context::Context(const lvox::Ray& ray_) : ray(ray_)
{
}

template <typename T>
typename WooTraversal<T>::VisitorFunction const WooTraversal<T>::CountVisitor =
    [](lvox::Grid3d<T>& grid, Context& context) {
    grid.addValueAtIndex(context.currentVoxelIndex, 1);
};

template <typename T>
typename WooTraversal<T>::VisitorFunction const WooTraversal<T>::DistanceVisitor =
    [](lvox::Grid3d<T>& grid, Context& context) {

        grid.addValueAtIndex(context.currentVoxelIndex, 1);
        Eigen::Vector3d bot, top, nearInter, farInter;
        grid.computeCellBottomLeftTopRightCornerAtColLinLevel(context.colLinLevel.x(), context.colLinLevel.y(),
                                                              context.colLinLevel.z(), bot, top);

        if (context.ray.getIntersectionOfRay(bot, top, nearInter, farInter))
        {
            // qDebug()<<context.rayOrigin(0) << context.rayOrigin(1)<< context.rayOrigin(2) <<
            // context.rayDirection(0)<< context.rayDirection(1)<< context.rayDirection(2);

            const double distance = (nearInter - farInter).norm();

            grid.doSommation(context.currentVoxelIndex, distance, distance);
        }
    };

template <typename T> const uint8_t WooTraversal<T>::CHOOSE_AXIS[8] = {2, 1, 2, 1, 2, 2, 0, 0};

template <typename T> const double WooTraversal<T>::MAX_DOUBLE_VALUE = std::numeric_limits<double>::max();

template <typename T>
WooTraversal<T>::WooTraversal(lvox::Grid3d<T>& grid, VisitorFunction& compute, bool visitFirstVoxelTouched, lvox::Logger* logger)
    : _visitFirstVoxelTouched(visitFirstVoxelTouched), _grid(grid), _computeFunction(compute), _log(logger)
{
}

template <typename T> void WooTraversal<T>::traverse(const lvox::Ray& ray)
{
    //_log.logDebug("Woo - Start");
    Eigen::Vector3d gridBottom = _grid.getBoundingBox().first;
    Eigen::Vector3d gridTop = _grid.getBoundingBox().second;

    Eigen::Vector3d start, end;
    if (ray.intersect(gridBottom, gridTop, start, end) == false) return;
    // _log.logDebug("Woo - Passed Intersect");

    // std::stringstream fmt;
    // _log.logDebug(fmt.str());

    Context context(ray);
    context.nearImpactPointWithGrid = start;
    context.farImpactPointWithGrid = end;

    // fmt << start(0) << " " << start(1) << " " << start(2) << " " << end(0) << " " << end(1) << " " << end(2) << " ";
    // _log.logDebug(fmt.str());

    _grid.computeGridIndexForPoint(context.nearImpactPointWithGrid,
                             context.colLinLevel.x(),
                             context.colLinLevel.y(),
                             context.colLinLevel.z(),
                             context.currentVoxelIndex);

    // fmt << context.currentVoxelIndex << " "
    //     << context.colLinLevel.x() << " "
    //     << context.colLinLevel.y() << " "
    //     << context.colLinLevel.z() << " "

    //     << context.nearImpactPointWithGrid(0) << " "
    //     << context.nearImpactPointWithGrid(1) << " "
    //     << context.nearImpactPointWithGrid(2) << " "

    //     << _grid.getMinX() << " "
    //     << _grid.getMinY() << " "
    //     << _grid.getMinZ() << " "

    //     << _grid.getDimensionX() << " "
    //     << _grid.getDimensionY() << " "
    //     << _grid.getDimensionZ() << " "
    //     << _grid.getResolution() << " ";




    Eigen::Vector3d stepAxis, boundary, tMax, tDel;

    for (int i = 0; i < 3; ++i)
    {
        if (context.farImpactPointWithGrid(i) > context.nearImpactPointWithGrid(i))
        {
            stepAxis(i) = 1;
            boundary(i) = ((context.colLinLevel(i) + 1) * _grid.getResolution()) + gridBottom(i);
        }
        else
        {
            stepAxis(i) = -1;
            boundary(i) = (context.colLinLevel(i) * _grid.getResolution()) + gridBottom(i);
        }

        if (ray.getDirection()(i) != 0)
        {
            tMax(i) = fabs((boundary(i) - context.nearImpactPointWithGrid(i)) / ray.getDirection()(i));
            tDel(i) = fabs(_grid.getResolution() / ray.getDirection()(i));
        }
        else
        {
            tMax(i) = MAX_DOUBLE_VALUE;
            tDel(i) = MAX_DOUBLE_VALUE;
        }
    }

    if (_visitFirstVoxelTouched)
    {
        if (!_grid.isFiltered(_grid.getValueAtIndex(context.currentVoxelIndex)))
        {
            // fmt << context.currentVoxelIndex << " ";
            _computeFunction(_grid, context);
        }
        else
        {
            return;
        }
    }

    while (1)
    {
        // Finds along which axis to do the next step
        const uint8_t bits = ((tMax(0) < tMax(1)) << 2)
                           + ((tMax(0) < tMax(2)) << 1)
                           + ((tMax(1) < tMax(2)));
        const uint8_t nextStepAxis = CHOOSE_AXIS[bits];
        context.colLinLevel(nextStepAxis) += stepAxis(nextStepAxis);

        if (context.colLinLevel.x() >= _grid.getDimensionX())
        {
            //_log.logDebug(fmt.str());
            return;
        }
        if (context.colLinLevel.y() >= _grid.getDimensionY())
        {
            //_log.logDebug(fmt.str());
            return;
        }
        if (context.colLinLevel.z() >= _grid.getDimensionZ())
        {
            //_log.logDebug(fmt.str());
            return;
        }

        // Add the index of the voxel to the list
        context.currentVoxelIndex = _grid.computeGridIndexForColLinLevel(
            context.colLinLevel.x(), context.colLinLevel.y(), context.colLinLevel.z());

        if (!_grid.isFiltered(_grid.getValueAtIndex(context.currentVoxelIndex)))
        {
            // fmt << context.currentVoxelIndex << " ";
            _computeFunction(_grid, context);
        }
        else
        {
            return;
        }

        // Updating tmax of this axis (increasing by deltaT)
        tMax(nextStepAxis) = tMax(nextStepAxis) + tDel(nextStepAxis);
    }
};

#endif // WOOTRANVERSAL_H
