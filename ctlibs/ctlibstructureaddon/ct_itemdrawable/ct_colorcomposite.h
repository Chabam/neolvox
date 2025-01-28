/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
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

#ifndef CT_COLORCOMPOSITE_H
#define CT_COLORCOMPOSITE_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"
#include "ct_itemdrawable/tools/drawmanager/ct_standardcolorcompositedrawmanager.h"
#include "ct_itemdrawable/ct_image2d.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_ColorComposite : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_ColorComposite, CT_AbstractItemDrawableWithoutPointCloud, Color Composite)

    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:

    CT_ColorComposite();
    CT_ColorComposite(CT_AbstractImage2D* red,
                      CT_AbstractImage2D* green,
                      CT_AbstractImage2D* blue,
                      CT_Image2D<float>* zvalue);
    CT_ColorComposite(const CT_ColorComposite& other);
    ~CT_ColorComposite() override;

    const CT_Image2D<quint8>* getRedBand() const;
    const CT_Image2D<quint8>* getGreenBand() const;
    const CT_Image2D<quint8>* getBlueBand() const;
    const CT_Image2D<float>* getZValueRaster() const;

    CT_ITEM_COPY_IMP(CT_ColorComposite)

private:
    static const CT_StandardColorCompositeDrawManager COLOR_COMPOSITE_DRAW_MANAGER;

    CT_Image2D<quint8>*  _red;
    CT_Image2D<quint8>*  _green;
    CT_Image2D<quint8>*  _blue;
    CT_Image2D<float>*   _zValue;

    CT_AbstractImage2D* _redSource;
    CT_AbstractImage2D* _greenSource;
    CT_AbstractImage2D* _blueSource;

    bool computeBands(CT_AbstractImage2D* red,
                      CT_AbstractImage2D* green,
                      CT_AbstractImage2D* blue,
                      CT_Image2D<float>* zvalue);
};

#endif // CT_COLORCOMPOSITE_H
