#include "ct_shootingpatternd.h"

CT_DEFAULT_IA_INIT(CT_ShootingPatternD)

CT_ShootingPatternD::CT_ShootingPatternD() : SuperClass(),
    m_pattern(nullptr)
{
}

CT_ShootingPatternD::CT_ShootingPatternD(CT_ShootingPattern* pattern) : SuperClass(),
    m_pattern(pattern)
{
    if(m_pattern != nullptr)
        setCenterCoordinate(m_pattern->centerCoordinate());
}

CT_ShootingPatternD::CT_ShootingPatternD(const CT_ShootingPatternD& other) : SuperClass(other)
{
    m_pattern = ((other.m_pattern == nullptr) ? nullptr : other.m_pattern->clone());
}

CT_ShootingPatternD::~CT_ShootingPatternD()
{
    if(m_pattern != nullptr)
        delete m_pattern;
}

CT_ShootingPattern* CT_ShootingPatternD::getShootingPattern() const
{
    return m_pattern;
}

size_t CT_ShootingPatternD::getNumberOfShots() const
{
    return m_pattern->numberOfShots();
}

CT_Shot CT_ShootingPatternD::getShotAt(const size_t &index) const
{
    return m_pattern->shotAt(index);
}

