/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_GRID3DPROPAGATIONALGORITHM_H
#define LVOX3_GRID3DPROPAGATIONALGORITHM_H

#include "tools/3dgrid/lvox_grid3d.h"

#include "tools/lvox3_gridtools.h"

#include "visitor/lvox3_propagationvisitorcontext.h"

class LVOX3_PropagationVisitor;

/**
 * @brief Use this class to start from a cell of a 3D grid and propagate in all cells
 *        around it with a maximum radius (distance from center of cells)
 */
class LVOX3_Grid3DPropagationAlgorithm
{
public:
    typedef QVector<LVOX3_PropagationVisitor*> VisitorCollection;

    /**
     * @brief Construct a propagator
     * @param grid : grid to iterate
     * @param list : list of visitors to use when a cell must be visited
     * @param radius : maximum radius to search cells
     * @param startCellIndex : (optionnal) index of the cell to start, if you want to use the method "startCompute"
     */
    LVOX3_Grid3DPropagationAlgorithm(const CT_AbstractGrid3D* grid,
                                     const VisitorCollection& list,
                                     const double& radius,
                                     const Index& startCellIndex = 0);
    ~LVOX3_Grid3DPropagationAlgorithm();

    /**
     * @brief Use this method if you have already set the index of the cell in constructor
     */
    void startCompute();

    /**
     * @brief Use this method to start from a cell defined here
     */
    void startFromCell(const Index &index);

private:
    struct StackVar {
        StackVar() : cellIndex(0),
            col(0),
            lin(0),
            level(0){}
        StackVar(const Index& cellI,
                 const Index& c,
                 const Index& li,
                 const Index& le) : cellIndex(cellI),
                                     col(c),
                                     lin(li),
                                     level(le) {}

        Index cellIndex;
        Index col;
        Index lin;
        Index level;
    };

    LVOX3_GridTools*            m_gridTools;
    CT_AbstractGrid3D*       m_grid;
    const VisitorCollection&    m_visitors;
    double                      m_radius;
    Index                      m_startCellIndex;

    QSet<Index>                m_visited;

    /**
     * @brief Recursively check if the cell was already visited and if not go to next cells if it was not
     *        at a distance longer then the defined radius
     * @param cellIndex : current cell index to test
     * @param col : current cell it's column
     * @param lin : current cell it's line
     * @param level : current cell it's level
     * @param firstCellCenter : first cell it's center
     */
    void recursiveComputeCell(const Index& cellIndex,
                              const Index& col,
                              const Index& lin,
                              const Index& level,
                              const Eigen::Vector3d& firstCellCenter,
                              QStack<StackVar> &stack);
private:
    void enqueueCell(const Index& col,
            		 const Index& lin,
					 const Index& level,
					 QStack<StackVar> &stack);
};

#endif // LVOX3_GRID3DPROPAGATIONALGORITHM_H
