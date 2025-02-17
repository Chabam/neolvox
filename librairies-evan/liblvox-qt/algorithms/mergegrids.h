#ifndef MERGEGRIDS_H
#define MERGEGRIDS_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../datastructures/types.h"

#include "../reducers/base.h"
#include "../factory/factory.hpp"

#define LVOX_REDUCER_RDI "MaxRDI"
#define LVOX_REDUCER_HITS "MaxNi"
#define LVOX_REDUCER_TRUST "MaxTrust"
#define LVOX_REDUCER_TRUST_RATIO "MaxTrustRatio"
#define LVOX_REDUCER_SUM_RATIO "SumRatio"
#define LVOX_REDUCER_WEIGHTED_RDI "WeightedRDI"
#define LVOX_REDUCER_WEIGHTED_RDI_ALT "WeightedRDIAlternative"

namespace lvox
{
/**
 * @class MergeGrids
 * @brief A class to merge grids using different methods and voxel reducers.
 *
 * This class provides functionality to merge grids using various methods and voxel reducers.
 * It allows users to select different merging methods and apply voxel reducers to customize
 * the merging process.
 */
class __declspec(dllexport) MergeGrids
{
public:
    /**
     * @enum Method
     * @brief Enumerates different merging methods.
     *
     * This enumeration defines various methods for merging grids, each method
     * represents a different technique to merge grids based on specific criteria.
     */
    enum class Method
    {
        RDI,                        ///< RDI merging method.
        TRUST,                      ///< TRUST merging method.
        HITS,                       ///< HITS merging method.
        NI,
        TRUST_RATIO,                ///< TRUST_RATIO merging method.
        SUM_RATIO,                  ///< SUM_RATIO merging method.
        WEIGHTED_RDI,               ///< WEIGHTED_RDI merging method.
        WEIGHTED_RDI_ALT            ///< Alternative WEIGHTED_RDI merging method.
    };

    /**
     * @struct Settings
     * @brief Represents the settings for grid merging.
     *
     * This structure holds various settings required for grid merging.
     */
    struct Settings
    {
        Method Method;              ///< The merging method to be used.
    };

    /**
     * @struct VoxelReducerDefinition
     * @brief Represents the definition of a voxel reducer.
     *
     * This structure holds the label, name, and description of a voxel reducer.
     */
    struct VoxelReducerDefinition
    {
        VoxelReducerDefinition() = default;
        VoxelReducerDefinition(std::string l, std::string n, std::  string d) : label(l), name(n), desc(d) {};
        std::string label;          ///< The label of the voxel reducer.
        std::string name;           ///< The name of the voxel reducer.
        std::string desc;           ///< The description of the voxel reducer.
    };

private:
    Factory<ReducerBase, MergeGrids::Method> _f;        ///< Factory for voxel reducers.
    static const std::vector<VoxelReducerDefinition> _voxelReducerDefinitions; ///< Vector of voxel reducer definitions.
    Grid3DSets& _gridSets;          ///< Reference to the Grid3DSets containing grids to merge.
    Grid3DSet& _mergeSet;           ///< Reference to the GridSet in which the merge will happen
    Settings& _settings;            ///< Reference to the settings for grid merging.

public:

    /**
     * @brief Constructor to create a MergeGrids object for merging grids.
     * @param gridsToMerge The Grid3DSets containing grids to merge.
     * @param mergedSet The Grid3DSet for the merged result.
     * @param settings The settings for grid merging.
     */
    MergeGrids(Grid3DSets& gridsToMerge, Grid3DSet& mergedSet, Settings& settings);

    /**
     * @brief Perform grid merging based on the specified settings and method.
     */
    void compute();

    /**
     * @brief Apply voxel reducer to merge grids with a progress monitor.
     * @param merged The Grid3DSet for the merged result.
     * @param gs Vector of Grid3DSet containing grids to merge.
     * @param reducer The voxel reducer to be applied for merging.
     * @param monitor The ProgressMonitor to track the merging progress.
     */
    void apply(Grid3DSet& merged, std::vector<Grid3DSet>& gs, ReducerBase* reducer);

    /**
     * @brief Get the list of available voxel reducers.
     * @return Vector of VoxelReducerDefinition containing the voxel reducer details.
     */
    static inline const std::vector<MergeGrids::VoxelReducerDefinition> getReducerList()
    {
        return std::vector<MergeGrids::VoxelReducerDefinition>
        {
                MergeGrids::VoxelReducerDefinition{LVOX_REDUCER_RDI, "Max density", "max(rdi)"},
                MergeGrids::VoxelReducerDefinition{LVOX_REDUCER_TRUST, "Max trust", "max(nt - nb)"},
                MergeGrids::VoxelReducerDefinition{LVOX_REDUCER_TRUST_RATIO, "Max trust ratio", "max((nt - nb)/nt)"},
                MergeGrids::VoxelReducerDefinition{LVOX_REDUCER_HITS, "Max hits", "max(ni)"},
                MergeGrids::VoxelReducerDefinition{LVOX_REDUCER_SUM_RATIO, "Sum ratio", "sum(ni)/sum(nt - nb)"},
                MergeGrids::VoxelReducerDefinition{LVOX_REDUCER_WEIGHTED_RDI, "Weighted rdi", "sum((nt-nb)*rdi)/sum(nt-nb)"},
                MergeGrids::VoxelReducerDefinition{LVOX_REDUCER_WEIGHTED_RDI_ALT, "Weighted rdi alternative",
                "sum((nt-nb)/nt*rdi)/sum((nt-nb)/nt)"}
        };
    }
    static const std::vector<MergeGrids::VoxelReducerDefinition> VOXEL_REDUCER_DEFINITION;


    /**
     * @brief Create a unique pointer to a voxel reducer based on the specified label.
     * @param label The label of the voxel reducer to be created.
     * @return Unique pointer to the created voxel reducer.
     */
    std::unique_ptr<ReducerBase> makeReducer()
    {
        return std::unique_ptr<ReducerBase>(_f.create(_settings.Method));
    }

    static bool doReducerExists(std::string& str)
    {
        if(str == LVOX_REDUCER_RDI) return true;
        else if(str == LVOX_REDUCER_HITS) return true;
        else if(str == LVOX_REDUCER_TRUST) return true;
        else if(str == LVOX_REDUCER_TRUST_RATIO) return true;
        else if(str == LVOX_REDUCER_SUM_RATIO) return true;
        else if(str == LVOX_REDUCER_WEIGHTED_RDI) return true;
        else if(str == LVOX_REDUCER_WEIGHTED_RDI_ALT) return true;

        return false;
    }

    static inline Method getMethodFromString(std::string& label)
    {
        if(label == LVOX_REDUCER_RDI) return Method::RDI;
        else if(label == LVOX_REDUCER_HITS) return Method::HITS;
        else if(label == LVOX_REDUCER_TRUST) return Method::TRUST;
        else if(label == LVOX_REDUCER_TRUST_RATIO) return Method::TRUST_RATIO;
        else if(label == LVOX_REDUCER_SUM_RATIO) return Method::SUM_RATIO;
        else if(label == LVOX_REDUCER_WEIGHTED_RDI) return Method::WEIGHTED_RDI;
        return Method::WEIGHTED_RDI_ALT;

    }
};
}

#endif // MERGEGRIDS_H
