/*
 * ct_shot.cpp
 */

#include "ct_itemdrawable/tools/scanner/ct_shot.h"

CT_Shot::CT_Shot(bool valid)
{
    if(valid)
        m_direction = Eigen::Vector3d(1,0,0);
}

CT_Shot::CT_Shot(const Eigen::Vector3d& origin, const Eigen::Vector3d& direction) :
    m_origin(origin),
    m_direction(direction)
{
}

void CT_Shot::setDirection(const Eigen::Vector3d &direction)
{
    m_direction = direction;
}

const Eigen::Vector3d& CT_Shot::direction() const
{
    return m_direction;
}

void CT_Shot::setOrigin(const Eigen::Vector3d &origin)
{
    m_origin = origin;
}

const Eigen::Vector3d& CT_Shot::origin() const
{
    return m_origin;
}
