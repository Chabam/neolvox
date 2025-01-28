#ifndef PLYTYPE_H
#define PLYTYPE_H

#include <QString>
#include <QObject>

/**
 * @brief All types available in a ply file
 */
enum PlyType {
    PLY_unknown = 0,// unknown type
    PLY_char,       // character                 1 byte
    PLY_uchar,      // unsigned character        1 byte
    PLY_short,      // short integer             2 bytes
    PLY_ushort,     // unsigned short integer    2 bytes
    PLY_int,        // integer                   4 bytes
    PLY_uint,       // unsigned integer          4 bytes
    PLY_float,      // single-precision float    4 bytes
    PLY_double      // double-precision float    8 bytes
};

/**
 * @brief Format type of a file
 */
enum PlyFormatType {
    PLY_unknown_format = 0,
    PLY_ascii,
    PLY_binary_little_endian,
    PLY_binary_big_endian
};

/**
 * @brief Type of element
 */
enum PlyElementType {
    PLY_unknown_element_type,
    PLY_vertex,
    PLY_face,
    PLY_edge,
    PLY_custom
};

/**
 * @brief String to type converter or vice-versa
 */
class PlyTypeConverter {
public:

    /**
     * @brief Convert the word readed in the ply file to the ply file format type
     */
    static PlyFormatType staticStringToPlyFormatType(const QString& word) {

        if(word == "ascii")
            return PLY_ascii;
        else if(word == "binary_little_endian")
            return PLY_binary_little_endian;
        else  if(word == "binary_big_endian")
            return PLY_binary_big_endian;

        return PLY_unknown_format;
    }

    /**
     * @brief Convert the word readed in the ply file to the ply type
     */
    static PlyType staticStringToPlyType(const QString& word) {

        if(word == "char")
            return PLY_char;
        else if(word == "int8")
            return PLY_char;
        else if(word == "uint8")
            return PLY_uchar;
        else if(word == "uchar")
            return PLY_uchar;
        else if(word == "int16")
            return PLY_short;
        else  if(word == "short")
            return PLY_short;
        else  if(word == "uint16")
            return PLY_ushort;
        else  if(word == "ushort")
            return PLY_ushort;
        else  if(word == "int32")
            return PLY_int;
        else  if(word == "int")
            return PLY_int;
        else  if(word == "uint32")
            return PLY_uint;
        else  if(word == "uint")
            return PLY_uint;
        else  if(word == "float")
            return PLY_float;
        else  if(word == "float32")
            return PLY_float;
        else  if(word == "float64")
            return PLY_double;
        else  if(word == "double")
            return PLY_double;

        return PLY_unknown;
    }

    /**
     * @brief Convert the word readed in the ply file to the ply file format type
     */
    static PlyElementType staticStringToPlyElementType(const QString& word) {

        if(word == "vertex")
            return PLY_vertex;
        else if(word == "face")
            return PLY_face;
        else  if(word == "edge")
            return PLY_edge;
        else if(!word.isEmpty())
            return PLY_custom;

        return PLY_unknown_element_type;
    }

    /**
     * @brief Convert the ply file format type to a displayable string
     */
    static QString staticPlyFormatTypeToDisplayableString(const PlyFormatType& f) {
        if(f ==PLY_ascii)
            return QObject::tr("Ascii");
        else if(f == PLY_binary_little_endian)
            return QObject::tr("Binary little endian");
        else  if(f == PLY_binary_big_endian)
            return QObject::tr("Binary big endian");

        return QObject::tr("Unknown");
    }

    /**
     * @brief Convert the ply file format type to a displayable string
     */
    static QString staticPlyTypeToDisplayableString(const PlyType& t) {
        if(t == PLY_char)
            return QObject::tr("char");
        else if(t == PLY_uchar)
            return QObject::tr("uchar");
        else  if(t == PLY_short)
            return QObject::tr("short");
        else  if(t == PLY_ushort)
            return QObject::tr("ushort");
        else  if(t == PLY_int)
            return QObject::tr("int");
        else  if(t == PLY_uint)
            return QObject::tr("uint");
        else  if(t == PLY_float)
            return QObject::tr("float");
        else  if(t == PLY_double)
            return QObject::tr("double");

        return QObject::tr("unknown");
    }
};

#endif // PLYTYPE_H
