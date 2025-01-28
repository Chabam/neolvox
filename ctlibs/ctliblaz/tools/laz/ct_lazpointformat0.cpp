#include "ct_lazpointformat0.h"

#include "ct_point.h"

#include "laszip/laszip_api.h"

CT_LAZPointFormat0::CT_LAZPointFormat0() : CT_AbstractLAZPointFormat()
{
}

size_t CT_LAZPointFormat0::sizeInBytes() const
{
    return sizeof(m_emptyData);
}

void CT_LAZPointFormat0::write(laszip_point* lzPoint, CT_LAZHeader* header, const CT_Point &p, const size_t &globalIndex) const
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

    // return number + number of return + scan direction flag + edge of flight line
    // quint8 tmp = 0;

    const CT_AbstractPointAttributesScalar* sc;

    if(((sc = scalars(CT_LasDefine::Return_Number)) != nullptr) && sc->hasBeenSet(globalIndex))
        // tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 0);
        lzPoint->return_number = ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) & 0x07);

    if(((sc = scalars(CT_LasDefine::Number_of_Returns)) != nullptr) && sc->hasBeenSet(globalIndex))
        // tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 3);
        lzPoint->number_of_returns = ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) & 0x07);

    if(((sc = scalars(CT_LasDefine::Scan_Direction_Flag)) != nullptr) && sc->hasBeenSet(globalIndex))
        // tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 6);
        lzPoint->scan_direction_flag = ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) & 0x01);

    if(((sc = scalars(CT_LasDefine::Edge_of_Flight_Line)) != nullptr) && sc->hasBeenSet(globalIndex))
        // tmp |= ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) << 7);
        lzPoint->edge_of_flight_line = ((static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex))) & 0x01);

    // stream << tmp;

    // classification
    // CT_WRITE_LAS_SCALAR(CT_LasDefine::Classification, quint8);
    if(((sc = scalars(CT_LasDefine::Classification)) != nullptr) && sc->hasBeenSet(globalIndex)) {
        quint8 classification = static_cast<quint8>(sc->scalarAsDoubleAt(globalIndex));

        lzPoint->classification = classification & 0x1F;
        lzPoint->synthetic_flag = (classification >> 5) & 0x01;
        lzPoint->keypoint_flag = (classification >> 6) & 0x01;
        lzPoint->withheld_flag = (classification >> 7) & 0x01;
    }

    // scan angle rank
    // CT_WRITE_LAS_SCALAR(CT_LasDefine::Scan_Angle_Rank, qint8);
    CT_WRITE_LAZ_SCALAR(CT_LasDefine::Scan_Angle_Rank, qint8, scan_angle_rank);

    // user data
    // CT_WRITE_LAS_SCALAR(CT_LasDefine::User_Data, quint8);
    CT_WRITE_LAZ_SCALAR(CT_LasDefine::User_Data, quint8, user_data);

    // point source ID
    // CT_WRITE_LAS_SCALAR(CT_LasDefine::Point_Source_ID, quint16);
    CT_WRITE_LAZ_SCALAR(CT_LasDefine::Point_Source_ID, quint16, point_source_ID);
}

QList<CT_LasDefine::LASPointAttributesType> CT_LAZPointFormat0::typesToSearch()
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
