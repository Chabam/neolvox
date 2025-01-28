/****************************************************************************

 Copyright (C) 2010-2015 the Office National des Forêts (ONF), France
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Alexandre Piboule (ONF)

 This file is part of PluginShared library 2.0.

 PluginShared is free library: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 PluginShared is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with PluginShared.  If not, see <http://www.gnu.org/licenses/lgpl.html>.

*****************************************************************************/

#ifndef CT_DELAUNAYSIDELIST_H
#define CT_DELAUNAYSIDELIST_H

#include "ct_delaunayvertex.h"
#include "ct_delaunaytriangle.h"


/**
 * DelaunaySide - side of the Delaunay triangulation.
 *
 * @author A. Piboule - february 2004 / modified october 2016
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_DelaunaySideList
{

public:
    CT_DelaunaySideList ();

    ~CT_DelaunaySideList();

    inline int size() const {return _tri.size();}

    inline CT_DelaunayTriangle* getTri(int i) const {return _tri[i];}
    inline CT_DelaunayVertex*   getV1(int i)  const {return _v1[i];}
    inline CT_DelaunayVertex*   getV2(int i)  const {return _v2[i];}

    inline CT_DelaunayTriangle* getRemovedTri() const {return _removed_tri;}
    inline CT_DelaunayVertex*   getRemovedV1()  const {return _removed_v1;}
    inline CT_DelaunayVertex*   getRemovedV2()  const {return _removed_v2;}

    void clear();

    // give the other vertex of the side
    CT_DelaunayVertex *swap(int i, CT_DelaunayVertex *vt) const;

    bool sidesEquals (int i, int j) const;

    void appendSide(CT_DelaunayTriangle *trit, CT_DelaunayVertex *v1t, CT_DelaunayVertex *v2t);
    void removeSide(int i, bool keep = false);



private:
    friend class CT_DelaunayTriangulation;

    QList<CT_DelaunayVertex*>     _v1;
    QList<CT_DelaunayVertex*>     _v2;
    QList<CT_DelaunayTriangle*>   _tri;

    CT_DelaunayVertex*      _removed_v1;
    CT_DelaunayVertex*      _removed_v2;
    CT_DelaunayTriangle*    _removed_tri;

};



#endif // CT_DELAUNAYSIDELIST_H
