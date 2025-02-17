/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef LVOX3_GRID3DPROPAGATIONALGORITHM_H
#define LVOX3_GRID3DPROPAGATIONALGORITHM_H

#include <Eigen/Geometry>
#include <inttypes.h>
#include <set>
#include <stack>
#include <vector>

#include "visitor/propagationvisitor.h"
#include "visitor/propagationvisitorcontext.h"

#include <stack>

#include "../../datastructures/types.h"

/**
 * @class Grid3DPropagationAlgorithm
 * @brief A class to start from a cell of a 3D grid and propagate in all cells around it with a maximum radius.
 *
 * This class provides a method to start from a cell of a 3D grid and visit all cells around it up to a specified
 * maximum radius. It uses a set of visitor classes to perform specific tasks when a cell is visited.
 */
class Grid3DPropagationAlgorithm
{
public:
    typedef std::vector<PropagationVisitor*> VisitorCollection; ///< Collection of propagation visitors.

    /**
     * @brief Constructor for the Grid3DPropagationAlgorithm class.
     * @param grid The 3D grid to iterate.
     * @param list List of visitors to use when a cell must be visited.
     * @param radius Maximum radius to search cells.
     * @param startCellIndex (optional) Index of the cell to start from if using the method "startCompute".
     */
    Grid3DPropagationAlgorithm(const Grid3Df& grid, const VisitorCollection& list, const double& radius,
                               const size_t& startCellIndex = 0);

    /**
     * @brief Start the propagation algorithm from the cell index specified in the constructor.
     *
     * This method starts the propagation algorithm from the cell index specified in the constructor.
     * It recursively checks if the cell was already visited and, if not, proceeds to visit the neighboring cells
     * as long as they are within the defined radius.
     */
    void startCompute();

    /**
     * @brief Start the propagation algorithm from a specific cell index.
     * @param index The index of the cell to start from.
     *
     * This method starts the propagation algorithm from the cell with the specified index.
     * It recursively checks if the cell was already visited and, if not, proceeds to visit the neighboring cells
     * as long as they are within the defined radius.
     */
    void startFromCell(const size_t& index);

private:
    struct StackVar
    {
        StackVar() : cellIndex(0), col(0), lin(0), level(0) {}
        StackVar(const size_t& cellI, const size_t& c, const size_t& li, const size_t& le)
            : cellIndex(cellI), col(c), lin(li), level(le) {}

        size_t cellIndex;
        size_t col;
        size_t lin;
        size_t level;
    };

    const Grid3Df& m_grid;             ///< Reference to the 3D grid to be traversed.
    const VisitorCollection& m_visitors; ///< Reference to the collection of propagation visitors.
    double m_radius;                   ///< Maximum radius to search cells.
    size_t m_startCellIndex;           ///< Index of the cell to start from.

    std::set<size_t> m_visited;        ///< Set to track already visited cell indices.

    /**
     * @brief Recursively check if the cell was already visited and if not, visit neighboring cells.
     * @param cellIndex Current cell index to check.
     * @param col Current cell's column.
     * @param lin Current cell's line.
     * @param level Current cell's level.
     * @param firstCellCenter Center of the first cell to start the propagation.
     * @param stack Stack used for recursive traversal.
     *
     * This method checks if the cell with the given cellIndex was already visited. If not, it visits the cell
     * and recursively proceeds to visit its neighboring cells if they are within the defined radius.
     */
    void recursiveComputeCell(const size_t& cellIndex, const size_t& col, const size_t& lin, const size_t& level,
                              const Eigen::Vector3d& firstCellCenter, std::stack<StackVar>& stack);

    /**
     * @brief Enqueue the neighboring cell for propagation.
     * @param col Column index of the neighboring cell.
     * @param lin Line index of the neighboring cell.
     * @param level Level index of the neighboring cell.
     * @param stack Stack used for recursive traversal.
     *
     * This method enqueues the neighboring cell with the given column, line, and level indices into the stack
     * for recursive traversal.
     */
    void enqueueCell(const size_t& col, const size_t& lin, const size_t& level, std::stack<StackVar>& stack);
};

#endif // LVOX3_GRID3DPROPAGATIONALGORITHM_H
