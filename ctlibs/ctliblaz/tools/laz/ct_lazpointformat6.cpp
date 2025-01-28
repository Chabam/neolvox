#include "ct_lazpointformat6.h"

#include "ct_point.h"

#include "laszip/laszip_api.h"

CT_LAZPointFormat6::CT_LAZPointFormat6() : CT_AbstractLAZPointFormat()
{
}

size_t CT_LAZPointFormat6::sizeInBytes() const
{
    return sizeof(m_emptyData);
}

void CT_LAZPointFormat6::write(laszip_point* lzPoint, CT_LAZHeader* header, const CT_Point &p, const size_t &globalIndex) const
{
    qint32 m_x, m_y, m_z;
    header->inverseTransformPoint(p(CT_Point::X), p(CT_Point::Y), p(CT_Point::Z), m_x, m_y, m_z);

    // x / y / z
    // stream << m_x << m_y << m_z;
    lzPoint->X = m_x;
    lzPoint->Y = m_y;
    lzPoint->Z = m_z;

    // intensity
    // CT_WRITE_LAS_SCALAR(CT_LasDefine::Intensity, quint16);
    CT_WRITE_LAZ_SCALAR(CT_LasDefine::Intensity, quint16, intensity);

    // return number + number of return
    // quint8 tmp = 0;

    const CT_AbstractPointAttributesScalar* sc;

    if(((sc = scalars(CT_LasDefine::Return_Number)) != nullptr) && sc->hasBeenSet(globalIndex))
        // tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 0);
        lzPoint->extended_return_number = ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) & 0x0F);

    if(((sc = scalars(CT_LasDefine::Number_of_Returns)) != nullptr) && sc->hasBeenSet(globalIndex))
        // tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 4);
        lzPoint->extended_number_of_returns = ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) & 0x0F);

    // stream << tmp;

    // classification flags + scanner channel + scan direction flag + edge of flight line
    // tmp = 0;


    if(((sc = scalars(CT_LasDefine::Classification_Flags)) != nullptr) && sc->hasBeenSet(globalIndex))
        // tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 0);
        lzPoint->extended_classification_flags = ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) & 0x0F);

    if(((sc = scalars(CT_LasDefine::Scanner_Channel)) != nullptr) && sc->hasBeenSet(globalIndex))
        // tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 4);
        lzPoint->extended_scanner_channel = ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) & 0x02);

    if(((sc = scalars(CT_LasDefine::Scan_Direction_Flag)) != nullptr) && sc->hasBeenSet(globalIndex))
        // tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 6);
        lzPoint->scan_direction_flag = ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) & 0x01);

    if(((sc = scalars(CT_LasDefine::Edge_of_Flight_Line)) != nullptr) && sc->hasBeenSet(globalIndex))
        // tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 7);
        lzPoint->edge_of_flight_line = ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) & 0x01);

    // stream << tmp;

    // classification
    // CT_WRITE_LAS_SCALAR(CT_LasDefine::Classification, quint8);
    CT_WRITE_LAZ_SCALAR(CT_LasDefine::Classification, quint8, extended_classification);

    // user data
    // CT_WRITE_LAS_SCALAR(CT_LasDefine::User_Data, quint8);
    CT_WRITE_LAZ_SCALAR(CT_LasDefine::User_Data, quint8, user_data);

    // scan angle
    // CT_WRITE_LAS_SCALAR(CT_LasDefine::Scan_Angle_Rank, qint16);
    CT_WRITE_LAZ_SCALAR(CT_LasDefine::Scan_Angle_Rank, qint16, extended_scan_angle);

    // point source ID
    // CT_WRITE_LAS_SCALAR(CT_LasDefine::Point_Source_ID, quint16);
    CT_WRITE_LAZ_SCALAR(CT_LasDefine::Point_Source_ID, quint16, point_source_ID);

    // gps time
    // CT_WRITE_LAS_SCALAR(CT_LasDefine::GPS_Time, double);
    CT_WRITE_LAZ_SCALAR(CT_LasDefine::GPS_Time, double, gps_time);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LAZPointFormat6::typesToSearch()
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
