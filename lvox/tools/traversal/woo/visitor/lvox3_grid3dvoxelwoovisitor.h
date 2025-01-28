/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_GRID3DVOXELWOOVISITOR_H
#define LVOX3_GRID3DVOXELWOOVISITOR_H

#include "lvox3_grid3dvoxelwoovisitorcontext.h"

/**
 * @brief Inherit from this class to do what you want when a cell
 *        is visited by the woo algorithm implemented in class LVOX3_Grid3DWooTraversalAlgorithm
 */
class LVOX3_Grid3DVoxelWooVisitor
{
public:
    virtual ~LVOX3_Grid3DVoxelWooVisitor() {}

    /**
     * @brief Called when a voxel must be visited
     */
    virtual void visit(const LVOX3_Grid3DVoxelWooVisitorContext& context) = 0;
};

#endif // LVOX3_GRID3DVOXELWOOVISITOR_H
