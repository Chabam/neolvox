#ifndef CT_LASDATA_H
#define CT_LASDATA_H

#include <QtGlobal>
#include "ctliblas/ctliblas_global.h"


class CTLIBLAS_EXPORT CT_LASData
{
public:
    CT_LASData();

    quint16 _Intensity;
    quint16 _Return_Number;
    quint16 _Number_of_Returns;
    quint16 _Classification_Flags;
    quint16 _Scanner_Channel;
    quint16 _Scan_Direction_Flag;
    quint16 _Edge_of_Flight_Line;
    quint8  _Classification;
    qint16  _Scan_Angle_Rank;
    quint8  _User_Data;
    quint16 _Point_Source_ID;
    double  _GPS_Time;
    quint16 _Red;
    quint16 _Green;
    quint16 _Blue;
    quint16 _NIR;
    quint8  _Wave_Packet_Descriptor_Index;
    quint64 _Byte_offset_to_waveform_data;
    quint32 _Waveform_packet_size_in_bytes;
    float   _Return_Point_Waveform_Location;

};

#endif // CT_LASDATA_H
