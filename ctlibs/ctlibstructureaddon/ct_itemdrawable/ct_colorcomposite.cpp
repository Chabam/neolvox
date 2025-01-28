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

#include "ct_colorcomposite.h"

const CT_StandardColorCompositeDrawManager CT_ColorComposite::COLOR_COMPOSITE_DRAW_MANAGER;

CT_TYPE_IMPL_INIT_MACRO(CT_ColorComposite)

CT_ColorComposite::CT_ColorComposite() : SuperClass(),
    _red(nullptr),
    _green(nullptr),
    _blue(nullptr),
    _zValue(nullptr),
    _redSource(nullptr),
    _greenSource(nullptr),
    _blueSource(nullptr)
{
    setBaseDrawManager(&COLOR_COMPOSITE_DRAW_MANAGER);
}

CT_ColorComposite::CT_ColorComposite(CT_AbstractImage2D* red,
                                     CT_AbstractImage2D* green,
                                     CT_AbstractImage2D* blue,
                                     CT_Image2D<float>* zvalue) : SuperClass(),
    _red(nullptr),
    _green(nullptr),
    _blue(nullptr),
    _zValue(nullptr),
    _redSource(red),
    _greenSource(green),
    _blueSource(blue)
{
    if(computeBands(red, green, blue, zvalue))
    {
        setBoundingBox(red->minX(), red->minY(), 0,
                       red->maxX(), red->maxY(), 0);
    }

    setBaseDrawManager(&COLOR_COMPOSITE_DRAW_MANAGER);
}

CT_ColorComposite::CT_ColorComposite(const CT_ColorComposite& other) : SuperClass(other)
{
    _red = static_cast<CT_Image2D<quint8>*>((other._red == nullptr) ? nullptr : other._red->copy(other._red->modelStaticT<CT_OutAbstractItemModel>(), other._red->result()));
    _green = static_cast<CT_Image2D<quint8>*>((other._green == nullptr) ? nullptr : other._green->copy(other._green->modelStaticT<CT_OutAbstractItemModel>(), other._green->result()));
    _blue = static_cast<CT_Image2D<quint8>*>((other._blue == nullptr) ? nullptr : other._blue->copy(other._blue->modelStaticT<CT_OutAbstractItemModel>(), other._blue->result()));

    _zValue = other._zValue;
    _redSource = other._redSource;
    _greenSource = other._greenSource;
    _blueSource = other._blueSource;
}

CT_ColorComposite::~CT_ColorComposite()
{
    delete _red;
    delete _green;
    delete _blue;
}

const CT_Image2D<quint8>*CT_ColorComposite::getRedBand() const
{
    return _red;
}

const CT_Image2D<quint8>*CT_ColorComposite::getGreenBand() const
{
    return _green;
}

const CT_Image2D<quint8>*CT_ColorComposite::getBlueBand() const
{
    return _blue;
}

const CT_Image2D<float>*CT_ColorComposite::getZValueRaster() const
{
    return _zValue;
}

bool CT_ColorComposite::computeBands(CT_AbstractImage2D* red, CT_AbstractImage2D* green, CT_AbstractImage2D* blue, CT_Image2D<float>* zvalue)
{
    if (red != nullptr && green != nullptr && blue != nullptr &&
            qFuzzyCompare(red->minX(), green->minX()) && qFuzzyCompare(red->minX(), green->minX()) &&
            qFuzzyCompare(red->minY(), green->minY()) && qFuzzyCompare(red->minY(), green->minY()) &&
            (red->xdim() == green->xdim()) && (red->xdim() == green->xdim()) &&
            (red->ydim() == green->ydim()) && (red->ydim() == green->ydim()) &&
            qFuzzyCompare(red->resolution(), green->resolution()) && qFuzzyCompare(red->resolution(), green->resolution()))
    {
        _zValue = zvalue;

        delete _red;
        delete _green;
        delete _blue;

        _red = new CT_Image2D<quint8>(red->minX(), red->minY(), red->xdim(), red->ydim(), red->resolution(), 0, 0, 0);
        _green = new CT_Image2D<quint8>(red->minX(), red->minY(), red->xdim(), red->ydim(), red->resolution(), 0, 0, 0);
        _blue = new CT_Image2D<quint8>(red->minX(), red->minY(), red->xdim(), red->ydim(), red->resolution(), 0, 0, 0);

        const double minRed = red->minValueAsDouble();
        const double minGreen = green->minValueAsDouble();
        const double minBlue = blue->minValueAsDouble();
        const double amplitudeRed = red->maxValueAsDouble() - minRed;
        const double amplitudeGreen = green->maxValueAsDouble() - minGreen;
        const double amplitudeBlue = blue->maxValueAsDouble() - minBlue;

        const int nCol = red->xdim();
        const int nLin = red->ydim();

        size_t index;

        for (int xx = 0 ; xx < nCol ; xx++)
        {
            for (int yy = 0 ; yy < nLin ; yy++)
            {
                if (red->index(xx, yy, index))
                {
                    const double redV = red->valueAtIndexAsDouble(index);
                    const double greenV = green->valueAtIndexAsDouble(index);
                    const double blueV = blue->valueAtIndexAsDouble(index);

                    _red->setValue  (xx, yy, uchar(255*(redV / amplitudeRed)));
                    _green->setValue(xx, yy, uchar(255*(greenV / amplitudeGreen)));
                    _blue->setValue (xx, yy, uchar(255*(blueV / amplitudeBlue)));
                }
            }
        }

        _red->setMinMax(0, 255);
        _green->setMinMax(0, 255);
        _blue->setMinMax(0, 255);

        return true;
    }

    return false;
}
