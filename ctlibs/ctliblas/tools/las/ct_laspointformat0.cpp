#include "ct_laspointformat0.h"

#include "ct_point.h"

CT_LASPointFormat0::CT_LASPointFormat0() : CT_AbstractLASPointFormat()
{
}

size_t CT_LASPointFormat0::sizeInBytes() const
{
    return sizeof(m_emptyData);
}

void CT_LASPointFormat0::write(QDataStream &stream, CT_LASHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    qint32 m_x, m_y, m_z;
    header->inverseTransformPoint(p(CT_Point::X), p(CT_Point::Y), p(CT_Point::Z), m_x, m_y, m_z);

    // x / y / z
    stream << m_x << m_y << m_z;

    // intensity
    CT_WRITE_LAS_SCALAR(CT_LasDefine::Intensity, quint16);

    // return number + number of return + scan direction flag + edge of flight line
    quint8 tmp = 0;

    const CT_AbstractPointAttributesScalar* sc;

    if(((sc = scalars(CT_LasDefine::Return_Number)) != nullptr) && sc->hasBeenSet(globalIndex))
        tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 0);

    if(((sc = scalars(CT_LasDefine::Number_of_Returns)) != nullptr) && sc->hasBeenSet(globalIndex))
        tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 3);

    if(((sc = scalars(CT_LasDefine::Scan_Direction_Flag)) != nullptr) && sc->hasBeenSet(globalIndex))
        tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 6);

    if(((sc = scalars(CT_LasDefine::Edge_of_Flight_Line)) != nullptr) && sc->hasBeenSet(globalIndex))
        tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 7);

    stream << tmp;

    // classification
    CT_WRITE_LAS_SCALAR(CT_LasDefine::Classification, quint8);

    // scan angle rank
    CT_WRITE_LAS_SCALAR(CT_LasDefine::Scan_Angle_Rank, qint8);

    // user data
    CT_WRITE_LAS_SCALAR(CT_LasDefine::User_Data, quint8);

    // point source ID
    CT_WRITE_LAS_SCALAR(CT_LasDefine::Point_Source_ID, quint16);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LASPointFormat0::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l;
    l << CT_LasDefine::Intensity;
    l << CT_LasDefine::Return_Number;
    l << CT_LasDefine::Number_of_Returns;
    l << CT_LasDefine::Scan_Direction_Flag;
    l << CT_LasDefine::Edge_of_Flight_Line;
    l << CT_LasDefine::Classification;
    l << CT_LasDefine::Scan_Angle_Rank;
    l << CT_LasDefine::User_Data;
    l << CT_LasDefine::Point_Source_ID;

    return l;
}
