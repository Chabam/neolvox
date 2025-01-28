/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forets (ONF), France
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

#ifndef CT_AFFILIATIONID_H
#define CT_AFFILIATIONID_H

#include "ct_itemdrawable/abstract/ct_abstractitemdrawablewithoutpointcloud.h"

/*!
 * \class CT_AffiliationID
 * \ingroup PluginShared_Items
 * \brief <b>ItemDrawable managing affiliation ID</b>
 *
 */
class CTLIBSTRUCTUREADDON_EXPORT CT_AffiliationID : public CT_AbstractItemDrawableWithoutPointCloud
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_AffiliationID, CT_AbstractItemDrawableWithoutPointCloud, Affiliation ID)

    using SuperClass = CT_AbstractItemDrawableWithoutPointCloud;

public:
    CT_AffiliationID();
    CT_AffiliationID(size_t value);
    CT_AffiliationID(const CT_AffiliationID& other) = default;

    void setValue(size_t value);
    size_t getValue() const;
    QString getValueAsString() const;
    double getDoubleValue();

    CT_ITEM_COPY_IMP(CT_AffiliationID)

private:
    static size_t   LASTID;
    size_t          _value;

    CT_DEFAULT_IA_BEGIN(CT_AffiliationID)
    CT_DEFAULT_IA_V2(CT_AffiliationID, CT_AbstractCategory::staticInitDataId(), &CT_AffiliationID::getValue, QObject::tr("Affiliation_ID"))
    CT_DEFAULT_IA_END(CT_AffiliationID)
};

#endif // CT_AFFILIATIONID_H
