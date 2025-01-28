#include "ct_face.h"
#include "ct_mesh/ct_edge.h"

CT_Edge* CT_Face::edge() const { return &PS_REPOSITORY->globalEdgeCloud()->tAt(m_edge); }

size_t CT_Face::iPointAt(const int &j) const
{
    if(j==2) return edge()->next()->iPointAt(1); else return edge()->iPointAt(j);
}

void CT_Face::setEdge(CT_Edge *edge) { setEdge(PS_REPOSITORY->globalEdgeCloud()->indexOf(edge)); }
