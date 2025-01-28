/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef CT_SHOT_H
#define CT_SHOT_H

#include "ctlibstructureaddon_global.h"

#include <Eigen/Dense>

/**
 * @brief Represent a shot
 */
struct CTLIBSTRUCTUREADDON_EXPORT CT_Shot {
public:
    /**
     * @brief Construct a default shot
     * @param valid : if true set the direction to (1,0,0) otherwise to (0,0,0)
     */
    CT_Shot(bool valid = true);

    /**
     * @brief Construct a shot with an origin and a direction
     */
    CT_Shot(const Eigen::Vector3d& origin, const Eigen::Vector3d& direction);

    CT_Shot(const CT_Shot& other) = default;

    /**
     * @brief Set the origin of the shot
     */
    void setOrigin(const Eigen::Vector3d& origin);

    /**
     * @brief Returns the origin of the shot
     */
    const Eigen::Vector3d& origin() const;

    /**
     * @brief Set the direction of the shot
     */
    void setDirection(const Eigen::Vector3d& direction);

    /**
     * @brief Returns the direction of the shot
     */
    const Eigen::Vector3d& direction() const;

private:
    Eigen::Vector3d     m_origin;
    Eigen::Vector3d     m_direction;
};

#endif // CT_SHOT_H
