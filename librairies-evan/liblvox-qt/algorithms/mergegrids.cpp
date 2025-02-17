#include "mergegrids.h"
#include "../reducers/maxni.h"
#include "../reducers/maxrdi.h"
#include "../reducers/maxtrust.h"
#include "../reducers/maxtrustratio.h"
#include "../reducers/sumratio.h"
#include "../reducers/weightedrdi.h"

namespace lvox {
MergeGrids::MergeGrids(Grid3DSets& gridsToMerge, Grid3DSet& mergedSet, Settings& settings)
    : _gridSets(gridsToMerge),
      _mergeSet(mergedSet),
      _settings(settings)
{
    _f.registerType<MaxRDI>(MergeGrids::Method::RDI);
    _f.registerType<MaxTrust>(MergeGrids::Method::TRUST);
    _f.registerType<MaxTrustRatio>(MergeGrids::Method::TRUST_RATIO);
    _f.registerType<MaxNi>(MergeGrids::Method::HITS);
    _f.registerType<SumRatio>(MergeGrids::Method::SUM_RATIO);
    _f.registerType<WeightedRDI>(MergeGrids::Method::WEIGHTED_RDI);
    _f.registerType<WeightedRDIAlt>(MergeGrids::Method::WEIGHTED_RDI_ALT);
}

void MergeGrids::compute()
{
    std::unique_ptr<ReducerBase> reducer = makeReducer();
    apply(_mergeSet, _gridSets, reducer.get());
}

void MergeGrids::apply(Grid3DSet& merged, std::vector<Grid3DSet>& gs, ReducerBase* reducer)
{
    if (gs.empty())
        return;

    Grid3DSet& a = gs.back();
    size_t n = a.Density->cellCount();
    VoxelData item, rhs;
    for (size_t idx = 0; idx < n; idx++)
    {
        // default values
        item.load(a, idx);
        reducer->init(item);
        // Loops through all the grids for each voxel
        for (int gi = 1; gi < gs.size(); gi++)
        {
            Grid3DSet& set = gs.at(gi);
            rhs.load(set, idx);
            reducer->join(rhs);
        }
        // Assigns the value of the voxel for the merged density grid
        reducer->value().commit(merged, idx);
    }
}
}
