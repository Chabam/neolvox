#ifndef CT_ABSTRACTLAZPOINTFORMAT_H
#define CT_ABSTRACTLAZPOINTFORMAT_H

#include <cstring>

#include "ctliblas/tools/las/ct_laspointinfo.h"
// #include "ctliblas/readers/headers/ct_lasheader.h"
#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"
#include "ct_itemdrawable/ct_pointsattributescolor.h"

#include "ctliblaz/readers/headers/ct_lazheader.h"

#include "laszip/laszip_api.h"

#define CT_WRITE_LAZ_SCALAR(LAS_DEFINE_TYPE, CAST_TO, LASZIP_POINT_FIELD) \
    { \
        const CT_AbstractPointAttributesScalar* sc; \
        \
        if(((sc = scalars(LAS_DEFINE_TYPE)) != nullptr) && sc->hasBeenSet(globalIndex)) \
            lzPoint->LASZIP_POINT_FIELD = static_cast<CAST_TO>(sc->scalarAsDoubleAt(globalIndex)); \
        else \
            lzPoint->LASZIP_POINT_FIELD = static_cast<CAST_TO>(0); \
    }


#define CT_WRITE_LAZ_SCALAR_WITH(ATTRIBUTES, CAST_TO, LASZIP_POINT_FIELD) \
    { \
        if((ATTRIBUTES != nullptr) && ATTRIBUTES->hasBeenSet(globalIndex)) \
            lzPoint->LASZIP_POINT_FIELD = static_cast<CAST_TO>(ATTRIBUTES->scalarAsDoubleAt(globalIndex)); \
        else \
            lzPoint->LASZIP_POINT_FIELD = static_cast<CAST_TO>(0); \
    }


#define LASZIP_POINT_WAVE_PACKET_WPDI 0
#define LASZIP_POINT_WAVE_PACKET_BOWD (LASZIP_POINT_WAVE_PACKET_WPDI + sizeof(quint8))
#define LASZIP_POINT_WAVE_PACKET_WPSB (LASZIP_POINT_WAVE_PACKET_BOWD + sizeof(quint64))
#define LASZIP_POINT_WAVE_PACKET_RPWL (LASZIP_POINT_WAVE_PACKET_WPSB + sizeof(quint32))
#define LASZIP_POINT_WAVE_PACKET_XT   (LASZIP_POINT_WAVE_PACKET_RPWL + sizeof(float))
#define LASZIP_POINT_WAVE_PACKET_YT   (LASZIP_POINT_WAVE_PACKET_XT   + sizeof(float))
#define LASZIP_POINT_WAVE_PACKET_ZT   (LASZIP_POINT_WAVE_PACKET_YT   + sizeof(float))

#define CT_WRITE_LAZ_WAVE_PACKET_ITEM_WITH(ATTRIBUTES, TYPE, LASZIP_POINT_WAVE_PACKET_OFFSET) \
    { \
        TYPE itemValue; \
\
        if((ATTRIBUTES != nullptr) && ATTRIBUTES->hasBeenSet(globalIndex)) \
            itemValue = static_cast<TYPE>(ATTRIBUTES->scalarAsDoubleAt(globalIndex)); \
        else \
            itemValue = static_cast<TYPE>(0); \
\
        memcpy(lzPoint->wave_packet + LASZIP_POINT_WAVE_PACKET_OFFSET, &itemValue, sizeof(TYPE)); \
    }


/**
 * @brief A tool class that can be use to write a point in a LAS File
 */
class CTLIBLAZ_EXPORT CT_AbstractLAZPointFormat
{
public:
    // a type = an attribute (if there is no value for a type in the collection it means that no attribute has been found for this type)
    using AttributeByTypeCollection = QMultiHash<CT_LasDefine::LASPointAttributesType, const CT_AbstractPointAttributesScalar*>;

    CT_AbstractLAZPointFormat();
    virtual ~CT_AbstractLAZPointFormat();

    /**
     * @brief Return the size of the points in byte
     */
    virtual size_t sizeInBytes() const = 0;

    /**
     * @brief Clear infos of all points
     */
    void clearInfos();

    /**
     * @brief Call this method before use the method "write" and after "setAttributes". This will create the
     *        ReturnNumber informations for each points to write.
     */
    bool initReturnNumber(const AttributeByTypeCollection& attributes);

    /**
     * @brief Call this method before use the method "write" and after "setAttributes". This will create all
     *        informations necessary for each points to write.
     */
    bool initWrite(const AttributeByTypeCollection& attributes, const CT_PointsAttributesColor* colors);

    const CT_AbstractPointAttributesScalar* scalars(const CT_LasDefine::LASPointAttributesType& type) const;
    const CT_PointsAttributesColor* colors() const;

    /**
     * @brief Write the point 'globalIndex' (index in the global pointCloud)
     */
    virtual void write(laszip_point* lzPoint, CT_LAZHeader* header, const CT_Point &p, const size_t &globalIndex) const = 0;

private:
    QMutex                                              mMutexInitialization;
    bool                                                mInitialized;
    bool                                                mReturnNumberInitialized;
    QVector<const CT_AbstractPointAttributesScalar*>    mAttributesByType;
    const CT_PointsAttributesColor*                     mColors;

    void initType(const CT_LasDefine::LASPointAttributesType& type, const AttributeByTypeCollection& attributes);

protected:
    /**
     * @brief Derivated class must return attribute types that they need to write a point
     */
    virtual QList<CT_LasDefine::LASPointAttributesType> typesToSearch() = 0;

};

#endif // CT_ABSTRACTLAZPOINTFORMAT_H
