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

#ifndef CT_VARIANTMAP_H
#define CT_VARIANTMAP_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"
#include <QVariantMap>
/*!
 * \class CT_VariantMap
 * \brief <b>ItemDrawable representing a Kodama Buffer</b>
 *
 */
class PLUGINSHAREDSHARED_EXPORT CT_VariantMap : public CT_AbstractItemDrawableWithoutPointCloud
{
    // IMPORTANT pour avoir le nom de l'ItemDrawable
    Q_OBJECT

public:
    CT_VariantMap();

    /**
      * \brief Contructeur
      */
    CT_VariantMap(const CT_OutAbstractSingularItemModel *model, const CT_AbstractResult *result);
    CT_VariantMap(const QString &modelName, const CT_AbstractResult *result);

    /**
      * ATTENTION : ne pas oublier de redéfinir ces deux méthodes si vous héritez de cette classe.
      */
    virtual QString getType() const;
    static QString staticGetType();

    void insert(const QString key, const QVariant &value);
    const QVariant value(const QString key, const QVariant &adefaultValue = QVariant());

    virtual CT_AbstractItemDrawable* copy(const CT_OutAbstractItemModel *model, const CT_AbstractResult *result, CT_ResultCopyModeList copyModeList);
    virtual CT_AbstractItemDrawable* copy(const QString &modelName, const CT_AbstractResult *result, CT_ResultCopyModeList copyModeList);

private:
    QVariantMap     _variantMap;

};

#endif // CT_VARIANTMAP_H
