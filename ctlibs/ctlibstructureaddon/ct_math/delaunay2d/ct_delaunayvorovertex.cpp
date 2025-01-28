/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
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

#include "ct_delaunayvorovertex.h"

CT_DelaunayVoroVertex::CT_DelaunayVoroVertex(const CT_DelaunayTriangle *trit, CT_DelaunayVertex *v1t, CT_DelaunayVertex *v2t)
{
    _x = trit->_ccX;
    _y = trit->_ccY;
    _v1 = v1t;
    _v2 = v2t;
}

CT_DelaunayVoroVertex::~CT_DelaunayVoroVertex()
{
    _v1 = nullptr;
    _v2 = nullptr;
}

CT_DelaunayVertex* CT_DelaunayVoroVertex::next(const CT_DelaunayVertex* vt)
{
    if      (vt == _v1) {return _v2;}
    else if (vt == _v2) {return _v1;}
    else                {return nullptr;}
}
