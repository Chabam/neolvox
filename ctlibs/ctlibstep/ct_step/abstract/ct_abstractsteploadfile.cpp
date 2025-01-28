/****************************************************************************

 Copyright (C) 2010-2012 the Office National des Forêts (ONF), France
                     and the Association de Recherche Technologie et Sciences (ARTS), Ecole Nationale Supérieure d'Arts et Métiers (ENSAM), Cluny, France.
                     All rights reserved.

 Contact : alexandre.piboule@onf.fr

 Developers : Michaël KREBS (ARTS/ENSAM)

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

#include "ct_abstractsteploadfile.h"

#include <QFileDialog>
#include <QDir>
#define _USE_MATH_DEFINES
#include <math.h>

CT_AbstractStepLoadFile::CT_AbstractStepLoadFile() : SuperClass()
{
    m_filePath = "";
    m_defaultDirPath = ".";
}

void CT_AbstractStepLoadFile::savePreSettings(SettingsWriterInterface &writer) const
{
    SuperClass::savePreSettings(writer);

    writer.addParameterPath(this, "FilePath", m_filePath);
}

bool CT_AbstractStepLoadFile::restorePreSettings(SettingsReaderInterface &reader)
{
    if(!SuperClass::restorePreSettings(reader))
        return false;

    reader.parameterPath(this, "FilePath", m_filePath);

    return true;
}

bool CT_AbstractStepLoadFile::acceptFile(const QString& filePath, bool* allAccepted) const
{
    const QList<FileFormat> extList = fileExtensionAccepted();

    const QFileInfo info(filePath);
    const QString suffixToTest = info.suffix().toLower();

    bool allAcceptedTmp = false;

    if (allAccepted != nullptr)
        *allAccepted = false;

    for(const FileFormat& ff : extList)
    {
        for(const QString& ext : ff.suffixes())
        {
            if (ext == "*")
                allAcceptedTmp = true;

            if(suffixToTest == ext.toLower())
                return true;
        }
    }

    if (allAccepted != nullptr)
        *allAccepted = allAcceptedTmp;

    return allAcceptedTmp;
}

bool CT_AbstractStepLoadFile::setFilePath(const QString& filePath)
{
    if(m_filePath != filePath) {

        if(!acceptFile(filePath))
            return false;

        m_filePath = filePath;
        setSettingsModified(true);
    }

    return true;
}

QString CT_AbstractStepLoadFile::filePath() const
{
    return m_filePath;
}

QString CT_AbstractStepLoadFile::defaultDirPath() const
{
    return m_defaultDirPath;
}

bool CT_AbstractStepLoadFile::postInputConfigure()
{
    if(!SuperClass::postInputConfigure())
        return false;

    const QString currentPath = m_filePath.isEmpty() ? m_defaultDirPath : m_filePath;

    const QString finalFilePath = QFileDialog::getOpenFileName(nullptr, description(), currentPath, tr("%1 compatible file (%2);;All Files (*.*)").arg(displayableName()).arg(createAcceptedExtensionString(" *")));

    if(finalFilePath.isEmpty())
        return false;

    if(m_filePath != finalFilePath)
    {
        QFileInfo info(finalFilePath);

        setDefaultDirPath(info.dir().path());
        setFilePath(finalFilePath);
    }

    return true;
}

void CT_AbstractStepLoadFile::setDefaultDirPath(const QString &path)
{
    m_defaultDirPath = path;
}

QString CT_AbstractStepLoadFile::createAcceptedExtensionString(const QString &preString) const
{
    QString tmp;

    const QList<FileFormat> fileFormats = fileExtensionAccepted();

    for(const FileFormat& ff : fileFormats) {
        for(const QString& ext : ff.suffixes()) {
            tmp += preString + "." + ext;
        }
    }

    return tmp;
}

double CT_AbstractStepLoadFile::stringToDouble(char* data, bool little_endian)
{
    qint64 result = 0;

    if(little_endian)
    {
        for(int i=7; i>=0; --i)
            result = (result << 8) | ((quint8)data[i]);
    }
    else
    {
        for(int i=0; i<8; ++i)
            result = (result << 8) | ((quint8)data[i]);
    }

    // conversion from long to double (from the doc of java methode longBitsToDouble).
    const int s = ((result >> 63) == 0) ? 1 : -1;

#ifndef _MSC_VER
    const int e = (int)((result >> 52) & 0x7ffLLU);
    const qint64 m = (e == 0) ?
                     (result & 0xfffffffffffffLLU) << 1 :
                     (result & 0xfffffffffffffLLU) | 0x10000000000000LLU;
#else
    const int e = (int)((result >> 52) & 0x7ffULL);
    const qint64 m = (e == 0) ?
                     (result & 0xfffffffffffffULL) << 1 :
                     (result & 0xfffffffffffffULL) | 0x10000000000000ULL;
#endif

    return pow(2.0, e-1075) * m * s;
}
