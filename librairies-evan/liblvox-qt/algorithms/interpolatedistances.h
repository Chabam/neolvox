#ifndef INTERPOLATEDISTANCES_H
#define INTERPOLATEDISTANCES_H

#include "../tools/logger.h"
#include "../datastructures/types.h"

/**
 * @class InterpolateDistances
 * @brief A class to perform distance interpolation based on specified settings and method.
 *
 * This class provides functionality to perform distance interpolation on interpolation grids,
 * utilizing different interpolation methods and settings to achieve accurate interpolation results.
 */
class InterpolateDistances
{
public:
    /**
     * @enum Method
     * @brief Enumerates different interpolation methods.
     *
     * This enumeration defines various methods for distance interpolation, each method
     * represents a different technique to interpolate distances in the grids.
     */
    enum class Method
    {
        Distance = 0,           ///< Distance interpolation method.
        Trust,                  ///< Trust interpolation method.
        ZAverage,               ///< Z Average interpolation method.
    };

    /**
     * @struct Settings
     * @brief Represents the settings for distance interpolation.
     *
     * This structure holds various settings required for performing distance interpolation.
     */
    struct Settings
    {
        double InterpolateRadius;               ///< The radius for distance interpolation.

        /**
         * Distance interpolation settings
         */
        double InterpolateDensityThreshold;     ///< The density threshold for distance interpolation.
        int InterpolatePower;                   ///< The power value for distance interpolation.

        /**
         * Trust interpolation settings
         */
        bool EnableInterpolateTrust;            ///< Flag to enable trust interpolation.
        int TrustLowThreshold;                  ///< The low threshold value for trust interpolation.
        int TrustHighThreshold;                 ///< The high threshold value for trust interpolation.

        /**
         * Z Average interpolation settings
         */
        int EffectiveRayThresh;                 ///< The threshold for effective ray calculation in Z Average interpolation.
        int NumZCalculatedOn;                   ///< The number of Z values to be calculated on in Z Average interpolation.
    };

    using Input = InterpolationGrids;

    /**
     * @brief Constructor to create an InterpolateDistances object.
     * @param sets The InterpolationGridsSet containing the interpolation grids.
     * @param method The interpolation method to be used.
     * @param settings The settings for distance interpolation.
     */
    InterpolateDistances(std::vector<Input>& input, Method method, Settings settings, lvox::Logger* logger);

    /**
     * @brief Perform distance interpolation based on the specified settings and method.
     */
    void compute();

private:
    std::vector<Input>& _sets;       ///< Reference to the InterpolationGridsSet containing the interpolation grids.
    Method _method;                     ///< The interpolation method to be used.
    Settings _settings;                 ///< The settings for distance interpolation.
    lvox::Logger* _logger;
};

#endif
