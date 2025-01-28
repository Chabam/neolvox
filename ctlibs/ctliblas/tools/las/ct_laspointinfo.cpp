#include "ct_laspointinfo.h"

CT_LasPointInfo::CT_LasPointInfo()
{
    m_i = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_rn = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_nor = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_cf = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_sc = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_sdf = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_efl = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_c = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_sar = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_ud = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_psID = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_gpsT = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_red = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_green = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_blue = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_color = qMakePair((const CT_PointsAttributesColor*)nullptr, (size_t)0);
    m_nir = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_wpdi = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_bowd = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_wpsb = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
    m_rpwl = qMakePair((const CT_AbstractPointAttributesScalar*)nullptr, (size_t)0);
}

bool CT_LasPointInfo::setAttribute(CT_LasDefine::LASPointAttributesType type, const CT_AbstractPointAttributesScalar *scalar, const size_t &index)
{
    switch(type)
    {
    case CT_LasDefine::Intensity : m_i = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Return_Number: m_rn = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Number_of_Returns: m_nor = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Classification_Flags: m_cf = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Scanner_Channel: m_sc = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Scan_Direction_Flag: m_sdf = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Edge_of_Flight_Line: m_efl = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Classification: m_c = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Scan_Angle_Rank: m_sar = qMakePair(scalar, index);
        break;

    case CT_LasDefine::User_Data: m_ud = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Point_Source_ID: m_psID = qMakePair(scalar, index);
        break;

    case CT_LasDefine::GPS_Time: m_gpsT = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Red: m_red = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Green: m_green = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Blue: m_blue = qMakePair(scalar, index);
        break;

    case CT_LasDefine::NIR: m_nir = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Wave_Packet_Descriptor_Index: m_wpdi = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Byte_offset_to_waveform_data: m_bowd = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Waveform_packet_size_in_bytes: m_wpsb = qMakePair(scalar, index);
        break;

    case CT_LasDefine::Return_Point_Waveform_Location: m_rpwl = qMakePair(scalar, index);
        break;

    default:
        return false;
    }

    return true;
}

void CT_LasPointInfo::setColorAttribute(const CT_PointsAttributesColor* color, const size_t& index)
{
    m_color = qMakePair(color, index);
}
