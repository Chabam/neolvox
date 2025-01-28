#ifndef CT_ABSTRACTLASPOINTFORMAT_H
#define CT_ABSTRACTLASPOINTFORMAT_H

#include "ctliblas/tools/las/ct_laspointinfo.h"
#include "ctliblas/readers/headers/ct_lasheader.h"
#include "ct_itemdrawable/abstract/ct_abstractpointattributesscalar.h"
#include "ct_itemdrawable/ct_pointsattributescolor.h"

#define CT_WRITE_LAS_SCALAR(LAS_DEFINE_TYPE, CAST_TO) \
    { \
        const CT_AbstractPointAttributesScalar* sc; \
        \
        if(((sc = scalars(LAS_DEFINE_TYPE)) != nullptr) && sc->hasBeenSet(globalIndex)) \
            stream << static_cast<CAST_TO>(sc->scalarAsDoubleAt(globalIndex)); \
        else \
            stream << static_cast<CAST_TO>(0); \
    }

#define CT_WRITE_LAS_SCALAR_WITH(ATTRIBUTES, CAST_TO) \
    { \
        if((ATTRIBUTES != nullptr) && ATTRIBUTES->hasBeenSet(globalIndex)) \
            stream << static_cast<CAST_TO>(ATTRIBUTES->scalarAsDoubleAt(globalIndex)); \
        else \
            stream << static_cast<CAST_TO>(0); \
    }

/**
 * @brief A tool class that can be use to write a point in a LAS File
 */
class CTLIBLAS_EXPORT CT_AbstractLASPointFormat
{
public:
    // a type = an attribute (if there is no value for a type in the collection it means that no attribute has been found for this type)
    using AttributeByTypeCollection = QMultiHash<CT_LasDefine::LASPointAttributesType, const CT_AbstractPointAttributesScalar*>;

    CT_AbstractLASPointFormat();
    virtual ~CT_AbstractLASPointFormat();

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
    virtual void write(QDataStream &stream, CT_LASHeader* header, const CT_Point &p, const size_t &globalIndex) const = 0;

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

#endif // CT_ABSTRACTLASPOINTFORMAT_H
