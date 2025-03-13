/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_DISTANCEINTERPOLATIONVISITOR_H
#define LVOX3_DISTANCEINTERPOLATIONVISITOR_H

#include "lvox3_propagationvisitor.h"
#include "tools/lvox3_gridtype.h"

/**
 * @brief Use this visitor to interpolate a grid with a distance factor.
 *
 * The formula used is : Sum(IDR / D^p)
 *                       --------------------
 *                       Sum(1 / D^p)
 *
 * "IDR" = value of density of the cell inspected
 * "D" the distance between the first cell and the visited cell
 * "p" the power value defined in constructor
 */
class LVOX3_DistanceInterpolationVisitor : public LVOX3_PropagationVisitor
{
public:
    LVOX3_DistanceInterpolationVisitor(const lvox::Grid3Df* inDensityGrid,
                                       lvox::Grid3Df* const outDensityGrid,
                                       int power,
                                       float densityThreshold);

    /**
     * @brief Called when the propagation start
     * @param context : will contains all elements compute by the algorithm  when it will start. Contains
     *                  information of the first cell
     */
    void start(const LVOX3_PropagationVisitorContext& context);

    /**
     * @brief Called when a voxel must be visited
     * @param context : will contains all elements compute by the algorithm  when it will start. Contains
     *                  information of the current cell
     */
    void visit(const LVOX3_PropagationVisitorContext& context);

    /**
     * @brief Called when the propagation was finished
     * @param context : will contains all elements compute by the algorithm  when it will start. Contains
     *                  information of the first cell
     */
    void finish(const LVOX3_PropagationVisitorContext& context);

private:
    const lvox::Grid3Df*    m_inGrid;
    lvox::Grid3Df* const    m_outGrid;
    const int               m_power;
    const float             m_densityThreshold;
    double                  m_numerator;
    double                  m_denominator;
};

#endif // LVOX3_DISTANCEINTERPOLATIONVISITOR_H
