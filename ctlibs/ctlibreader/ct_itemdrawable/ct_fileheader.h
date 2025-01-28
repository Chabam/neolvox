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

#ifndef CT_FILEHEADER_H
#define CT_FILEHEADER_H

#include "ctlibreader_global.h"
#include "ct_itemdrawable/abstract/ct_abstractsingularitemdrawable.h"

#include <QFileInfo>


class CT_AbstractReader;

/**
  * @brief Represent a file header
  */
class CTLIBREADER_EXPORT CT_FileHeader : public CT_AbstractSingularItemDrawable
{
    Q_OBJECT
    CT_TYPE_IMPL_MACRO(CT_FileHeader, CT_AbstractSingularItemDrawable, File header)

    using SuperClass = CT_AbstractSingularItemDrawable;

public:
    CT_FileHeader();

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
     *          - File info
     *
     *        What is initialized differently :
     *          - Parent is set to nullptr
     *          - isSelected and isDisplayed is set to false
     *          - Document list is not copied
     */
    CT_FileHeader(const CT_FileHeader& other);

    ~CT_FileHeader() override;

    /**
     * @brief Set the filepath
     */
    void setFilePath(const QString& filepath);

    /**
     * @brief Set the reader
     */
    void setReader(CT_AbstractReader* reader, bool mustAutoDeleteReader);


    /**
     * @brief Returns the file info of the current file path
     */
    QFileInfo fileInfo() const;

    /**
     * @brief Returns the current file name
     */
    QString fileName() const;

    /**
     * @brief Returns the current file path
     */
    QString filePath() const;

    /**
     * @brief Returns the directory path where the file is
     */
    QString directoryPath() const;

    /**
     * @brief Returns the reader
     */
    CT_AbstractReader* reader();

    /**
     * @brief Returns true if we must auto delete the reader
     */
    bool mustAutoDeleteReader() const;

    /**
     * @brief Returns the displayable name used by the reader
     */
    QString readerDisplayableName() const;

    CT_ITEM_COPY_IMP(CT_FileHeader)

protected:
    QFileInfo     m_fileInfo;

    CT_AbstractReader*  m_reader;
    bool                m_mustAutoDeleteReader;

private:
    CT_DEFAULT_IA_BEGIN(CT_FileHeader)
    CT_DEFAULT_IA_V2(CT_FileHeader, CT_AbstractCategory::staticInitDataDisplayableName(), &CT_FileHeader::fileName, QObject::tr("FileName"))
    CT_DEFAULT_IA_V2(CT_FileHeader, CT_AbstractCategory::staticInitDataFileName(), &CT_FileHeader::filePath, QObject::tr("FilePath"))
    CT_DEFAULT_IA_V2(CT_FileHeader, CT_AbstractCategory::staticInitDataDisplayableName(), &CT_FileHeader::directoryPath, QObject::tr("Directory"))
    CT_DEFAULT_IA_V2(CT_FileHeader, CT_AbstractCategory::staticInitDataDisplayableName(), &CT_FileHeader::readerDisplayableName, QObject::tr("ReaderType"))
    CT_DEFAULT_IA_END(CT_FileHeader)
};

#endif // CT_FILEHEADER_H
