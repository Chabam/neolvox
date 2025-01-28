#ifndef CT_ABSTRACTCATEGORY_H
#define CT_ABSTRACTCATEGORY_H

#include "ct_model/interfacesformodels.h"

#include "ct_tools/ct_typeinfo.h"

#include <QObject>
#include <QString>
#include <QDebug>
#include <typeinfo>

#define CT_SPECIALIZE_VALUE_TYPE_TO(SCALARTYPE, CATEGORY_TYPE, CATEGORY_TYPE_STRING) \
template<> \
inline constexpr CT_AbstractCategory::ValueType CT_AbstractCategory::staticValueTypeToCategoryType<SCALARTYPE>() { return CATEGORY_TYPE; } \
template<> \
inline QString CT_AbstractCategory::staticValueTypeToCategoryTypeString<SCALARTYPE>() { return QString(CATEGORY_TYPE_STRING); }

/**
 * @brief \defgroup CT_AbstractCategoryClass
 *        A category is used in conjonction with a CT_ItemAttribute. It represent a type of
 *        data (x, y, z coordinate or a radius or a height, etc....) and has a type of
 *        value (boolean or string or number or real number, etc....). You can test if a category
 *        is equivalent to another by use the method "isEquivalentTo(...)".
 */
class CTLIBMODELS_EXPORT CT_AbstractCategory : public QObject, public ICategoryForModel
{
    Q_OBJECT

public:

    /**
     * @brief Type of value (boolean or number or string, etc...)
     */
    enum ValueType {
        BOOLEAN       = 1,      // a boolean

        DOUBLE        = 2,      // a double (it's a NUMBER_REAL too)
        FLOAT         = 4,      // un float (it's a NUMBER_REAL too)
        LONG_DOUBLE   = 8,      // un long double (it's a NUMBER_REAL too)

        NUMBER_REAL   = 14,     // a real number (it's a NUMBER too)

        INT8          = 16,     // a integer 8 bits (it's a NUMBER_INT too)
        UINT8         = 32,     // a unsigned integer 8 bits (it's a NUMBER_INT too)
        INT16         = 64,     // a integer 16 bits (it's a NUMBER_INT too)
        UINT16        = 128,    // a unsigned integer 16 bits (it's a NUMBER_INT too)
        INT32         = 256,    // a signed integer 32 bits (it's a NUMBER_INT too)
        UINT32        = 512,    // a unsigned integer 32 bits (it's a NUMBER_INT too)
        INT64         = 1024,   // a signed integer 64 bits (it's a NUMBER_INT too)
        UINT64        = 2048,   // a unsigned integer 64 bits (it's a NUMBER_INT too)

#if defined(__x86_64__) || _M_X64 // Definition for GCC, Clang, and Intel's compiler or MSVC++
        SIZE_T        = UINT64,    // a unsigned integer 64 bits (it's a NUMBER_INT too)
#else
        SIZE_T        = UINT32,    // a unsigned integer 32 bits (it's a NUMBER_INT too)
#endif

        NUMBER_INT    = 4080,   // a integer (it's a NUMBER too) = (16+32+64+128+256+....)

        NUMBER        = 4094,   // a number = (NUMBER_INT + NUMBER_REAL)

        STRING        = 4096,   // a string

        UNKNOWN       = 8192,   // a unknown type

        ANY           = 16383   // Any ValueType (including UNKNOWN)
    };
    Q_ENUMS(ValueType)

