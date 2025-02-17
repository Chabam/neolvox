#include "grid3dpropagationalgorithm.h"

Grid3DPropagationAlgorithm::Grid3DPropagationAlgorithm(const Grid3Df& grid, const VisitorCollection& list,
                                                       const double& radius, const size_t& startCellIndex)
    : m_grid(grid), m_visitors(list)
{
    m_radius = radius;
    m_startCellIndex = startCellIndex;
}

void Grid3DPropagationAlgorithm::startCompute()
{
    startFromCell(m_startCellIndex);
}

void Grid3DPropagationAlgorithm::startFromCell(const size_t& index)
{
    size_t col, lin, level;

    Eigen::Vector3d cellCenter = m_grid.getCellCenterCoordinates(index);
    Eigen::Vector3i gridCoords = m_grid.computeGridColumnLineLevelAtIndex(index);

    PropagationVisitorContext context(index, gridCoords.x(), gridCoords.y(), gridCoords.z(), cellCenter, 0);

    for (int i = 0; i < m_visitors.size(); ++i)
        m_visitors.at(i)->start(context);

    std::stack<StackVar> stack;
    stack.push(StackVar(index, col, lin, level));
    m_visited.insert(index);

    while (!stack.empty())
    {
        StackVar c = stack.top();
        recursiveComputeCell(c.cellIndex, c.col, c.lin, c.level, cellCenter, stack);
        stack.pop();
    }

    for (int i = 0; i < m_visitors.size(); ++i)
        m_visitors.at(i)->finish(context);

    m_visited.clear();
}

void Grid3DPropagationAlgorithm::recursiveComputeCell(const size_t& cellIndex, const size_t& col, const size_t& lin,
                                                      const size_t& level, const Eigen::Vector3d& firstCellCenter,
                                                      std::stack<StackVar>& stack)
{
    const size_t maxCol = m_grid.getDimensionX() - 1;
    const size_t maxLin = m_grid.getDimensionY() - 1;
    const size_t maxLevel = m_grid.getDimensionZ() - 1;

    Eigen::Vector3d newCellCenter;

    newCellCenter = m_grid.getCellCenterCoordinates(m_grid.computeGridIndexForColLinLevel(col, lin, level));

    const double distanceBetweenCells = std::abs((firstCellCenter - newCellCenter).norm());

    if (distanceBetweenCells < m_radius)
    {

        PropagationVisitorContext context(cellIndex, col, lin, level, newCellCenter, distanceBetweenCells);

        for (int i = 0; i < m_visitors.size(); ++i)
            m_visitors.at(i)->visit(context);

        if (col > 0)
        {
            enqueueCell(col - 1, lin, level, stack);
        }

        if (col < maxCol)
        {
            enqueueCell(col + 1, lin, level, stack);
        }

        if (lin > 0)
        {
            enqueueCell(col, lin - 1, level, stack);
        }

        if (lin < maxLin)
        {
            enqueueCell(col, lin + 1, level, stack);
        }

        if (level > 0)
        {
            enqueueCell(col, lin, level - 1, stack);
        }

        if (level < maxLevel)
        {
            enqueueCell(col, lin, level + 1, stack);
        }
    }
}

void Grid3DPropagationAlgorithm::enqueueCell(const size_t& col, const size_t& lin, const size_t& level,
                                             std::stack<StackVar>& stack)
{
    size_t newIndice = m_grid.computeGridIndexForColLinLevel(col, lin, level);
    if (m_visited.insert(newIndice).second == true)
    {
        stack.push(StackVar(newIndice, col, lin, level));
    }
}
