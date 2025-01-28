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

#ifndef CT_ABSTRACTSTEPLOADFILE_H
#define CT_ABSTRACTSTEPLOADFILE_H

#include "ct_abstractstep.h"

/**
 * @brief Represent a step that can be used in root of a workflow to load a file. It will
 *        help the developper because the dialog to request the file to load and the process
 *        to save/restore settings was already writted.
 *
 *        The developper must redefine methods below :
 *
 * CT_VirtualAbstractStep* createNewInstance()                                  // create a empty copy of your object
 * void declareInputModels(CT_StepInModelStructureManager& manager)             // declare INPUTS
 * void declareOutputModels(CT_StepOutModelStructureManager& manager)           // declare OUTPUTS
 * void compute()                                                               // your process !
 */
class CTLIBSTEP_EXPORT CT_AbstractStepLoadFile : public CT_AbstractStep
{
    Q_OBJECT
    typedef CT_AbstractStep SuperClass;

public:
    CT_AbstractStepLoadFile();

    void savePreSettings(SettingsWriterInterface& writer) const override;
    bool restorePreSettings(SettingsReaderInterface &reader) override;

    /**
     * @brief Returns true if the filepath passed in parameter is accepted
     * @param filePath : path to the file
     */
    virtual bool acceptFile(const QString& filePath, bool* allAccepted = 0) const;

    /**
     * @brief Returns the list of extension accepted by this step
     */
    virtual QList<FileFormat> fileExtensionAccepted() const = 0;

    /**
     * @brief Set the path of the file to read
     * @return true if this file can be read
     */
    virtual bool setFilePath(const QString& filePath);

    /**
     * @brief Returns the path to the file that must be or was readed
     */
    QString filePath() const;

    /**
     * @brief Returns the path to the last directory used in open file dialog
     */
    QString defaultDirPath() const;

protected:
    bool postInputConfigure() override;

    /**
     * @brief Set the path to the directory to use when the next open file dialog will be displayed
     */
    void setDefaultDirPath(const QString &path);

    /**
     * @brief Append all extensions in one string
     * @param preString : pre string to add to all extension. Typically " *".
     * @return the results string. If preString == " *" and extensions are ".xxx" and ".yyy" the results will be " *.xxx *.yyy".
     */
    virtual QString createAcceptedExtensionString(const QString& preString) const;

    /**
     * @brief Convert a string to a double
     * @param data : the string to convert
     * @param little_endian : true if data is in littleEndian
     * @return data converted to double
     */
    static double stringToDouble(char* data, bool little_endian = false);

private:
    QString     m_defaultDirPath;
    QString     m_filePath;
};

#endif // CT_ABSTRACTSTEPLOADFILE_H
