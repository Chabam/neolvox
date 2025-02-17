#include "computelvoxgrids.h"
#include "../workers/grids/before.h"
#include "../workers/grids/hits.h"
#include "../workers/grids/theoretical.h"
#include <string>

lvox::ComputeLvoxGrids::ComputeLvoxGrids(std::vector<ItemPreparator>& computeItems, Settings& settings, lvox::Logger* log)
    : _settings(settings), _computeItems(computeItems), _logger(log)
{
    Eigen::AlignedBox3d sceneBoundingBox;
    Eigen::AlignedBox3d scannerBoundingBox;

    for (auto& e : computeItems)
    {
        lvox::PointCloudScene* scene = e.PointCloudScene;
        // lvox::Scanner* scanner = e.Scanner;
        lvox::ShootingPattern* pattern = e.ShootingPattern;

        const Eigen::Vector3d& scanPosition = pattern->getCenterCoordinate();
        scannerBoundingBox.extend(scanPosition);
        //_logger->logInfo(std::to_string(scanPosition.x()) + " " + std::to_string(scanPosition.y()) + " " + std::to_string(scanPosition.z()));

        Eigen::Vector3d sceneBBOXMin, sceneBBOXMax;
        scene->boundingBox(sceneBBOXMin, sceneBBOXMax);
        sceneBoundingBox.extend(sceneBBOXMin);
        sceneBoundingBox.extend(sceneBBOXMax);
    }


    switch (_settings.GridMode)
    {
    case GridMode::BoundingBoxOfTheScene:
        _voxelBox.extend(sceneBoundingBox);
        break;
    case GridMode::RelativeToCoordinates:
        _voxelBox.extend(_settings.Position);
        _voxelBox.extend(sceneBoundingBox.max());
        break;
    case GridMode::FromOtherGrid:
    case GridMode::RelativeToCoordinatesAndCustomDimensions:
        _voxelBox.extend(_settings.Position);
        _voxelBox.extend(_settings.Position + _settings.Dimension);
        _voxelBox.clamp(sceneBoundingBox);
        break;
    case GridMode::CenteredOnCoordinatesAndCustomDimensions:
        _voxelBox.extend(_settings.Position - _settings.Dimension);
        _voxelBox.extend(_settings.Position + _settings.Dimension);
        break;
    case GridMode::FromGridFileParameters:
        return;
        break;
    }

    // extends the bounding box to include the scanner
    if (!_voxelBox.contains(scannerBoundingBox))
    {
        _logger->logInfo("Grid dimensions has been enlarged to include scanner and/or their range");
        if (_settings.GridMode == GridMode::CenteredOnCoordinatesAndCustomDimensions)
        {
            Eigen::Vector3d v1s = scannerBoundingBox.min().array();
            Eigen::Vector3d v2s = scannerBoundingBox.max().array();
            Eigen::Vector3d v1 = _voxelBox.min().array();
            Eigen::Vector3d v2 = _voxelBox.max().array();
            while (v1(0) > v1s(0))
            {
                v1(0) -= _settings.GridResolution;
            }
            while (v1(1) > v1s(1))
            {
                v1(1) -= _settings.GridResolution;
            }
            while (v1(2) > v1s(2))
            {
                v1(2) -= _settings.GridResolution;
            }
            while (v2(0) < v2s(0))
            {
                v2(0) += _settings.GridResolution;
            }
            while (v2(1) < v2s(1))
            {
                v2(1) += _settings.GridResolution;
            }
            while (v2(2) < v2s(2))
            {
                v2(2) += _settings.GridResolution;
            }
            _voxelBox.extend(v1);
            _voxelBox.extend(v2);
        }
        else
        {
            _voxelBox.extend(scannerBoundingBox);
        }
    }

    std::stringstream fmt;
    fmt << _voxelBox.min()(0) << " " << _voxelBox.min()(1) << " " <<_voxelBox.min()(2) << " "
        << _voxelBox.max()(0) << " " << _voxelBox.max()(1) << " " <<_voxelBox.max()(2);
    if(_logger != nullptr)
        _logger->logInfo(fmt.str());
}

bool lvox::ComputeLvoxGrids::getInitBoundingBox(Eigen::Vector3d& minBox, Eigen::Vector3d& maxBox) const
{
    minBox = _voxelBox.min();
    maxBox = _voxelBox.max();
    return true;
}

void lvox::ComputeLvoxGrids::compute(std::vector<ComputeItem>& items)
{
    if(_logger != nullptr) _logger->logDebug("Started Computing grids");
    //WorkerManager workerManager;

    for (auto& computeItem : items)
    {
        if(_settings.MustComputeDistances)
        {
            double epa = computeElementProjectedArea();
            computeItem.BeforeGrid->setLambda1(epa);
            computeItem.TheoreticalGrid->setLambda1(epa);
            computeItem.HitsGrid->setLambda1(epa);
        }
        Hits wh(computeItem.ShootingPattern, computeItem.PointCloudScene, computeItem.HitsGrid,
                _settings.MustComputeDistances, _logger);
        Theoretical wt(computeItem.ShootingPattern, computeItem.TheoreticalGrid, _settings.MustComputeDistances, _logger);
        Before wb(computeItem.ShootingPattern, computeItem.PointCloudScene, computeItem.BeforeGrid, true, _logger);

        if(_logger != nullptr) _logger->logInfo("Start worker hits");
        wh.run();
        if(_logger != nullptr) _logger->logInfo("Start worker theo");
        wt.run();
        if(_logger != nullptr) _logger->logInfo("Start worker before");
        wb.run();
    }
}

double lvox::ComputeLvoxGrids::computeElementProjectedArea()
{
    switch (_settings.VegetationType)
    {
    case NeedleFromDimension:
        return (2 * M_PI * (_settings.VegetationSettings.Length/100) * (_settings.VegetationSettings.Diameter/100)/2) / 4;//= (2pi *  lenght * radius) / 4
    case BroadLeafFromDimension:
        return (2 * M_PI * pow((_settings.VegetationSettings.Extension1/100),2)) / 4;//= (2pi * radius^2) / 4
    case NeedleFromFlatArea:
        return (M_PI *(_settings.VegetationSettings.FlatProjectedArea / 10000)) / 4;//= (pi*flatarea) / 4
    case BroadLeafFromFlatArea:
        return (2 *(_settings.VegetationSettings.FlatProjectedArea/10000)) / 4;//= (2halfarea) / 4
    }
}
