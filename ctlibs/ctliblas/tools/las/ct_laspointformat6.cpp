#include "ct_laspointformat6.h"

#include "ct_point.h"

CT_LASPointFormat6::CT_LASPointFormat6() : CT_AbstractLASPointFormat()
{
}

size_t CT_LASPointFormat6::sizeInBytes() const
{
    return sizeof(m_emptyData);
}

void CT_LASPointFormat6::write(QDataStream &stream, CT_LASHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    qint32 m_x, m_y, m_z;
    header->inverseTransformPoint(p(CT_Point::X), p(CT_Point::Y), p(CT_Point::Z), m_x, m_y, m_z);

    // x / y / z
    stream << m_x << m_y << m_z;

    // intensity
    CT_WRITE_LAS_SCALAR(CT_LasDefine::Intensity, quint16);

    // return number + number of return
    quint8 tmp = 0;

    const CT_AbstractPointAttributesScalar* sc;

    if(((sc = scalars(CT_LasDefine::Return_Number)) != nullptr) && sc->hasBeenSet(globalIndex))
        tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 0);

    if(((sc = scalars(CT_LasDefine::Number_of_Returns)) != nullptr) && sc->hasBeenSet(globalIndex))
        tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 4);

    stream << tmp;

    // classification flags + scanner channel + scan direction flag + edge of flight line
    tmp = 0;


    if(((sc = scalars(CT_LasDefine::Classification_Flags)) != nullptr) && sc->hasBeenSet(globalIndex))
        tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 0);

    if(((sc = scalars(CT_LasDefine::Scanner_Channel)) != nullptr) && sc->hasBeenSet(globalIndex))
        tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 4);

    if(((sc = scalars(CT_LasDefine::Scan_Direction_Flag)) != nullptr) && sc->hasBeenSet(globalIndex))
        tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 6);

    if(((sc = scalars(CT_LasDefine::Edge_of_Flight_Line)) != nullptr) && sc->hasBeenSet(globalIndex))
        tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 7);

    stream << tmp;

    // classification
    CT_WRITE_LAS_SCALAR(CT_LasDefine::Classification, quint8);

    // user data
    CT_WRITE_LAS_SCALAR(CT_LasDefine::User_Data, quint8);

    // scan angle
    CT_WRITE_LAS_SCALAR(CT_LasDefine::Scan_Angle_Rank, qint16);

    // point source ID
    CT_WRITE_LAS_SCALAR(CT_LasDefine::Point_Source_ID, quint16);

    // gps time
    CT_WRITE_LAS_SCALAR(CT_LasDefine::GPS_Time, double);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LASPointFormat6::typesToSearch()
{
    QList<CT_LasDefine::LASPointAttributesType> l;
    l << CT_LasDefine::Intensity;
    l << CT_LasDefine::Return_Number;
    l << CT_LasDefine::Number_of_Returns;
    l << CT_LasDefine::Classification_Flags;
    l << CT_LasDefine::Scanner_Channel;
    l << CT_LasDefine::Scan_Direction_Flag;
    l << CT_LasDefine::Edge_of_Flight_Line;
    l << CT_LasDefine::Classification;
    l << CT_LasDefine::Scan_Angle_Rank;
    l << CT_LasDefine::User_Data;
    l << CT_LasDefine::Point_Source_ID;
    l << CT_LasDefine::GPS_Time;

    return l;
}
