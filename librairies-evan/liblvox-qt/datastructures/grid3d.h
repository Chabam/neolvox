#ifndef GRID3D_H
#define GRID3D_H

#include <Eigen/Geometry>
namespace lvox
{
/**
 * @brief The Grid3d class every implementation for other languages should implement this class
 */
template <typename DataT> class Grid3d
{
public:
    using GridTypeT = DataT;

    /** Getters */
    virtual DataT getNAValue() const = 0;
    virtual float getLambda1() const = 0; ///<
    virtual size_t cellCount() const = 0; ///< Nombre de cellules
    virtual DataT getValueAtIndex(size_t index) const = 0;
    virtual DataT getValueAtCoordinates(int x, int y, int z) const = 0;
    inline DataT getValueAtCoordinates(Eigen::Vector3d coords)
    {
        return getValueAtCoordinates(coords.x(), coords.y(), coords.z());
    }
    virtual Eigen::Vector3d getCellCenterCoordinates(size_t index) const = 0;

    virtual double getResolution() const = 0;

    virtual size_t getDimensionX() const = 0;
    virtual size_t getDimensionY() const = 0;
    virtual size_t getDimensionZ() const = 0;

    virtual double getMinX() const = 0;
    virtual double getMinY() const = 0;
    virtual double getMinZ() const = 0;

    virtual double getMaxX() const = 0;
    virtual double getMaxY() const = 0;
    virtual double getMaxZ() const = 0;

    ///< The pair should be in the <bottomLeft, topRight> order. Only Axis Aligned Bounding boxes are supported
    virtual std::pair<Eigen::Vector3d,Eigen::Vector3d> getBoundingBox() const = 0;


    virtual float getSommationDeltaAtIndex(size_t index) = 0;
    virtual float getSommationFreeAtIndex(size_t index) = 0;
    virtual float getSommationEffectiveDeltaAtIndex(size_t index) = 0;
    virtual float getSommationEffectiveHitsAtIndex(size_t index) = 0;
    virtual float getSommationEffectiveFreeAtIndex(size_t index) = 0;
    virtual float getSommationDeltaSquareAtIndex(size_t index) = 0;

    /** Action */
    virtual void resizeGrid(Eigen::Vector3d& min, Eigen::Vector3d& max, double resolution) = 0;
    virtual void addValueAtIndex(size_t index, DataT value) = 0;
    virtual void addValueAtCoordinates(double x, double y, double z, DataT value) = 0;
    virtual void addSommationDelta(size_t index, double delta) = 0;
    virtual void addSommationFree(size_t index, double free) = 0;
    virtual void addSommationEffectiveDelta(size_t index, double effectiveDelta) = 0;
    virtual void addSommationEffectiveHits(size_t index, double effectiveHits) = 0;
    virtual void addSommationEffectiveFree(size_t index, double effectiveFree) = 0;
    virtual void addSommationDeltaSquare(size_t index, double deltaSquare) = 0;

    /** Setters */
    virtual void setLambda1(double lambda1) = 0;
    virtual void setValueAtIndex(size_t index, DataT value) = 0;
    virtual void setValueAtCoordinates(double x, double y, double z, DataT value) = 0;
    inline void setValueAtCoordinates(Eigen::Vector3d coords, DataT value)
    {
        return setValueAtCoordinates(coords.x(), coords.y(), coords.z(), value);
    }


    inline size_t computeGridIndexForColLinLevel(const size_t col, const size_t lin, const size_t level) const
    {
        return level * getDimensionX() * getDimensionY() + lin * getDimensionX() + col;
    }

    inline void doSommation(size_t index, float delta, float free)
    {
        float attenuationCoefficient = getLambda1();
        int sommaI = 0;
        if (free < delta)
            sommaI = 1;
        addSommationDelta(index, delta);
        addSommationFree(index, free);
        addSommationEffectiveDelta(index, -(log(1 - delta * attenuationCoefficient) / attenuationCoefficient));
        addSommationEffectiveHits(index, -(sommaI * log(1 - free * attenuationCoefficient) / attenuationCoefficient));
        addSommationEffectiveFree(index, -(log(1 - free * attenuationCoefficient) / attenuationCoefficient));
        addSommationDeltaSquare(index, pow(-(log(1 - delta * attenuationCoefficient) / attenuationCoefficient), 2));
    }

    inline void computeCellBottomLeftTopRightCornerAtColLinLevel(int column, int line, int level,
                                                                 Eigen::Vector3d& outBottomLeftCorner,
                                                                 Eigen::Vector3d& outTopRightCorner) const
    {
        outBottomLeftCorner.x() = getMinX() + column * getResolution();
        outBottomLeftCorner.y() = getMinY() + line * getResolution();
        outBottomLeftCorner.z() = getMinZ() + level * getResolution();
        outTopRightCorner.x() = outBottomLeftCorner.x() + getResolution();
        outTopRightCorner.y() = outBottomLeftCorner.y() + getResolution();
        outTopRightCorner.z() = outBottomLeftCorner.z() + getResolution();
    }

    inline void computeGridIndexForPoint(const Eigen::Vector3d& point,
                                                    size_t& col,
                                                    size_t& lin,
                                                    size_t& level,
                                                    size_t& indice) const
    {
        col = std::floor((point.x() - getMinX()) / getResolution());
        lin = std::floor((point.y() - getMinY()) / getResolution());
        level = std::floor((point.z() - getMinZ()) / getResolution());
        indice = level * getDimensionX() * getDimensionY() + lin * getDimensionX() + col;
    }

    inline Eigen::Vector3i computeGridColumnLineLevelAtIndex(size_t index) const
    {
        int line = 0;
        int level = 0;
        int column = 0;

        int dim2 = getDimensionX() * getDimensionY();

        if (index >= dim2)
            level = index / dim2;

        if (index >= getDimensionX())
            line = (index - (level * dim2)) / getDimensionX();

        column = index - (level * dim2) - (line * getDimensionX());

        return Eigen::Vector3i{column, line, level};
    }

    /**
     * @brief Error or warning code that must be used in grid
     *
     * Technically, that's legacy code and could be removed without altering current behavior.
     * Kept as it may find a use in the future.
     */
    enum ErrorOrWarningCode
    {
        B_Minus_C_Inferior_Threshold = -1, // warning
        B_Equals_C = -2,                   // error
        B_Inferior_C = -3,                 // error
        A_Superior_B_Minus_C = -4,         // error
        A_Equals_B_Minus_C = -5,           // warning
        IDR_Surestimated = -6,             // warning
        MNT = -7,                          // filter
        Sky = -8,                          // filter
        Max_Error_Code = -9,               // don't use it ! it is just to set NA() in grid

        // Same as previous but with Nb, Nt, Ni
        Nt_Equals_Nb = B_Equals_C,
        Nt_Inferior_Nb = B_Inferior_C,
        Nt_Minus_Nb_Inferior_Threshold = B_Minus_C_Inferior_Threshold,
        Ni_Superior_Nt_Minus_Nb = A_Superior_B_Minus_C,
        Ni_Equals_Nt_Minus_Nb = A_Equals_B_Minus_C,

        // Same as previous but more explicit
        All_theoretical_rays_have_already_been_intercepted = B_Equals_C,
        A_voxel_has_more_theoretical_rays_than_returned = B_Inferior_C,
        Too_little_information_for_IDR_calculation = B_Minus_C_Inferior_Threshold,
        A_voxel_has_returned_more_rays_than_incoming_rays = A_Superior_B_Minus_C,
        All_incoming_rays_were_returned = A_Equals_B_Minus_C,
        Probable_probability_of_number_of_returned_rays = IDR_Surestimated,

        // other
        Zero_Division = B_Equals_C
    };

    /**
     * @brief Call it to know if the value is filtered or not
     */
    static bool isFiltered(const DataT& value)
    {
        return static_cast<ErrorOrWarningCode>(value) <= ErrorOrWarningCode::MNT;
    }

    /**
     * @brief Call it to know if the value is a warning or not
     */
    static bool isAWarning(const DataT& value)
    {
        return (static_cast<ErrorOrWarningCode>(value) == ErrorOrWarningCode::Nt_Minus_Nb_Inferior_Threshold) ||
               (static_cast<ErrorOrWarningCode>(value) == ErrorOrWarningCode::Nt_Equals_Nb) ||
               (static_cast<ErrorOrWarningCode>(value) == ErrorOrWarningCode::IDR_Surestimated);
    }

    /**
     * @brief Call it to know if the value is nodata or not
     */
    template <typename T> static bool isNoData(const T& value)
    {
        return static_cast<ErrorOrWarningCode>(value) == ErrorOrWarningCode::Nt_Minus_Nb_Inferior_Threshold ||
               static_cast<ErrorOrWarningCode>(value) == ErrorOrWarningCode::Nt_Equals_Nb;
    }
};
} // namespace lvox

#endif // GRID3D_H
