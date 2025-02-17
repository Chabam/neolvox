/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_PROPAGATIONVISITOR_H
#define LVOX3_PROPAGATIONVISITOR_H

#include "propagationvisitorcontext.h"

/**
 * @class PropagationVisitor
 * @brief Inherit from this class to perform custom actions when a cell is visited by the woo algorithm
 * implemented in the class LVOX3_Grid3DPropagationAlgorithm.
 */
class PropagationVisitor
{
public:
    /**
     * @brief Virtual destructor for the PropagationVisitor class.
     * This is provided to ensure proper cleanup when derived classes are destroyed.
     */
    virtual ~PropagationVisitor() {}

    /**
     * @brief Called when the propagation starts.
     * @param context Will contain all elements computed by the algorithm when it starts and contains
     *                information of the first cell.
     */
    virtual void start(const PropagationVisitorContext& context) = 0;

    /**
     * @brief Called when a voxel must be visited.
     * @param context Will contain all elements computed by the algorithm when it starts and contains
     *                information of the current cell.
     */
    virtual void visit(const PropagationVisitorContext& context) = 0;

    /**
     * @brief Called when the propagation is finished.
     * @param context Will contain all elements computed by the algorithm when it starts and contains
     *                information of the first cell.
     */
    virtual void finish(const PropagationVisitorContext& context) = 0;
};


#endif // LVOX3_PROPAGATIONVISITOR_H
