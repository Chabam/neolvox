/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
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

#include "ct_variantmap.h"


CT_VariantMap::CT_VariantMap() : CT_AbstractItemDrawableWithoutPointCloud()
{
}

CT_VariantMap::CT_VariantMap(const CT_OutAbstractSingularItemModel *model, const CT_AbstractResult *result) : CT_AbstractItemDrawableWithoutPointCloud(model, result)
{
}


CT_VariantMap::CT_VariantMap(const QString &modelName, const CT_AbstractResult *result) : CT_AbstractItemDrawableWithoutPointCloud(modelName, result)
{
}

QString CT_VariantMap::getType() const
{
    return staticGetType();
}

QString CT_VariantMap::staticGetType()
{
    return CT_AbstractItemDrawableWithoutPointCloud::staticGetType() + "/CT_VariantMap";
}

void CT_VariantMap::insert(const QString key, const QVariant &value)
{
    _variantMap.insert(key, value);

}

const QVariant CT_VariantMap::value(const QString key, const QVariant &adefaultValue)
{
    return _variantMap.value(key, adefaultValue);
}

CT_AbstractItemDrawable* CT_VariantMap::copy(const CT_OutAbstractItemModel *model, const CT_AbstractResult *result, CT_ResultCopyModeList copyModeList)
{
    Q_UNUSED(copyModeList);
    CT_VariantMap *map = new CT_VariantMap((const CT_OutAbstractSingularItemModel *)model, result);

    map->setAlternativeDrawManager(getAlternativeDrawManager());

    QMapIterator<QString, QVariant> it(_variantMap);
    while (it.hasNext())
    {
        it.next();
        map->insert(it.key(), it.value());
    }

    return map;
}

CT_AbstractItemDrawable *CT_VariantMap::copy(const QString &modelName, const CT_AbstractResult *result, CT_ResultCopyModeList copyModeList)
{
    Q_UNUSED(copyModeList);
    CT_VariantMap *map = new CT_VariantMap(modelName, result);

    map->setAlternativeDrawManager(getAlternativeDrawManager());

    QMapIterator<QString, QVariant> it(_variantMap);
    while (it.hasNext())
    {
        it.next();
        map->insert(it.key(), it.value());
    }

    return map;
}
