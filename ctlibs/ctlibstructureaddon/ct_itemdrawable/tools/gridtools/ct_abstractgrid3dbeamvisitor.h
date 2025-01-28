#ifndef CT_ABSTRACTGRID3DBEAMVISITOR_H
#define CT_ABSTRACTGRID3DBEAMVISITOR_H

#include "ct_itemdrawable/ct_beam.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractGrid3DBeamVisitor
{
public:   
    CT_AbstractGrid3DBeamVisitor();

    virtual void visit(const size_t &index, const CT_Beam *beam) = 0;

};

#endif // CT_ABSTRACTGRID3DBEAMVISITOR_H
