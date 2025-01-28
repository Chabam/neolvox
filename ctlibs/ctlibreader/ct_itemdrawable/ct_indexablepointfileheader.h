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

#ifndef CT_INDEXABLEPOINTFILEHEADER_H
#define CT_INDEXABLEPOINTFILEHEADER_H

#include "ctlibreader_global.h"
#include "ct_itemdrawable/ct_fileheader.h"
#include "ct_reader/extensions/ct_indexablepointsreader.h"

/**
  * @brief Represent a indexable point file header
  */
class CTLIBREADER_EXPORT CT_IndexablePointFileHeader : public CT_FileHeader
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_IndexablePointFileHeader, CT_FileHeader, Indexable Point File header)

    using SuperClass = CT_FileHeader;

public:
    CT_IndexablePointFileHeader();

    CT_IndexablePointFileHeader(const CT_IndexablePointFileHeader& other);

    ~CT_IndexablePointFileHeader() override;

    CT_IndexablePointsReader *indexablePointReader();

    virtual size_t nPoints() const {return 0;}

    CT_ITEM_COPY_IMP(CT_IndexablePointFileHeader)

protected:


private:


//    CT_DEFAULT_IA_BEGIN(CT_IndexablePointFileHeader)
//    CT_DEFAULT_IA_V2(CT_IndexablePointFileHeader, CT_AbstractCategory::staticInitDataDisplayableName(), &CT_IndexablePointFileHeader::readerDisplayableName, QObject::tr("ReaderType"))
//    CT_DEFAULT_IA_END(CT_IndexablePointFileHeader)
};

#endif // CT_INDEXABLEPOINTFILEHEADER_H
