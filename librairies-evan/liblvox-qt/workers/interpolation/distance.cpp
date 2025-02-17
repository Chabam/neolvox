#include "distance.h"
#include "../../traversal/propagation/grid3dpropagationalgorithm.h"
#include "../../traversal/propagation/visitor/distanceinterpolationvisitor.h"

Distance::Distance(const Grid3Df* grid, Grid3Df* resultGrid, double radius, int power, float densityThreshold, lvox::Logger* logger) :
    WorkerBase(logger),
    _grid(grid),
    _resultGrid(resultGrid),
    _radius(radius),
    _power(power),
    _densityThreshold(densityThreshold)
{

}

void Distance::run()
{
    DistanceInterpolationVisitor visitor(*_grid, *_resultGrid, _power, _densityThreshold);

    std::vector<PropagationVisitor*> l;
    l.push_back(&visitor);

    std::stringstream fmt;
    fmt << "Start algo\n";
    fmt << "Radius : " << _radius << "\n" ;


    Grid3DPropagationAlgorithm algo(*_grid, l, _radius);

    const size_t nCells = _grid->cellCount();

    _logger->logInfo(fmt.str());
    for(size_t i=0; i< nCells; ++i) {
        std::stringstream fmt;
        fmt << "Start algo : " << i <<"\n";

        const Grid3Df::GridTypeT density = _grid->getValueAtIndex(i);
        fmt << density << "\n" ;

        //Goes through the grid and interpolates what is considered no data (-1,-2), the rest is an error code and brought back to zero
        if(_grid->isNoData(density))
        {
            fmt << "before algo : " << i <<"\n";
            algo.startFromCell(i);
            fmt << "after algo : " << i <<"\n";
        }
        // _logger->logInfo(fmt.str());
    }
}
