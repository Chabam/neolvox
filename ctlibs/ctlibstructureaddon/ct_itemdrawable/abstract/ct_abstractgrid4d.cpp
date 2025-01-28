#include "ct_abstractgrid4d.h"

CT_TYPE_IMPL_INIT_MACRO(CT_AbstractGrid4D)

CT_AbstractGrid4D::CT_AbstractGrid4D() : SuperClass()
{
    _bot(0) =0;
    _bot(1) =0;
    _bot(2) =0;
    _bot(3) =0;

    _top(0) =0;
    _top(1) =0;
    _top(2) =0;
    _top(3) =0;

    _resw = 0;
    _resx = 0;
    _resy = 0;
    _resz = 0;
    _dimw = 1;
    _dimx = 1;
    _dimy = 1;
    _dimz = 1;
}

CT_AbstractGrid4D::CT_AbstractGrid4D(double wmin, double xmin, double ymin, double zmin, int dimw, int dimx, int dimy, int dimz, double resw, double resx, double resy, double resz)
{
    _bot(0) = wmin;
    _bot(1) = xmin;
    _bot(2) = ymin;
    _bot(3) = zmin;

    _top(0) = wmin + resw * dimw;
    _top(1) = xmin + resx * dimx;
    _top(2) = ymin + resy * dimy;
    _top(3) = zmin + resz * dimz;

    _dimw = dimw;
    _dimx = dimx;
    _dimy = dimy;
    _dimz = dimz;

    _resw = resw;
    _resx = resx;
    _resy = resy;
    _resz = resz;

    setBoundingBox(xmin, ymin, zmin,
                   xmin + resx * dimx,
                   ymin + resy * dimy,
                   zmin + resz * dimz);
}

CT_AbstractGrid4D::CT_AbstractGrid4D(double wmin, double xmin, double ymin, double zmin, double wmax, double xmax, double ymax, double zmax, double resw, double resx, double resy, double resz)
{
    _bot(0) = ( wmin );
    _bot(1) = ( xmin );
    _bot(2) = ( ymin );
    _bot(3) = ( zmin );

    _resw = resw;
    _resx = resx;
    _resy = resy;
    _resz = resz;

    _dimw = int(ceil((wmax - wmin)/_resw));
    _dimx = int(ceil((xmax - xmin)/_resx));
    _dimy = int(ceil((ymax - ymin)/_resy));
    _dimz = int(ceil((zmax - zmin)/_resz));

    Eigen::Vector3d maxCoordinates(xmin + resx * _dimx,
                                   ymin + resy * _dimy,
                                   zmin + resz * _dimz);

    _top(0) = ( wmin + resw * _dimw );
    _top(1) = ( xmin + resx * _dimx );
    _top(2) = ( ymin + resy * _dimy );
    _top(3) = ( zmin + resz * _dimz );

    // to ensure a point exactly on a maximum limit of the grid will be included in the grid
    while (wmax >= maxW())
    {
        _dimw++;
        _top(0) = ( _top(0) + _resw);
    }

    while (xmax >= maxX())
    {
        _dimx++;
        maxCoordinates(0) = (maxCoordinates(0) + _resx);
        _top(1) = ( _top(1) + _resx);
    }

    while (ymax >= maxY())
    {
        _dimy++;
        maxCoordinates(1) = (maxCoordinates(1) + _resy);
        _top(2) = ( _top(2) + _resy);
    }

    while (zmax >= maxZ())
    {
        _dimz++;
        maxCoordinates(2) = (maxCoordinates(2) + _resz);
        _top(3) = ( _top(3) + _resz);
    }

    setBoundingBox(xmin, ymin, zmin,
                   maxCoordinates(0), maxCoordinates(1), maxCoordinates(2));
}
