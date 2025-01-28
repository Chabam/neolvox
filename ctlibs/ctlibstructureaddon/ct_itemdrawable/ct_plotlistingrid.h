/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Suprieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Alexandre PIBOULE (ONF)

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

#ifndef CT_PLOTLISTINGRID_H
#define CT_PLOTLISTINGRID_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"
#include "ct_shape2ddata/ct_areashape2ddata.h"

#include <QHash>

class CTLIBSTRUCTUREADDON_EXPORT CT_PlotListInGrid : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_PlotListInGrid, CT_AbstractItemDrawableWithoutPointCloud, Plot grid list)
    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:

    enum Type {
        T_Circle,
        T_Square
    };


    CT_PlotListInGrid();
    CT_PlotListInGrid(const CT_AreaShape2DData* areaShape2D,
                      const Eigen::Vector2d& refCoords,
                      double spacing,
                      double size);
    CT_PlotListInGrid(const CT_PlotListInGrid& other);
    ~CT_PlotListInGrid() override;

    bool hasBoundingBox() const override {return true;}

    void getBoundingBox2D(Eigen::Vector2d &min, Eigen::Vector2d &max) const;

    inline double getSpacing() const {return _spacing;}
    inline double getSize() const {return _size;}

    void setIndices(size_t firstIndex, size_t indexJumpAtEOL);

    QHash<CT_AreaShape2DData *, size_t> createPlots(CT_PlotListInGrid::Type type);

    static bool orderTopLeftToBottomRight(CT_PlotListInGrid *s1, CT_PlotListInGrid *s2)
    {
        if (s1->_min(1) >= s2->_max(1)) {return true;}
        if (s1->_min(0) <  s2->_min(0)) {return true;}
        return false;
    }

    CT_ITEM_COPY_IMP(CT_PlotListInGrid)

protected:

    CT_AreaShape2DData*     _areaXY;
    Eigen::Vector2d         _min;
    Eigen::Vector2d         _max;
    double                  _spacing;
    double                  _size;
    size_t                  _firstIndex;
    size_t                  _indexJumpAtEOL;


private:
    CT_DEFAULT_IA_BEGIN(CT_PlotListInGrid)
//    CT_DEFAULT_IA_V2(CT_PlotListInGrid, CT_AbstractCategory::staticInitDataNumber(), &CT_PlotListInGrid::getPlotNumber, QObject::tr("Nombre de placettes"))
    CT_DEFAULT_IA_END(CT_PlotListInGrid)

    void computeMinMax(const Eigen::Vector2d &refCoords);
};

#endif // CT_PLOTLISTINGRID_H
