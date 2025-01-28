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

#include "ct_fileheader.h"
#include "ct_reader/abstract/ct_abstractreader.h"


CT_DEFAULT_IA_INIT(CT_FileHeader)

CT_FileHeader::CT_FileHeader() : SuperClass(),
    m_reader(nullptr),
    m_mustAutoDeleteReader(false)
{

}

CT_FileHeader::CT_FileHeader(const CT_FileHeader &other) : SuperClass(other),
    m_reader((other.m_reader == nullptr) ? nullptr : other.m_reader),
    m_mustAutoDeleteReader(false)
{

}

CT_FileHeader::~CT_FileHeader()
{
    if(m_mustAutoDeleteReader)
        delete m_reader;
}

void CT_FileHeader::setFilePath(const QString& filepath)
{
    m_fileInfo = QFileInfo(filepath);
}

void CT_FileHeader::setReader(CT_AbstractReader *reader, bool mustAutoDeleteReader)
{
    m_reader = reader;
    m_mustAutoDeleteReader = mustAutoDeleteReader;
}

QFileInfo CT_FileHeader::fileInfo() const
{
    return m_fileInfo;
}

QString CT_FileHeader::fileName() const
{
    return m_fileInfo.fileName();
}

QString CT_FileHeader::filePath() const
{
    return m_fileInfo.filePath();
}

QString CT_FileHeader::directoryPath() const
{
    return m_fileInfo.path();
}

CT_AbstractReader* CT_FileHeader::reader()
{
    return m_reader;
}

bool CT_FileHeader::mustAutoDeleteReader() const
{
    return m_mustAutoDeleteReader;
}

QString CT_FileHeader::readerDisplayableName() const
{
    if (m_reader == nullptr) {return "";}
    return m_reader->displayableName();
}


