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

#ifndef CT_PLOTGRIDMANAGER_H
#define CT_PLOTGRIDMANAGER_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"

class CTLIBSTRUCTUREADDON_EXPORT CT_PlotGridManager : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_PlotGridManager, CT_AbstractItemDrawableWithoutPointCloud, Plot grid manager)
    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:
    CT_PlotGridManager();
    CT_PlotGridManager(const CT_PlotGridManager& other) = default;

    bool hasBoundingBox() const override { return false; }

    CT_ITEM_COPY_IMP(CT_PlotGridManager)

private:
    CT_DEFAULT_IA_BEGIN(CT_PlotGridManager)
//    CT_DEFAULT_IA_V2(CT_PlotGridManager, CT_AbstractCategory::staticInitDataNumber(), &CT_PlotGridManager::getPlotNumber, QObject::tr("Nombre de placettes"))
    CT_DEFAULT_IA_END(CT_PlotGridManager)

};

#endif // CT_PLOTGRIDMANAGER_H
