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

#include "ct_delaunaysidelist.h"


CT_DelaunaySideList::CT_DelaunaySideList()
{
    _removed_tri = nullptr;
    _removed_v1 = nullptr;
    _removed_v2 = nullptr;
}

CT_DelaunaySideList::~CT_DelaunaySideList()
{
    clear();
}

void CT_DelaunaySideList::clear()
{
    _tri.clear();
    _v1.clear();
    _v2.clear();

    _removed_tri = nullptr;
    _removed_v1 = nullptr;
    _removed_v2 = nullptr;
}

void CT_DelaunaySideList::appendSide(CT_DelaunayTriangle *trit, CT_DelaunayVertex *v1t, CT_DelaunayVertex *v2t)
{
    _tri.append(trit);
    _v1.append(v1t);
    _v2.append(v2t);
}

void CT_DelaunaySideList::removeSide(int i, bool keep)
{
    if (i >= _tri.size()) {return;}

    if (keep)
    {
        _removed_tri =  _tri.takeAt(i);
        _removed_v1 =  _v1.takeAt(i);
        _removed_v2 =  _v2.takeAt(i);
    } else {
        _tri.removeAt(i);
        _v1.removeAt(i);
        _v2.removeAt(i);

        _removed_tri = nullptr;
        _removed_v1 = nullptr;
        _removed_v2 = nullptr;
    }
}


CT_DelaunayVertex* CT_DelaunaySideList::swap(int i, CT_DelaunayVertex *vt) const
{
    if      (vt == _v1[i]) {return _v2[i];}
    else if (vt == _v2[i]) {return _v1[i];}
    else                {return nullptr;}
}

bool CT_DelaunaySideList::sidesEquals(int i, int j) const
{
    return(((_v1[i] == _v1[j]) && (_v2[i] == _v2[j])) || ((_v2[i] == _v1[j]) && (_v1[i] == _v2[j])));
}
