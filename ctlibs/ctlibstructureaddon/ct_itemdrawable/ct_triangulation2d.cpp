#include "ct_triangulation2d.h"

const CT_StandardTriangulation2DDrawManager CT_Triangulation2D::TRIANGULATION2D_DRAW_MANAGER;

CT_TYPE_IMPL_INIT_MACRO(CT_Triangulation2D)

CT_Triangulation2D::CT_Triangulation2D() : CT_Triangulation2D(nullptr)
{
}

CT_Triangulation2D::CT_Triangulation2D(CT_DelaunayTriangulation* delaunay) : SuperClass()
{
    _delaunay = delaunay;
    setBaseDrawManager(&TRIANGULATION2D_DRAW_MANAGER);

    if(_delaunay != nullptr)
    {
        setBoundingBox(delaunay->getMinX(), delaunay->getMinY(), 0,
                       delaunay->getMaxX(), delaunay->getMaxY(), 0);
    }
}

CT_Triangulation2D::CT_Triangulation2D(const CT_Triangulation2D& other) : SuperClass(other)
{
    _delaunay = nullptr;

    if(other._delaunay != nullptr)
        _delaunay = other._delaunay->copy();
}

CT_Triangulation2D::~CT_Triangulation2D()
{
    delete _delaunay;
}

CT_DelaunayTriangulation *CT_Triangulation2D::getDelaunayT() const
{
    return _delaunay;
}
