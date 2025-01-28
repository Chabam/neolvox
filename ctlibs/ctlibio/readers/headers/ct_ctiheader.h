#ifndef CT_CTIHEADER_H
#define CT_CTIHEADER_H

#include "ctlibio/ctlibio_global.h"
#include "ct_itemdrawable/ct_fileheader.h"

#include "ct_shape2ddata/ct_box2ddata.h"
#include "ct_shape2ddata/ct_circle2ddata.h"
#include "ct_shape2ddata/ct_polygon2ddata.h"

#include <QString>


class CTLIBIO_EXPORT CT_CTIHeader : public CT_FileHeader
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_CTIHeader, CT_FileHeader, CTI Header)

    using SuperClass = CT_FileHeader;

public:
    CT_CTIHeader();
    CT_CTIHeader(const CT_CTIHeader& other) = default;

    bool hasBoundingBox() const final {return true;}
    void boundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const final;

    void setFileFormat(const QString& fileFormat);
    void setBit64(bool bit64);
    void setExtent(double xmin, double ymin, double xmax, double ymax);

    void setBox2DArea(double xmin, double ymin, double xmax, double ymax);
    void setCircle2DArea(double x, double y, double radius);
    void setPolygon2DArea(const QVector<Eigen::Vector2d> &vertices);

    void setBox2DAreaSmall(double xmin, double ymin, double xmax, double ymax);
    void setCircle2DAreaSmall(double x, double y, double radius);
    void setPolygon2DAreaSmall(const QVector<Eigen::Vector2d> &vertices);

    void setFirstFileIndexPos(qint64 pos);

    void setBasePath(QString path);

    QString fileFormat() const;
    bool bit64() const;
    double xmin() const;
    double ymin() const;
    double xmax() const;
    double ymax() const;
    QString areaShapeType() const;
    QString areaSmallShapeType() const;
    qint64 firstFileIndexPos() const;

    const CT_Box2DData& box2d() const {return _box2d;}
    const CT_Circle2DData& circle2d() const {return _circle2d;}
    const CT_Polygon2DData& polygon2d() const {return _polygon2d;}

    const CT_Box2DData& box2dSmall() const {return _box2dSmall;}
    const CT_Circle2DData& circle2dSmall() const {return _circle2dSmall;}
    const CT_Polygon2DData& polygon2dSmall() const {return _polygon2dSmall;}


    QString basePath() const;


    CT_ITEM_COPY_IMP(CT_CTIHeader)

private:

    QString             _fileFormat;
    bool                _bit64;
    double              _xmin;
    double              _ymin;
    double              _xmax;
    double              _ymax;
    qint64              _firstFileIndexPos;

    QString             _areaShapeType;
    CT_Box2DData        _box2d;
    CT_Circle2DData     _circle2d;
    CT_Polygon2DData    _polygon2d;

    QString             _areaSmallShapeType;
    CT_Box2DData        _box2dSmall;
    CT_Circle2DData     _circle2dSmall;
    CT_Polygon2DData    _polygon2dSmall;


    QString             _basePath;


    CT_DEFAULT_IA_BEGIN(CT_CTIHeader)
    CT_DEFAULT_IA_V2(CT_CTIHeader, CT_AbstractCategory::staticInitDataValue(), &CT_CTIHeader::fileFormat, QObject::tr("fileFormat"))
    CT_DEFAULT_IA_V2(CT_CTIHeader, CT_AbstractCategory::staticInitDataValue(), &CT_CTIHeader::areaShapeType, QObject::tr("areaShapeType"))
    CT_DEFAULT_IA_V2(CT_CTIHeader, CT_AbstractCategory::staticInitDataValue(), &CT_CTIHeader::areaSmallShapeType, QObject::tr("areaSmallShapeType"))
    CT_DEFAULT_IA_V2(CT_CTIHeader, CT_AbstractCategory::staticInitDataX(), &CT_CTIHeader::xmin, QObject::tr("xmin"))
    CT_DEFAULT_IA_V2(CT_CTIHeader, CT_AbstractCategory::staticInitDataY(), &CT_CTIHeader::ymin, QObject::tr("ymin"))
    CT_DEFAULT_IA_V2(CT_CTIHeader, CT_AbstractCategory::staticInitDataX(), &CT_CTIHeader::xmax, QObject::tr("xmax"))
    CT_DEFAULT_IA_V2(CT_CTIHeader, CT_AbstractCategory::staticInitDataY(), &CT_CTIHeader::ymax, QObject::tr("ymax"))
    CT_DEFAULT_IA_V2(CT_CTIHeader, CT_AbstractCategory::staticInitDataValue(), &CT_CTIHeader::basePath, QObject::tr("basePath"))
    CT_DEFAULT_IA_END(CT_CTIHeader)
};

#endif // CT_CTIHEADER_H
