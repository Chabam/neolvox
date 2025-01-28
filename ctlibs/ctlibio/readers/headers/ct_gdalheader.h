#ifndef CT_GDALHEADER_H
#define CT_GDALHEADER_H

#include "ctlibio/ctlibio_global.h"
#include "ct_itemdrawable/ct_fileheader.h"

class CTLIBIO_EXPORT CT_GDALHeader : public CT_FileHeader
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_GDALHeader, CT_FileHeader, GDAL Header)

    using SuperClass = CT_FileHeader;

public:
    CT_GDALHeader();
    CT_GDALHeader(const CT_GDALHeader& other) = default;

    virtual bool hasBoundingBox() const override {return true;}
    void setBoundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max);
    virtual void boundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const override;

    double minX() const {return m_minCoordinates(0);}
    double minY() const {return m_minCoordinates(1);}
    double maxX() const {return m_maxCoordinates(0);}
    double maxY() const {return m_maxCoordinates(1);}

    Eigen::Vector3d     m_minCoordinates;
    Eigen::Vector3d     m_maxCoordinates;

    CT_ITEM_COPY_IMP(CT_GDALHeader)

    CT_DEFAULT_IA_BEGIN(CT_AbstractGeometricalItem)
    CT_DEFAULT_IA_V2(CT_GDALHeader, CT_AbstractCategory::staticInitDataX(), &CT_GDALHeader::minX, QObject::tr("Min X"))
    CT_DEFAULT_IA_V2(CT_GDALHeader, CT_AbstractCategory::staticInitDataX(), &CT_GDALHeader::maxX, QObject::tr("Max X"))
    CT_DEFAULT_IA_V2(CT_GDALHeader, CT_AbstractCategory::staticInitDataY(), &CT_GDALHeader::minY, QObject::tr("Min Y"))
    CT_DEFAULT_IA_V2(CT_GDALHeader, CT_AbstractCategory::staticInitDataY(), &CT_GDALHeader::maxY, QObject::tr("Max Y"))
    CT_DEFAULT_IA_END(CT_AbstractGeometricalItem)


};

#endif // CT_GDALHEADER_H
