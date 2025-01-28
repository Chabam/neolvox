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

#include "ct_abstractsteploadfileinscene.h"

#include "ct_global/ct_cloudscontext.h"
#include "ct_global/ct_repositorymanager.h"
#include "ct_global/ct_repository.h"

#include "ct_iterator/ct_mutablepointiterator.h"
#include "ct_cloudindex/registered/ct_standardmodifiablecloudindexregisteredt.h"

#include <QDir>
#define _USE_MATH_DEFINES
#include <math.h>
#include <limits>

CT_AbstractStepLoadFileInScene::CT_AbstractStepLoadFileInScene() : CT_AbstractStepLoadFile()
{
}

void CT_AbstractStepLoadFileInScene::declareOutputModels(CT_StepOutModelStructureManager& manager)
{
    manager.addResult(m_hOutResult);
    manager.setRootGroup(m_hOutResult, m_hOutRootGroup);
    manager.addItem(m_hOutRootGroup, m_hOutScene, tr("Scene"));
}

void CT_AbstractStepLoadFileInScene::compute()
{
    setErrorCode(-1);

    if(QFile::exists(filePath()))
    {
        QFile f(filePath());

        if(f.open(QIODevice::ReadOnly))
        {
            setErrorCode(0);

            int offset = readHeaderFile(f);

            if(offset != -1)
                readDataFile(f, offset);

            f.close();
        }
    }
}

void CT_AbstractStepLoadFileInScene::readDataFile(QFile &f, int offset, bool little_endian)
{
    qint64 filesize = f.size();
    quint64 a = 0;
    //quint64 seek_to = offset;
    bool seek_ok = true;

    // un tableau de n_points
    quint64 n_points = (filesize - offset) / 26;

    /*if(_number_of_points_to_ignore > 0)
    {
        n_points = n_points/((quint64)_number_of_points_to_ignore);

        if(n_points == 0)
        {
            n_points = 1;
        }
    }*/

    CT_NMPCIR pcir = PS_REPOSITORY->createNewPointCloud(n_points);

    CT_MutablePointIterator it(pcir);
    CT_Point pReaded;

    double xmin = std::numeric_limits<float>::max();
    double ymin = std::numeric_limits<float>::max();
    double zmin = std::numeric_limits<float>::max();

    double xmax = -xmin;
    double ymax = -xmin;
    double zmax = -xmin;

    char d_data[8];

    while((a < n_points)
            && (seek_ok)
            && (!isStopped()))
    {
        f.read(d_data, 8);
        double x = stringToDouble(d_data, little_endian);
        f.read(d_data, 8);
        double y = stringToDouble(d_data, little_endian);
        f.read(d_data, 8);
        double z = stringToDouble(d_data, little_endian);
        f.read(d_data, 2);
        //getShort(d_data); // reflectance

        pReaded(0) = x;
        pReaded(1) = y;
        pReaded(2) = z;

        it.next();
        it.replaceCurrentPoint(pReaded);

        if (x<xmin) {xmin = (float)x;}

        if (x>xmax) {xmax = (float)x;}

        if (y<ymin) {ymin = (float)y;}

        if (y>ymax) {ymax = (float)y;}

        if (z<zmin) {zmin = (float)z;}

        if (z>zmax) {zmax = (float)z;}

        // progres de 0 a 100
        setProgress((a*100)/n_points);

        /*if(_number_of_points_to_ignore > 0)
        {
            seek_to += 26*_number_of_points_to_ignore;

            seek_ok = f.seek(seek_to);
        }*/

        ++a;
    }

    createOutResult(pcir, xmin, xmax, ymin, ymax, zmin, zmax);

}

void CT_AbstractStepLoadFileInScene::createOutResult(CT_PCIR pcir, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
    for(CT_ResultGroup* res : m_hOutResult.iterateOutputs()) {
        CT_StandardItemGroup* group = m_hOutRootGroup.createInstance();

        CT_Scene* scene = m_hOutScene.createInstance(pcir);
        scene->setBoundingBox(xmin, ymin, zmin, xmax, ymax, zmax);

        res->addRootGroup(m_hOutRootGroup, group);
        group->addSingularItem(m_hOutScene, scene);

        return;
    }
}
