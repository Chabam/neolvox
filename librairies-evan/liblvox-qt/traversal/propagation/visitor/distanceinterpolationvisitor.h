/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_DISTANCEINTERPOLATIONVISITOR_H
#define LVOX3_DISTANCEINTERPOLATIONVISITOR_H

#include "../../../datastructures/types.h"
#include "propagationvisitor.h"

/**
 * @class DistanceInterpolationVisitor
 * @brief A visitor used to interpolate a grid with a distance factor.
 *
 * This visitor performs interpolation on a grid using a distance factor. The formula used for interpolation is:
 * Sum(IDR / D^p)
 * --------------------
 * Sum(1 / D^p)
 *
 * Where:
 * - "IDR" is the value of the density of the cell inspected.
 * - "D" is the distance between the first cell and the visited cell.
 * - "p" is the power value defined in the constructor.
 */
class DistanceInterpolationVisitor : public PropagationVisitor
{
public:
    /**
     * @brief Constructor for the DistanceInterpolationVisitor class.
     * @param inDensityGrid The input density grid used for interpolation.
     * @param outDensityGrid The output density grid where the interpolation result will be stored.
     * @param power The power value used in the interpolation formula.
     * @param densityThreshold The density threshold for cells to be considered in the interpolation.
     */
    DistanceInterpolationVisitor(const Grid3Df& inDensityGrid, Grid3Df& outDensityGrid, int power, float densityThreshold);

    /**
     * @brief Called when the propagation starts.
     * @param context Will contain all elements computed by the algorithm when it starts and contains
     *                information of the first cell.
     */
    void start(const PropagationVisitorContext& context);

    /**
     * @brief Called when a voxel must be visited.
     * @param context Will contain all elements computed by the algorithm when it starts and contains
     *                information of the current cell.
     */
    void visit(const PropagationVisitorContext& context);

    /**
     * @brief Called when the propagation is finished.
     * @param context Will contain all elements computed by the algorithm when it starts and contains
     *                information of the first cell.
     */
    void finish(const PropagationVisitorContext& context);

private:
    const Grid3Df& m_inGrid;         ///< Reference to the input density grid.
    Grid3Df& m_outGrid;              ///< Reference to the output density grid.
    const int m_power;               ///< The power value used in the interpolation formula.
    const float m_densityThreshold;  ///< The density threshold for cells to be considered in the interpolation.
    double m_numerator;              ///< Numerator used in the interpolation formula.
    double m_denominator;            ///< Denominator used in the interpolation formula.
};
#endif // LVOX3_DISTANCEINTERPOLATIONVISITOR_H
