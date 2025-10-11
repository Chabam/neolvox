/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_PROPAGATIONVISITOR_H
#define LVOX3_PROPAGATIONVISITOR_H

#include "lvox3_propagationvisitorcontext.h"

/**
 * @brief Inherit from this class to do what you want when a cell
 *        is visited by the woo algorithm implemented in class LVOX3_Grid3DPropagationAlgorithm
 */
class LVOX3_PropagationVisitor {
public:
    virtual ~LVOX3_PropagationVisitor() {}

    /**
     * @brief Called when the propagation start
     * @param context : will contains all elements compute by the algorithm  when it will start. Contains
     *                  information of the first cell
     */
    virtual void start(const LVOX3_PropagationVisitorContext& context) = 0;

    /**
     * @brief Called when a voxel must be visited
     * @param context : will contains all elements compute by the algorithm  when it will start. Contains
     *                  information of the current cell
     */
    virtual void visit(const LVOX3_PropagationVisitorContext& context) = 0;

    /**
     * @brief Called when the propagation was finished
     * @param context : will contains all elements compute by the algorithm  when it will start. Contains
     *                  information of the first cell
     */
    virtual void finish(const LVOX3_PropagationVisitorContext& context) = 0;
};

#endif // LVOX3_PROPAGATIONVISITOR_H
