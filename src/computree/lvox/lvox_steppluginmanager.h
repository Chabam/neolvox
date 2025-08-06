/****************************************************************************

 Copyright (C) 2012-2012 Universite de Sherbrooke, Quebec, CANADA
                     All rights reserved.

 Contact :  richard.fournier@usherbrooke.ca
            jean-francois.cote@nrcan-rncan.gc.ca
            joris.ravaglia@gmail.com

 Developers : Joris RAVAGLIA
 Adapted by Alexandre Piboule for Computree 2.0

 This file is part of Computree version 2.0.

 Computree is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Computree is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Computree.  If not, see <http://www.gnu.org/licenses/>.

*****************************************************************************/

#ifndef LVOX_STEP_PLUGIN_MANAGER_H
#define LVOX_STEP_PLUGIN_MANAGER_H

#include "ct_abstractstepplugin.h"

class CT_AbstractReader;

class LVOX_StepPluginManager : public CT_AbstractStepPlugin
{
public:
    LVOX_StepPluginManager();
    ~LVOX_StepPluginManager();

    QString getPluginURL() const {return QString("https://computree.onf.fr/?page_id=1348");}

    virtual QString getPluginOfficialName() const override {return "LVOX";}

    QString getPluginRISCitation() const;

    CT_AbstractReader *readerAvailableByUniqueName(const QString &readerClassName) const;
    const QList<CT_AbstractReader *>& readersAvailable() const;

protected:

    bool loadGenericsStep();
    bool loadOpenFileStep();
    bool loadCanBeAddedFirstStep();
    bool loadActions();
    bool loadExporters();
    bool loadReaders();

    bool loadAfterAllPluginsLoaded() final;


    QList<CT_AbstractReader*>       m_readersOfAllPlugins;

private:
    void initReadersCollection();

};

#endif // LVOX_STEP_PLUGIN_MANAGER_H
