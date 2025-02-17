#ifndef REDUCERSBASE_H
#define REDUCERSBASE_H

#pragma once

#include <string>

#include "../datastructures/types.h"

/**
 * @struct VoxelReducerOptions
 * @brief Represents options for voxel reducers used in grid reduction.
 *
 * This structure represents options for voxel reducers used in grid reduction.
 * It allows customization of reduction behavior by providing various options such as the reducer label,
 * ignoring voxels with zero density, setting an effective rays threshold, and specifying whether the
 * threshold is relative.
 */
struct VoxelReducerOptions
{
    /**
     * @brief Default constructor for VoxelReducerOptions.
     */
    VoxelReducerOptions() : reducerLabel(), ignoreVoxelZeroDensity(true), effectiveRaysThreshold(5), isRelativeThreshold(false)
    {
    }

    std::string reducerLabel;           ///< The label of the voxel reducer.
    bool ignoreVoxelZeroDensity;        ///< Flag to ignore voxels with zero density during reduction.
    int effectiveRaysThreshold;         ///< The threshold for effective rays used in reduction.
    bool isRelativeThreshold;           ///< Flag indicating whether the threshold is relative.
};

/**
 * @struct VoxelData
 * @brief Represents data associated with a voxel in the grid.
 *
 * This structure represents data associated with a voxel in the grid.
 * It contains information such as nt (number of theoretical rays),
 * nb (number of background rays), ni (number of intercepted rays), and rd (reduction value).
 * The structure also provides methods to load and commit voxel data to a Grid3DSet.
 */
struct VoxelData
{
    int nt;     ///< Number of theoretical rays.
    int nb;     ///< Number of background rays.
    int ni;     ///< Number of intercepted rays.
    float rd;   ///< Reduction value.

    /**
     * @brief Load voxel data from a Grid3DSet at a specified index.
     * @param g The Grid3DSet from which to load the voxel data.
     * @param idx The index of the voxel data to load.
     */
    void load(Grid3DSet& g, size_t idx);

    /**
     * @brief Commit voxel data to a Grid3DSet at a specified index.
     * @param g The Grid3DSet to which the voxel data will be committed.
     * @param idx The index of the voxel data to commit.
     */
    void commit(Grid3DSet& g, size_t idx);
};

/**
 * @struct ReducerBase
 * @brief Represents the base class for voxel reducers used in grid reduction.
 *
 * This structure represents the base class for voxel reducers used in grid reduction.
 * It provides methods for initialization, joining voxel data, and accessing the voxel data.
 * The ReducerBase contains a VoxelData object to store the reduction result and a VoxelReducerOptions
 * object to hold the options for the reducer.
 */
struct ReducerBase
{
    /**
     * @brief Default constructor for ReducerBase.
     */
    ReducerBase() = default;

    /**
     * @brief Constructor to create a ReducerBase object with options.
     * @param opts The VoxelReducerOptions to be associated with the reducer.
     */
    ReducerBase(VoxelReducerOptions& opts);

    /**
     * @brief Initialize the voxel data with the provided VoxelData object.
     * @param data The VoxelData object to initialize the voxel data.
     */
    virtual void init(VoxelData& data);

    /**
     * @brief Join the current voxel data with another VoxelData object.
     * @param rhs The VoxelData object to join with the current voxel data.
     */
    virtual void join(const VoxelData& rhs) = 0;

    /**
     * @brief Get a reference to the voxel data stored in the reducer.
     * @return A reference to the VoxelData object.
     */
    virtual VoxelData& value();

    VoxelData _data;                    ///< Voxel data for the reducer.
    VoxelReducerOptions _opts;          ///< Options for the voxel reducer.
};

#endif
