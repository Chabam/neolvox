/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michal KREBS (ARTS/ENSAM)

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

#ifndef CT_ABSTRACTAREASHAPE2D_H
#define CT_ABSTRACTAREASHAPE2D_H

#include "ct_abstractitemdrawablewithoutpointcloud.h"
#include "ct_shape2ddata/ct_areashape2ddata.h"
#include "ct_itemdrawable/abstract/ct_abstractshape2d.h"

/**
  * Représente une forme géomtrique 2D surfacique
  */
class CTLIBSTRUCTUREADDON_EXPORT CT_AbstractAreaShape2D : public CT_AbstractShape2D
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AbstractAreaShape2D, CT_AbstractShape2D, 2D area shape)

    using SuperClass = CT_AbstractShape2D;

public:
    CT_AbstractAreaShape2D();

    /**
     * @brief Construct with a data. Cannot be nullptr !
     */
    CT_AbstractAreaShape2D(CT_AreaShape2DData* data);

    /**
     * @brief Copy constructor.
     *
     *        What is copied :
     *          - Pointer of the result and model of the original item.
     *          - Unique ID
     *          - Pointer of base and alternative draw manager
     *          - Displayable name
     *          - Center coordinates
     *          - Default Color
     *          - Min and Max coordinates (bounding box)
     *          - Data 2D
     *          - Z Value and if z value is defined
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_AbstractAreaShape2D(const CT_AbstractAreaShape2D& other) = default;

    virtual bool contains(double x, double y) const;
    virtual double getArea() const;

    const CT_AreaShape2DData* getPointerAreaData() const;
    const CT_AreaShape2DData& getAreaData() const;

protected:
    CT_AreaShape2DData* getAreaDataNotConst() const;
};

#endif // CT_ABSTRACTAREASHAPE2D_H