    /**
     * @brief Type of data (generic). Use them for uniqueName to create default categories.
     */
    static const QString DATA_VALUE;                // generic value
    static const QString DATA_ID;                   // unique id (equivalent to "DATA_VALUE")
    static const QString DATA_DISPLAYABLE_NAME;     // displayable name (equivalent to "DATA_VALUE")
    static const QString DATA_FILE_NAME;            // File name (equivalent to "DATA_VALUE")
    static const QString DATA_COORDINATE;           // generic coordinate (equivalent to "DATA_VALUE")
    static const QString DATA_X;                    // x coordinate (equivalent to "DATA_COORDINATE")
    static const QString DATA_Y;                    // y coordinate (equivalent to "DATA_COORDINATE")
    static const QString DATA_Z;                    // z coordinate (equivalent to "DATA_COORDINATE")
    static const QString DATA_CX;                   // x center coordinate (equivalent to "DATA_X")
    static const QString DATA_CY;                   // y center coordinate (equivalent to "DATA_Y")
    static const QString DATA_CZ;                   // z center coordinate (equivalent to "DATA_Z")
    static const QString DATA_DIRECTION;            // generic direction (equivalent to "DATA_VALUE")
    static const QString DATA_X_DIRECTION;          // x direction (equivalent to "DATA_X" and "DATA_DIRECTION")
    static const QString DATA_Y_DIRECTION;          // y direction (equivalent to "DATA_Y" and "DATA_DIRECTION")
    static const QString DATA_Z_DIRECTION;          // z direction (equivalent to "DATA_Z" and "DATA_DIRECTION")
    static const QString DATA_DIMENSION;            // generic dimension (equivalent to "DATA_VALUE")
    static const QString DATA_X_DIMENSION;          // x dimension (equivalent to "DATA_X" and "DATA_DIMENSION")
    static const QString DATA_Y_DIMENSION;          // y dimension (equivalent to "DATA_Y" and "DATA_DIMENSION")
    static const QString DATA_Z_DIMENSION;          // z dimension (equivalent to "DATA_Z" and "DATA_DIMENSION")
    static const QString DATA_RADIUS;               // radius (equivalent to "DATA_VALUE")
    static const QString DATA_R2;                   // r² for fitting (equivalent to "DATA_VALUE")
    static const QString DATA_SIZE;                 // size (equivalent to "DATA_VALUE")
    static const QString DATA_NUMBER;               // number of elemnts (equivalent to "DATA_SIZE")
    static const QString DATA_LENGTH;               // length (equivalent to "DATA_SIZE")
    static const QString DATA_WIDTH;                // width (equivalent to "DATA_LENGTH")
    static const QString DATA_HEIGHT;               // height (equivalent to "DATA_LENGTH")
    static const QString DATA_AREA;                 // area (equivalent to "DATA_VALUE")
    static const QString DATA_ANGLE;                // angle (equivalent to "DATA_VALUE")
    static const QString DATA_RATIO;                // ratio (equivalent to "DATA_VALUE")
    static const QString DATA_RESOLUTION;           // generic resolution (equivalent to "DATA_VALUE")
    static const QString DATA_X_RESOLUTION;         // x resolution (equivalent to "DATA_X" and "DATA_RESOLUTION")
    static const QString DATA_Y_RESOLUTION;         // y resolution (equivalent to "DATA_Y" and "DATA_RESOLUTION")
    static const QString DATA_Z_RESOLUTION;         // z resolution (equivalent to "DATA_Z" and "DATA_RESOLUTION")
    static const QString DATA_NA;                   // NA (equivalent to "DATA_VALUE")

    /**
     * @brief Construct a category
     * @param uniqueName : a category must have a unique name to find it in a list.
     * @param categoryEquivalentUniqueName : a unique name of a category that can be equivalent to this category
     * @param displayableName : a displayable name (by default the unique name if empty)
     * @param description : a description
     */
    CT_AbstractCategory(const QString &uniqueName,
                        const QString &categoryEquivalentUniqueName = "",
                        const QString &displayableName = "",
                        const QString &description = "");

    /**
     * @brief Construct a category
     * @param uniqueName : a category must have a unique name to find it in a list
     * @param categoryEquivalentUniqueNameCollection : a collection of unique name of categories that can be equivalent to this category
     * @param displayableName : a displayable name (by default the unique name if empty)
     * @param description : a description
     */
    CT_AbstractCategory(const QString &uniqueName,
                        const QList<QString> &categoryEquivalentUniqueNameCollection,
                        const QString &displayableName = "",
                        const QString &description = "");

    virtual ~CT_AbstractCategory() {}

    /**
     * @brief Add a new equivalent category
     */
    bool addEquivalentCategory(const QString &categoryEquivalentUniqueName);

    /**
     * @brief Returns the unique name of the category that can be used to find it in a list
     */
    QString uniqueName() const override;

    /**
     * @brief Convert a value type to a displayable string
     */
    static QString valueTypeToString(ValueType v);

    /**
     * @brief Convert a displayable string to a value type
     * @param ok : to true if String was found
     * @warning The string must have been generated by the method "valueTypeToString"
     */
    static ValueType stringToValueType(const QString &s, bool *ok = nullptr);

    /**
     * @brief Returns the displayable name
     */
    QString displayableName() const;

    /**
     * @brief Returns the description
     */
    QString description() const;

    /**
     * @brief Returns true if this category is equivalent to category passed in parameter
     */
    using ICategoryForModel::isEquivalentTo;
    virtual bool isEquivalentTo(const CT_AbstractCategory* c) const = 0;

    /**
     * @brief Convert a type of data to a type of data of a category
     */
    template <typename VType>
    constexpr static CT_AbstractCategory::ValueType staticValueTypeToCategoryType()
    {
        //qFatal(qPrintable(QString("staticValueTypeToCategoryType unknown data type (") + CT_TypeInfo::name<VType>() + ") ===>" + QString(__FILE__) + QString(__LINE__)));

        return CT_AbstractCategory::UNKNOWN;
    }

    /**
     * @brief Convert a type of data to a type of data of a category (String)
     */
    template <typename VType>
    static QString staticValueTypeToCategoryTypeString()
    {
        //qDebug() << QString("staticValueTypeToCategoryType unknown data type (") + CT_TypeInfo::name<VType>() + ") ===>" + QString(__FILE__) + QString(__LINE__);

        return QString("UNKNOWN (%1)").arg(CT_TypeInfo::name<VType>());
    }

