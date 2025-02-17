#ifndef LVOX3_IMPLSHOOTINGPATTERN_H
#define LVOX3_IMPLSHOOTINGPATTERN_H

#include "ct_itemdrawable/ct_shootingpatternd.h"
#include "liblvox/datastructures/shootingpattern.h"

class LVOX3_ImplShootingPattern : public lvox::ShootingPattern
{
public:
    LVOX3_ImplShootingPattern(CT_ShootingPattern* shootingPattern);

    const Eigen::Vector3d& getCenterCoordinate() const override;
    // lvox::Ray getRayForPoint(const Eigen::Vector3d& point) const override;
    lvox::Ray shotAt(size_t index) const override;
    size_t getNumberOfShots() const override;
    lvox::Ray getRayForPoint(const Eigen::Vector3d& point) const override;


private:
    CT_ShootingPattern* _shootingPattern;
};

#endif // LVOX3_IMPLSHOOTINGPATTERN_H
