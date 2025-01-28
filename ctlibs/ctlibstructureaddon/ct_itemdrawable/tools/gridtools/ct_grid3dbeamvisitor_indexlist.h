#ifndef CT_GRID3DBEAMVISITOR_INDEXLIST_H
#define CT_GRID3DBEAMVISITOR_INDEXLIST_H

#include "ct_itemdrawable/tools/gridtools/ct_abstractgrid3dbeamvisitor.h"

/*!
 * \brief Simple visitor used to obtain an ordered indices list of traversed voxels
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_Grid3DBeamVisitor_IndexList : public CT_AbstractGrid3DBeamVisitor
{
public:
    CT_Grid3DBeamVisitor_IndexList();

    virtual void visit(const size_t &index, const CT_Beam *beam);

    inline QList<size_t> getIndices() {return _indices;}

private:
    QList<size_t>  _indices;

};

#endif // CT_GRID3DBEAMVISITOR_INDEXLIST_H
