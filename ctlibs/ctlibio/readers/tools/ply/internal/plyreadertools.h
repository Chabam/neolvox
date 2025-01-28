#ifndef PLYREADERTOOLS_H
#define PLYREADERTOOLS_H

#include "plyproperty.h"

class PlyReaderTools {
public:
    /**
     * @brief Read a specific type of value with the streamer and return it
     */
    template<typename Streamer, typename ValueType>
    static inline ValueType staticReadValue(Streamer& stream) {
        ValueType v;
        stream >> v;
        return v;
    }

    /**
     * @brief Read the value of a property with the streamer and return it
     */
    template<typename Streamer>
    static inline double staticReadProperty(Streamer& stream, const PlyProperty& property) {
        if(property.getValueType() == PLY_char) {
            return staticReadValue<Streamer, qint8>(stream);
        } else if(property.getValueType() == PLY_uchar) {
            return staticReadValue<Streamer, quint8>(stream);
        } else if(property.getValueType() == PLY_short) {
            return staticReadValue<Streamer, qint16>(stream);
        } else if(property.getValueType() == PLY_ushort) {
            return staticReadValue<Streamer, quint16>(stream);
        } else if(property.getValueType() == PLY_int) {
            return staticReadValue<Streamer, qint32>(stream);
        } else if(property.getValueType() == PLY_uint) {
            return staticReadValue<Streamer, quint32>(stream);
        } else if(property.getValueType() == PLY_float) {
            stream.setFloatingPointPrecision(Streamer::SinglePrecision);
            return staticReadValue<Streamer, float>(stream);
        } else if(property.getValueType() == PLY_double) {
            stream.setFloatingPointPrecision(Streamer::DoublePrecision);
            return staticReadValue<Streamer, double>(stream);
        }

        return qQNaN();
    }

    static inline qint8 staticPropertySizeInByte(const PlyProperty& property) {
        if(property.getValueType() == PLY_char)
            return 1;
        else if(property.getValueType() == PLY_uchar)
            return 1;
        else if(property.getValueType() == PLY_short)
            return 2;
        else if(property.getValueType() == PLY_ushort)
            return 2;
        else if(property.getValueType() == PLY_int)
            return 4;
        else if(property.getValueType() == PLY_uint)
            return 4;
        else if(property.getValueType() == PLY_float)
            return 4;
        else if(property.getValueType() == PLY_double)
            return 8;

        return -1;
    }
};

#endif // PLYREADERTOOLS_H