    static QString staticInitDataValue() { return "CT_VALUE"; }
    static QString staticInitDataId() { return "CT_ID"; }
    static QString staticInitDataDisplayableName() { return "CT_DISPLAYABLE_NAME"; }
    static QString staticInitDataFileName() { return "CT_FILE_NAME"; }
    static QString staticInitDataCoordinate() { return "CT_COORDINATE"; }
    static QString staticInitDataX() { return "CT_X"; }
    static QString staticInitDataY() { return "CT_Y"; }
    static QString staticInitDataZ() { return "CT_Z"; }
    static QString staticInitDataCx() { return "CT_CX"; }
    static QString staticInitDataCy() { return "CT_CY"; }
    static QString staticInitDataCz() { return "CT_CZ"; }
    static QString staticInitDataDirection() { return "CT_DIRECTION"; }
    static QString staticInitDataXDirection() { return "CT_X_DIRECTION"; }
    static QString staticInitDataYDirection() { return "CT_Y_DIRECTION"; }
    static QString staticInitDataZDirection() { return "CT_Z_DIRECTION"; }
    static QString staticInitDataDimension() { return "CT_DIMENSION"; }
    static QString staticInitDataXDimension() { return "CT_X_DIMENSION"; }
    static QString staticInitDataYDimension() { return "CT_Y_DIMENSION"; }
    static QString staticInitDataZDimension() { return "CT_Z_DIMENSION"; }
    static QString staticInitDataRadius() { return "CT_RADIUS"; }
    static QString staticInitDataR2() { return "CT_R2"; }
    static QString staticInitDataSize() { return "CT_SIZE"; }
    static QString staticInitDataNumber() { return "CT_NUMBER"; }
    static QString staticInitDataLength() { return "CT_LENGTH"; }
    static QString staticInitDataWidth() { return "CT_WIDTH"; }
    static QString staticInitDataHeight() { return "CT_HEIGHT"; }
    static QString staticInitDataArea() { return "CT_AREA"; }
    static QString staticInitDataAngle() { return "CT_ANGLE"; }
    static QString staticInitDataRatio() { return "CT_RATIO"; }
    static QString staticInitDataResolution() { return "CT_RESOLUTION"; }
    static QString staticInitDataXResolution() { return "CT_X_RESOLUTION"; }
    static QString staticInitDataYResolution() { return "CT_Y_RESOLUTION"; }
    static QString staticInitDataZResolution() { return "CT_Z_RESOLUTION"; }
    static QString staticInitDataNa() { return "CT_NA"; }

protected:
    QString         m_uName;
    ValueType       m_vType;
    QString         m_dName;
    QString         m_description;
    QList<QString>  m_eCategories;

    /**
     * @brief check attributes and assert if it was a problem
     */
    void internalTestAttributes(const QString &where);
};

CT_SPECIALIZE_VALUE_TYPE_TO(bool, CT_AbstractCategory::BOOLEAN, "BOOL")
CT_SPECIALIZE_VALUE_TYPE_TO(double, CT_AbstractCategory::DOUBLE, "DOUBLE")
CT_SPECIALIZE_VALUE_TYPE_TO(float, CT_AbstractCategory::FLOAT, "FLOAT")
CT_SPECIALIZE_VALUE_TYPE_TO(long double, CT_AbstractCategory::LONG_DOUBLE, "LONG_DOUBLE")
CT_SPECIALIZE_VALUE_TYPE_TO(qint8, CT_AbstractCategory::INT8, "INT8")
CT_SPECIALIZE_VALUE_TYPE_TO(quint8, CT_AbstractCategory::UINT8, "UNSIGNED INT8")
CT_SPECIALIZE_VALUE_TYPE_TO(qint16, CT_AbstractCategory::INT16, "INT16")
CT_SPECIALIZE_VALUE_TYPE_TO(quint16, CT_AbstractCategory::UINT16, "UNSIGNED INT16")
CT_SPECIALIZE_VALUE_TYPE_TO(qint32, CT_AbstractCategory::INT32, "INT32")
CT_SPECIALIZE_VALUE_TYPE_TO(quint32, CT_AbstractCategory::UINT32, "UNSIGNED INT32")
CT_SPECIALIZE_VALUE_TYPE_TO(unsigned long, CT_AbstractCategory::UINT32, "UNSIGNED INT32")
CT_SPECIALIZE_VALUE_TYPE_TO(long int, CT_AbstractCategory::INT32, "INT32")
CT_SPECIALIZE_VALUE_TYPE_TO(qint64, CT_AbstractCategory::INT64, "INT64")
CT_SPECIALIZE_VALUE_TYPE_TO(quint64, CT_AbstractCategory::UINT64, "UNSIGNED INT64")
CT_SPECIALIZE_VALUE_TYPE_TO(QString, CT_AbstractCategory::STRING, "STRING")

#endif // CT_ABSTRACTCATEGORY_H
