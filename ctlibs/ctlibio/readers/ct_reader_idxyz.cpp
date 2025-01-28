#include "ct_reader_idxyz.h"
#include <QFile>
#include <QIODevice>
#include <QTextStream>

#include "ct_log/ct_logmanager.h"
#include "ct_point.h"
#include "ct_cloudindex/registered/ct_standardnotmodifiablecloudindexregisteredt.h"
#include "ct_pointcloudindex/ct_pointcloudindexvector.h"

#include <limits>

CT_Reader_IDXYZ::CT_Reader_IDXYZ(int subMenuLevel) : SuperClass(subMenuLevel)
{
    addNewReadableFormat(FileFormat("asc", tr("Fichiers de points ASCII (XYZRGB, sans entête, RGB [0;1])")));

    setToolTip(tr("Charge un fichier de clusters de points au format ASCII, avec les champs suivants :<br>"
                  "- ID : ID du cluster<br>"
                  "- X  : Coordonnée X<br>"
                  "- Y  : Coordonnée Y<br>"
                  "- Z  : Coordonnée Z<br>"));
}

CT_Reader_IDXYZ::CT_Reader_IDXYZ(const CT_Reader_IDXYZ& other) : SuperClass(other)
{
}

QString CT_Reader_IDXYZ::displayableName() const
{
    return tr("Clusters de Points, fichier ASCII (ID, X, Y, Z)");
}

bool CT_Reader_IDXYZ::preVerifyFile(const QString& filepath, QFile& fileOpenReadOnly) const
{
    QTextStream stream(&fileOpenReadOnly);

    stream.readLine();
    QString line = stream.readLine();
    if (!line.isNull())
    {
        QStringList values = line.split("\t");
        if (values.size() >= 4) {
            return true;
        } else {
            PS_LOG->addMessage(LogInterface::error, LogInterface::reader, QString(tr("Le fichier %1 n'a pas le bon format (colonnes manquantes).")).arg(filepath));
        }
    } else {
        PS_LOG->addMessage(LogInterface::error, LogInterface::reader, QString(tr("Le fichier %1 est vide.")).arg(filepath));
    }

    return false;
}


void CT_Reader_IDXYZ::internalDeclareOutputModels(CT_ReaderOutModelStructureManager& manager)
{
    manager.addItem(m_hOutScene, tr("Scène complète"));
    manager.addGroup(m_hOutSceneIdGroup, tr("Groupe"));
    manager.addItem(m_hOutSceneIdGroup, m_hOutSceneId, tr("Scène ID"));
}

bool CT_Reader_IDXYZ::internalReadFile(CT_StandardItemGroup* group)
{
    // Test File validity
    if(QFile::exists(filepath()))
    {
        QFile f(filepath());

        if (f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream stream(&f);

            CT_AbstractUndefinedSizePointCloud* pointCloud = PS_REPOSITORY->createNewUndefinedSizePointCloud();

            QString line;
            CT_Point pReaded;

            // Getting the file size to set progress
            qint64 fileSize = f.size();
            qint64 currentSizeRead = 0;

            QList<size_t> indices;
            QMap<size_t, CT_PointCloudIndexVector*> indexVectors;

            stream.readLine();

            while(!stream.atEnd()
                  && !isStopped())
            {
                line = stream.readLine();
                currentSizeRead += line.size();
                QStringList values = line.split("\t", QT_SKIP_EMPTY_PARTS);

                if (values.size() >= 4)
                {
                    bool okID = false;
                    bool okX = false;
                    bool okY = false;
                    bool okZ = false;

                    size_t id = values.at(0).toDouble(&okID);
                    double x  = values.at(1).toDouble(&okX);
                    double y = values.at(2).toDouble(&okY);
                    double z = values.at(3).toDouble(&okZ);

                    if (okID && okX && okY && okZ)
                    {
                        pReaded(0) = x;
                        pReaded(1) = y;
                        pReaded(2) = z;

                        pointCloud->addPoint(pReaded);
                        indices.append(id);
                        if (! indexVectors.contains(id)) {indexVectors.insert(id, new CT_PointCloudIndexVector());}
                    }
                }

                setProgress((currentSizeRead*100)/fileSize);
            }

            f.close();

            if (pointCloud->size() > 0)
            {
                CT_NMPCIR pcir = PS_REPOSITORY->registerUndefinedSizePointCloud(pointCloud);
                CT_Scene *scene = new CT_Scene(pcir);
                scene->updateBoundingBox();
                group->addSingularItem(m_hOutScene, scene);

                const CT_AbstractPointCloudIndex* sceneIndexVector =  scene->pointCloudIndex();

                for (int i = 0 ; i < indices.size() ; i++)
                {
                    size_t indice = indices.at(i);
                    CT_PointCloudIndexVector* indexVector = indexVectors.value(indice, nullptr);

                    if (indexVector != nullptr)
                    {
                        size_t pointIndex = sceneIndexVector->indexAt(i);
                        indexVector->addIndex(pointIndex);
                    }
                }

                QMapIterator<size_t, CT_PointCloudIndexVector*> it(indexVectors);
                while (it.hasNext())
                {
                    it.next();
                    size_t index = it.key();
                    CT_PointCloudIndexVector* indexVector = it.value();

                    if (indexVector != nullptr)
                    {
                        if (indexVector->size() > 0)
                        {
                            CT_Scene *sceneId = new CT_Scene(PS_REPOSITORY->registerPointCloudIndex(indexVector));
                            sceneId->updateBoundingBox();
                            sceneId->setDisplayableName(QString("Scene_%1").arg(QVariant::fromValue(index).toString()));

                            CT_StandardItemGroup* groupSc = new CT_StandardItemGroup();
                            group->addGroup(m_hOutSceneIdGroup, groupSc);
                            groupSc->addSingularItem(m_hOutSceneId, sceneId);

                        } else {
                            delete indexVector;
                        }
                    }
                }

                return true;

            } else
            {
                delete pointCloud;
            }
        } else {
            PS_LOG->addMessage(LogInterface::error, LogInterface::reader, QString(tr("Le fichier %1 n'est pas accessible.")).arg(filepath()));
        }
    } else {
        PS_LOG->addMessage(LogInterface::error, LogInterface::reader, QString(tr("Le fichier %1 n'existe pas.")).arg(filepath()));
    }

    return false;
}
