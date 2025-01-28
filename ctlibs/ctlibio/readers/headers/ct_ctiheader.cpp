#include "ct_ctiheader.h"

#include <QDir>

CT_DEFAULT_IA_INIT(CT_CTIHeader)

CT_CTIHeader::CT_CTIHeader() : SuperClass()
{
    _bit64 = false;
    _xmin = 0;
    _ymin = 0;
    _xmax = 0;
    _ymax = 0;
}

void CT_CTIHeader::boundingBox(Eigen::Vector3d &min, Eigen::Vector3d &max) const
{
    min(0) = _xmin;
    min(1) = _ymin;
    min(2) = 0;
    max(0) = _xmax;
    max(1) = _ymax;
    max(2) = 0;
}

void CT_CTIHeader::setFileFormat(const QString &fileFormat)
{
    _fileFormat = fileFormat;
}

void CT_CTIHeader::setBit64(bool bit64)
{
    _bit64 = bit64;
}

void CT_CTIHeader::setExtent(double xmin, double ymin, double xmax, double ymax)
{
    _xmin = xmin;
    _ymin = ymin;
    _xmax = xmax;
    _ymax = ymax;
}

void CT_CTIHeader::setBox2DArea(double xmin, double ymin, double xmax, double ymax)
{
    _areaShapeType = "BOX";

    Eigen::Vector2d min, max;
    min(0) = xmin;
    min(1) = ymin;
    max(0) = xmax;
    max(1) = ymax;

    _box2d = CT_Box2DData(min, max);
}

void CT_CTIHeader::setCircle2DArea(double x, double y, double radius)
{
    _areaShapeType = "CIRCLE";

    Eigen::Vector2d center;
    center(0) = x;
    center(1) = y;

    _circle2d = CT_Circle2DData(center, radius);
}

void CT_CTIHeader::setPolygon2DArea(const QVector<Eigen::Vector2d> &vertices)
{
    _areaShapeType = "POLYGON";

    _polygon2d = CT_Polygon2DData(vertices);
}


void CT_CTIHeader::setBox2DAreaSmall(double xmin, double ymin, double xmax, double ymax)
{
    _areaSmallShapeType = "BOX";

    Eigen::Vector2d min, max;
    min(0) = xmin;
    min(1) = ymin;
    max(0) = xmax;
    max(1) = ymax;

    _box2dSmall = CT_Box2DData(min, max);
}

void CT_CTIHeader::setCircle2DAreaSmall(double x, double y, double radius)
{
    _areaSmallShapeType = "CIRCLE";

    Eigen::Vector2d center;
    center(0) = x;
    center(1) = y;

    _circle2dSmall = CT_Circle2DData(center, radius);
}

void CT_CTIHeader::setPolygon2DAreaSmall(const QVector<Eigen::Vector2d> &vertices)
{
    _areaSmallShapeType = "POLYGON";

    _polygon2dSmall = CT_Polygon2DData(vertices);
}


void CT_CTIHeader::setFirstFileIndexPos(qint64 pos)
{
    _firstFileIndexPos = pos;
}

void CT_CTIHeader::setBasePath(QString path)
{
    QDir dir(path);

    _basePath = dir.path();
    if (!dir.exists())
    {
        _basePath = "..";
    }
}

QString CT_CTIHeader::fileFormat() const
{
    return _fileFormat;
}

bool CT_CTIHeader::bit64() const
{
    return _bit64;
}

double CT_CTIHeader::xmin() const
{
    return _xmin;
}

double CT_CTIHeader::ymin() const
{
    return _ymin;
}

double CT_CTIHeader::xmax() const
{
    return _xmax;
}

double CT_CTIHeader::ymax() const
{
    return _ymax;
}

QString CT_CTIHeader::areaShapeType() const
{
    return _areaShapeType;
}

QString CT_CTIHeader::areaSmallShapeType() const
{
    return _areaSmallShapeType;
}

qint64 CT_CTIHeader::firstFileIndexPos() const
{
    return _firstFileIndexPos;
}

QString CT_CTIHeader::basePath() const
{
    return _basePath;
}
