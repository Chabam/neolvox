/**
 * @author Michael Krebs (AMVALOR)
 * @date 25.01.2017
 * @version 1
 */
#ifndef CT_THETAPHISHOOTINGPATTERN_H
#define CT_THETAPHISHOOTINGPATTERN_H

#include "ct_shootingpattern.h"

class CT_Scanner;

/**
 * @brief This class is a simple shooting patterns with a horizontal/vertical field of view and angle resolution
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_ThetaPhiShootingPattern : public CT_ShootingPattern
{
    using SuperClass = CT_ShootingPattern;

public:
    /**
     * @brief Default constructor with parameters.
     */
    CT_ThetaPhiShootingPattern();

    /**
     * @brief Construct it with all information
     * @param origin : origin of all shot
     * @param hFov : horizontal field of view
     * @param vFov : vertical field of view
     * @param hRes : horizontal resolution (angle between two consecutive horizontal rays)
     * @param vRes : vertical resolution (angle between two consecutive vertical rays)
     * @param initTheta : (horizontal) angle between the first ray and the Ox axis
     * @param initPhi : (vertical) angle between the first vertical ray and the Oz axis
     * @param zVector : normal of the scanner
     * @param clockWise : clock wise or not
     * @param radians : type of angle (radians or degrees), degrees by default
     */
    CT_ThetaPhiShootingPattern(const Eigen::Vector3d& origin,
                               double hFov, double vFov,
                               double hRes, double vRes,
                               double initTheta, double initPhi,
                               const Eigen::Vector3d& zVector = Eigen::Vector3d(0,0,1),
                               bool clockWise = true,
                               bool radians = false);

    /**
     * @brief Construct it from existing object (copy it)
     */
    CT_ThetaPhiShootingPattern(const CT_ThetaPhiShootingPattern& other) = default;

    const Eigen::Vector3d& centerCoordinate() const override { return m_origin; }
    size_t numberOfShots() const override;
    CT_Shot shotAt(const size_t& index) override;
    CT_Shot shotAt(const size_t& i, const size_t& j);
    CT_Shot shotForPoint(const CT_Point& pt) override;
    CT_ShootingPattern* clone() const override;

    /**
     * @brief Returns the origin of all shots
     */
    inline const Eigen::Vector3d& origin() const { return m_origin; }

    /**
     * @brief Returns the normal of the scanner
     */
    inline const Eigen::Vector3d& zVector() const { return m_zVector; }

    /**
     * @brief Returns the horizontal field of view
     */
    inline double hFov() const { return m_hFov; }

    /**
     * @brief Returns the vertical field of view
     */
    inline double vFov() const { return m_vFov; }

    /**
     * @brief Returns the horizontal resolution
     */
    inline double hRes() const { return m_hRes; }

    /**
     * @brief Returns the vertical resolution
     */
    inline double vRes() const { return m_vRes; }

    /**
     * @brief Returns the initial theta
     */
    inline double initTheta() const { return m_initTheta; }

    /**
     * @brief Returns the initial phi
     */
    inline double initPhi() const { return m_initPhi; }

    /**
     * @brief Returns the number of horizontal rays
     */
    inline int nHRays() const { return m_nHRays; }

    /**
     * @brief Returns the number of vertical rays
     */
    inline double nVRays() const { return m_nVRays; }

    /**
     * @brief Returns true if the shooting pattern is made in clockwise order or not
     */
    inline bool isClockWise() const { return m_clockWise; }

    /**
     * @brief Set the origin of all shots
     */
    inline void setOrigin( const Eigen::Vector3d& origin ) { m_origin = origin; }

    /**
     * @brief Set the normal of the scanner
     */
    inline void setZVector ( const Eigen::Vector3d& zVector ) { m_zVector = zVector; }

    /**
     * @brief Set the horizontal field of view
     */
    inline void setHFov ( double hFov ) { m_hFov = hFov; updateNumberOfRays(); }

    /**
     * @brief Set the vertical field of view
     */
    inline void setVFov ( double vFov ) { m_vFov = vFov; updateNumberOfRays(); }

    /**
     * @brief Set the horizontal resolution
     */
    inline void setHRes ( double hRes ) { m_hRes = hRes; updateNumberOfRays(); }

    /**
     * @brief Set the vertical resolution
     */
    inline void setVRes ( double vRes ) { m_vRes = vRes; updateNumberOfRays(); }

    /**
     * @brief Set the the initial theta
     */
    inline void setInitTheta ( double initTheta ) { m_initTheta = initTheta; }

    /**
     * @brief Set the the initial phi
     */
    inline void setInitPhi ( double initPhi ) { m_initPhi = initPhi; }

    /**
     * @brief Set true if the shooting pattern is made in clockwise order or not
     */
    inline void setClockWise ( bool clockWise ) { m_clockWise = clockWise; }

private:
    void updateNumberOfRays();
    void resetCache();

    Eigen::Vector3d m_origin;		/*!< origin of the shots */
    Eigen::Vector3d	m_zVector;		/*!< normal of the scanner */
    double          m_hFov;			/*!< horizontal field of view */
    double          m_vFov;			/*!< vertical field of view */
    double          m_hRes;			/*!< horizontal angle resolution of the shooting pattern */
    double          m_vRes;			/*!< vertical angle resolution of the shooting pattern */
    double          m_initTheta;    /*!< initial horizontal angle of the measurement in the world coordinate system */
    double          m_initPhi;		/*!< initial vertical angle of the measurement in the world coordinate system */
    int             m_nHRays;		/*!< number of ray on a entire horizontal move of the shooting pattern */
    int             m_nVRays;		/*!< number of ray on a entire horizontal move of the shooting pattern */
    bool            m_clockWise;    /*!< Whether the shooting pattern has been done in clockwise or not */

    /* cache */
    size_t m_cacheI;
    size_t m_cacheJ;
    double m_cacheSinTheta;
    double m_cacheCosTheta;
    double m_cacheSinPhi;
    double m_cacheCosPhi;
};

#endif // CT_THETAPHISHOOTINGPATTERN_H
