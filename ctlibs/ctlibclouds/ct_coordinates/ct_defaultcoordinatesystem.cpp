#include "ct_defaultcoordinatesystem.h"

#include "ct_coordinates/tools/ct_coordinatesystemmanager.h"
#include "ct_global/ct_cloudscontext.h"

#include <limits>

CT_DefaultCoordinateSystem::CT_DefaultCoordinateSystem()
{
    m_xOffset = 0;
    m_yOffset = 0;
    m_zOffset = 0;
}

CT_DefaultCoordinateSystem::CT_DefaultCoordinateSystem(const Eigen::Vector3d &v)
{
    m_xOffset = v(0);
    m_yOffset = v(1);
    m_zOffset = v(2);
}

CT_DefaultCoordinateSystem::CT_DefaultCoordinateSystem(CT_AbstractCoordinateSystem::realEx x,
                                                       CT_AbstractCoordinateSystem::realEx y,
                                                       CT_AbstractCoordinateSystem::realEx z)
{
    m_xOffset = x;
    m_yOffset = y;
    m_zOffset = z;
}

bool CT_DefaultCoordinateSystem::canConvertImport(CT_AbstractCoordinateSystem::realEx x,
                                                  CT_AbstractCoordinateSystem::realEx y,
                                                  CT_AbstractCoordinateSystem::realEx z) const
{
    CT_AbstractCoordinateSystem::realEx max = std::numeric_limits<CT_AbstractCoordinateSystem::realIm>::max();
    CT_AbstractCoordinateSystem::realEx min = -max;

    CT_AbstractCoordinateSystem::realEx tmp = x-m_xOffset;

    if((tmp > max) || (tmp < min))
        return false;

    tmp = y-m_yOffset;

    if((tmp > max) || (tmp < min))
        return false;

    tmp = z-m_zOffset;

    return ((tmp > max) || (tmp < min));
}

void CT_DefaultCoordinateSystem::convertImport(CT_AbstractCoordinateSystem::realEx x,
                                               CT_AbstractCoordinateSystem::realEx y,
                                               CT_AbstractCoordinateSystem::realEx z,
                                               CT_AbstractCoordinateSystem::realIm &xOut,
                                               CT_AbstractCoordinateSystem::realIm &yOut,
                                               CT_AbstractCoordinateSystem::realIm &zOut,
                                               CT_AbstractCoordinateSystem::DatasToConvert convert) const
{
    if(convert.testFlag(CONVERT_X))
        xOut = x-m_xOffset;

    if(convert.testFlag(CONVERT_Y))
        yOut = y-m_yOffset;

    if(convert.testFlag(CONVERT_Z))
        zOut = z-m_zOffset;

    informThatUsed();
}

void CT_DefaultCoordinateSystem::convertExport(CT_AbstractCoordinateSystem::realIm x,
                                               CT_AbstractCoordinateSystem::realIm y,
                                               CT_AbstractCoordinateSystem::realIm z,
                                               CT_AbstractCoordinateSystem::realEx &xOut,
                                               CT_AbstractCoordinateSystem::realEx &yOut,
                                               CT_AbstractCoordinateSystem::realEx &zOut,
                                               CT_AbstractCoordinateSystem::DatasToConvert convert) const
{
    if(convert.testFlag(CONVERT_X))
        xOut = x+m_xOffset;

    if(convert.testFlag(CONVERT_Y))
        yOut = y+m_yOffset;

    if(convert.testFlag(CONVERT_Z))
        zOut = z+m_zOffset;

    informThatUsed();
}

void CT_DefaultCoordinateSystem::offset(CT_AbstractCoordinateSystem::realEx &x, CT_AbstractCoordinateSystem::realEx &y, CT_AbstractCoordinateSystem::realEx &z) const
{
    x = m_xOffset;
    y = m_yOffset;
    z = m_zOffset;
}

void CT_DefaultCoordinateSystem::offset(Eigen::Vector3d &v) const
{
    v(0) = m_xOffset;
    v(1) = m_yOffset;
    v(2) = m_zOffset;
}

Eigen::Matrix4d CT_DefaultCoordinateSystem::toMatrix4x4() const
{
    Eigen::Matrix4d m = Eigen::Matrix4d::Identity();
    m(0,3) = m_xOffset;
    m(1,3) = m_yOffset;
    m(2,3) = m_zOffset;

    return m;
}

GLuint CT_DefaultCoordinateSystem::indexInManager() const
{
    return PS_COORDINATES_SYS_MANAGER->indexOfCoordinateSystem(this);
}
