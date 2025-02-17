/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_TRUSTINTERPOLATIONVISITOR_H
#define LVOX3_TRUSTINTERPOLATIONVISITOR_H

#include "../../../datastructures/types.h"
#include "propagationvisitor.h"
#include <inttypes.h>

/**
 * @class TrustInterpolationVisitor
 * @brief Use this visitor to interpolate a grid with a trust factor based on effective rays count.
 *
 * The interpolation formula used is:
 *   Sum(IDR * TF)
 *   -------------
 *     Sum(TF)
 *
 * Where:
 *   - IDR: value of density of the cell inspected.
 *   - TF: trust factor calculated based on effective rays count (Nt - Nb).
 *
 * The trust factor (TF) is determined as follows:
 *   - If (Nt - Nb) <= effectiveRayThreshold, TF = 0.
 *   - If (Nt - Nb) > endRayThreshold, TF = 1.
 *   - For effectiveRayThreshold < (Nb - Nt) < endRayThreshold, TF = sin(((Nb - Nt) - effectiveRayThreshold) / (endRayThreshold - effectiveRayThreshold)).
 */
class TrustInterpolationVisitor : public PropagationVisitor
{
public:
    /**
     * @brief Constructor for TrustInterpolationVisitor.
     * @param inDensityGrid The grid to modify (input density grid).
     * @param outDensityGrid The grid where the interpolated density values will be stored.
     * @param inBeforeGrid The grid representing the sum of blocked rays.
     * @param inTheoreticalGrid The grid representing the number of theoretical rays.
     * @param effectiveRayThreshold The minimum number of effective rays (Nt - Nb) required to have a trust factor > 0.
     * @param endRayThreshold The maximum number of effective rays. If (Nt - Nb) > endRayThreshold, the trust factor will be 1.
     */
    TrustInterpolationVisitor(const Grid3Df& inDensityGrid, Grid3Df& outDensityGrid, const Grid3Di& inBeforeGrid,
                              const Grid3Di& inTheoreticalGrid, int32_t effectiveRayThreshold,
                              int32_t endRayThreshold);

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

    /**
     * @brief Main trust factor computation.
     * @param Nt The theoretical rays count.
     * @param Nb The blocked rays count.
     * @param effectiveRayThreshold The minimum number of effective rays.
     * @param endRayThreshold The threshold for fully trusted value.
     * @param diffEffectiveRay The difference between endRayThreshold and effectiveRayThreshold.
     * @return The calculated trust factor (TF).
     */
    static double getTrustFactor(const int32_t& Nt, const int32_t& Nb, const int32_t& effectiveRayThreshold,
                                 const int32_t& endRayThreshold, const int32_t& diffEffectiveRay);

private:
    const Grid3Df& m_inGrid;
    Grid3Df& m_outGrid;
    const Grid3Di& m_beforeGrid;
    const Grid3Di& m_theoriticalsGrid;
    int32_t m_effectiveRayThreshold;
    int32_t m_endRayThreshold;
    int32_t m_diffEffectiveRay;
    double m_denominator;
    double m_numerator;
};

#endif // LVOX3_TRUSTINTERPOLATIONVISITOR_H
