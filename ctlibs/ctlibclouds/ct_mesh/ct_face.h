#ifndef CT_FACE_H
#define CT_FACE_H

#include "ct_point.h"

class CT_Edge;

/**
 * A Face for a mesh. You can create Face with the help of the class CT_MeshAllocatorT.
 */
class CTLIBCLOUDS_EXPORT CT_Face
{
public:
    inline CT_Face() { m_edge = 0; }

    CT_Edge* edge() const;
    size_t iPointAt(const int &j) const;
    size_t iEdge() const { return m_edge; }

    void setEdge(CT_Edge *edge);
    inline void setEdge(size_t edgeGlobalIndex) { m_edge = edgeGlobalIndex; }

private:
    size_t  m_edge;   // edge
};

#endif // CT_FACE_H
